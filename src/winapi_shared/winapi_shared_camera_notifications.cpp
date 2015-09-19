#include "../utils.h"
#include "winapi_shared_camera_notifications.h"
#include "winapi_shared_unique_id.h"

#include <backend_factory.h>

#include <algorithm>

#include <ks.h>
#include <Ksmedia.h>
#include <wctype.h>

#define QUIT_FROM_NOTIFICATIONS_LOOP  0x01

namespace webcam_capture {

LRESULT CALLBACK WinapiShared_CameraNotifications::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam,
        LPARAM lParam)
{
    WinapiShared_CameraNotifications *pThis;

    switch (uMsg) {
        // guaranteed to be called only once, at the startup
        case WM_CREATE: {
            pThis = static_cast<WinapiShared_CameraNotifications *>(reinterpret_cast<CREATESTRUCT *>(lParam)->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);

            // initialize backend in new thread
            pThis->backend = BackendFactory::getBackend(pThis->implementation);
            if (!pThis->backend) {
                DEBUG_PRINT("Couldn't initialize backend.");
                break;
            }

            pThis->cameraList = pThis->backend->getAvailableCameras();
            break;
        }

        // called whenever a device that belongs to a group we have previously
        // registered to receive notifications of changes its state
        case WM_DEVICECHANGE: {
            pThis = reinterpret_cast<WinapiShared_CameraNotifications *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            if (!pThis) {
                DEBUG_PRINT("Couldn't get either this pointer or backend.");
                break;
            }

            if (!pThis->backend) {
                DEBUG_PRINT("Backend is still not initialized.");
                break;
            }

            if (wParam == DBT_DEVICEARRIVAL) {
                DEBUG_PRINT("A webcam device has been inserted and is now available.");
                pThis->onCameraConnectionStateChanged(reinterpret_cast<PDEV_BROADCAST_HDR>(lParam), CameraConnectionState::Connected);
            } else if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                DEBUG_PRINT("A webcam device has been removed.");
                pThis->onCameraConnectionStateChanged(reinterpret_cast<PDEV_BROADCAST_HDR>(lParam), CameraConnectionState::Disconnected);
            }

            break;
        }

        // called by us when WinapiShared_CameraNotifications::stop is called,
        // the rest of void WinapiShared_CameraNotifications::MessageLoop is being executed after we PostQuitMessage.
        case WM_USER: {
            if (lParam == QUIT_FROM_NOTIFICATIONS_LOOP) {
                DEBUG_PRINT("Received request to stop the loop.");
                PostQuitMessage(0);
                break;
            }
        }

        // guaranteed to be called only once, at the end of lifecycle when we call DestroyWindow on the window
        case WM_DESTROY: {
            pThis = reinterpret_cast<WinapiShared_CameraNotifications *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

            pThis->backend.reset();

            DEBUG_PRINT("Received destroy message.");

            break;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

WinapiShared_CameraNotifications::WinapiShared_CameraNotifications(BackendImplementation implementation) :
      implementation(implementation),
      backend(nullptr),
      cameraConnectionCallback(nullptr),
      threadIsRunning(false)
{    
}

WinapiShared_CameraNotifications::~WinapiShared_CameraNotifications()
{
    stop();
}

bool WinapiShared_CameraNotifications::start(CameraConnectionStateCallback cb)
{
    if (threadIsRunning) {
        return false;
    }

    cameraConnectionCallback = cb;
    messageLoopThread = std::thread(&WinapiShared_CameraNotifications::MessageLoop, this);
    threadIsRunning = true;
    DEBUG_PRINT("Notifications capturing was started.");

    return true;
}

bool WinapiShared_CameraNotifications::stop()
{
    if (!threadIsRunning) {
        return false;
    }

    SendMessage(messageWindow, WM_USER, 0, QUIT_FROM_NOTIFICATIONS_LOOP);

    if (messageLoopThread.joinable()) {
        messageLoopThread.join();
    }

    threadIsRunning = false;

    cameraList.clear();

    DEBUG_PRINT("Notifications capturing was stopped.");

    return true;
}

void WinapiShared_CameraNotifications::MessageLoop()
{
    windowClass = {};
    windowClass.lpfnWndProc = &WinapiShared_CameraNotifications::WindowProcedure;

    LPCSTR windowClassName = "CameraNotificationsMessageOnlyWindow";
    windowClass.lpszClassName = windowClassName;

    if (!RegisterClass(&windowClass)) {
        DEBUG_PRINT("Failed to register window class.");
        goto done;
    }

    messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, this);

    if (!messageWindow) {
        DEBUG_PRINT("Failed to create message-only window.");
        goto unregister_window_class;
    }

    DEV_BROADCAST_DEVICEINTERFACE notificationFilter;
    ZeroMemory(&notificationFilter, sizeof(notificationFilter));

    notificationFilter.dbcc_size = sizeof(notificationFilter);
    notificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    //can be used KSCATEGORY_CAPTURE. But KSCATEGORY_CAPTURE - gets audio/video messages device arrival and removed (2 messages)
    notificationFilter.dbcc_classguid = KSCATEGORY_VIDEO;


    deviceNotify = RegisterDeviceNotificationW(messageWindow, &notificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

    if (!deviceNotify) {
        DEBUG_PRINT("Failed to register for device notifications.");
        goto destroy_window;
    }

    MSG msg;

    while (GetMessage(&msg, messageWindow, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterDeviceNotification(deviceNotify);

destroy_window:
    DestroyWindow(messageWindow);

unregister_window_class:
    UnregisterClass(windowClass.lpszClassName, NULL);

done:
    cameraConnectionCallback = nullptr;

    DEBUG_PRINT("MessageLoop exited.");
}

void WinapiShared_CameraNotifications::onCameraConnectionStateChanged(DEV_BROADCAST_HDR *pHdr, CameraConnectionState state)
{
    if (pHdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
        DEBUG_PRINT("Unexpected device type.");
        return;
    }

    DEV_BROADCAST_DEVICEINTERFACE *deviceInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE *>(pHdr);
    size_t symbolicLinkLength = strlen(deviceInterface->dbcc_name);
    std::unique_ptr<WCHAR[]> symbolicLink(new WCHAR[symbolicLinkLength+1]);
    mbstowcs_s(nullptr, symbolicLink.get(), symbolicLinkLength+1, deviceInterface->dbcc_name, symbolicLinkLength);

    WinapiShared_UniqueId uniqueId(symbolicLink.get(), implementation);

    switch (state) {
        case CameraConnectionState::Connected: {
            std::vector<CameraInformation> camerasBuf = backend->getAvailableCameras();
            for (auto &&cameraInfo : camerasBuf) {
                if (*cameraInfo.getUniqueId() == uniqueId) {
                    cameraConnectionCallback(cameraInfo, CameraConnectionState::Connected);
                    cameraList.push_back(cameraInfo);
                    break;
                }
            }
            break;
        }

        case CameraConnectionState::Disconnected: {
            for (int i = 0; i < cameraList.size(); i++) {
                if (uniqueId == *cameraList[i].getUniqueId()) {
                    cameraConnectionCallback(cameraList[i], CameraConnectionState::Disconnected);
                    cameraList[i] = std::move(cameraList.back());
                    cameraList.pop_back();
                    break;
                }
            }
            break;
        }
    }
}

} // namespace webcam_capture
