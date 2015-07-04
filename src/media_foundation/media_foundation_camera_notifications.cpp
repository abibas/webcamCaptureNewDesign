#include "media_foundation_camera_notifications.h"
#include "../media_foundation/media_foundation_backend.h"
#include "../media_foundation/media_foundation_unique_id.h"
#include "../utils.h"

#include <iostream>
#include <ks.h>
#include <Ksmedia.h>
#include <wctype.h>
#include <windows.h>
#include <algorithm>

#define QUIT_FROM_NOTIFICATIONS_LOOP  0x01

namespace webcam_capture {

    LRESULT CALLBACK MediaFoundation_CameraNotifications::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        MediaFoundation_CameraNotifications* pParent;
        switch (uMsg) {
            case WM_CREATE: {
                pParent = (MediaFoundation_CameraNotifications*)((LPCREATESTRUCT)lParam)->lpCreateParams;
                SetWindowLongPtr(hWnd,GWLP_USERDATA,(LONG_PTR)pParent);
                //If we are using Media Foundation in new stream it have to be inited in those stream.
                pParent->devicesVector = pParent->backend->getAvailableCameras();
                break;
            }
            case WM_DEVICECHANGE: {
                pParent = (MediaFoundation_CameraNotifications*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
                if(!pParent) break;

                if (wParam == DBT_DEVICEARRIVAL) {
                    DEBUG_PRINT("A webcam device has been inserted and is now available.\n");
                    pParent->CameraWasConnected((PDEV_BROADCAST_HDR)lParam);
                }
                if (wParam == DBT_DEVICEREMOVECOMPLETE) {
                    DEBUG_PRINT("A webcam device has been removed.\n");
                    pParent->CameraWasRemoved((PDEV_BROADCAST_HDR)lParam);
                }
                break;
            }
            case WM_USER: {
                if (lParam == QUIT_FROM_NOTIFICATIONS_LOOP) {
                    PostQuitMessage(0);
                    break;
                }
            }
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    MediaFoundation_CameraNotifications::MediaFoundation_CameraNotifications()
        :   backend(nullptr),
            notif_cb(nullptr),
            stopMessageThread(false),
            messageLoopThread() {
        backend = new MediaFoundation_Backend();
    }

    MediaFoundation_CameraNotifications::~MediaFoundation_CameraNotifications(){
        if ( !stopMessageThread ) {
            this->Stop();
        }
        delete backend;
    }

    void MediaFoundation_CameraNotifications::Start(notifications_callback cb){
        notif_cb = cb;
        stopMessageThread = false;
        messageLoopThread = std::thread(&MediaFoundation_CameraNotifications::MessageLoop, this);
        DEBUG_PRINT("Notifications apturing was started.\n");
    }

    void MediaFoundation_CameraNotifications::Stop(){        
        //TODO executes 2 times - to fix this.
        stopMessageThread = true;
        SendMessageA(messageWindow, WM_USER, 0, QUIT_FROM_NOTIFICATIONS_LOOP);
        if( messageLoopThread.joinable() ) messageLoopThread.join();
        UnregisterDeviceNotification(hDevNotify);
        DestroyWindow(messageWindow);
        UnregisterClassA(windowClass.lpszClassName, NULL);
        notif_cb = nullptr;               
        //delete all records from cameraInfo vector
        for (int i = 0; i < devicesVector.size(); i++){
            delete devicesVector.at(i);
        }

        DEBUG_PRINT("Notifications capturing was stopped.\n");
    }

    void MediaFoundation_CameraNotifications::MessageLoop(){
        //create stream to move to new stream
        windowClass = {};
        windowClass.lpfnWndProc = &MediaFoundation_CameraNotifications::WindowProcedure;

        LPCSTR windowClassName = "CameraNotificationsMessageOnlyWindow";
        windowClass.lpszClassName = windowClassName;

        if (!RegisterClass(&windowClass)) {
            DEBUG_PRINT("Failed to register window class.\n");
            return;
        }

        messageWindow = CreateWindowA(windowClassName,0 ,0 ,0 ,0 ,0 ,0 , HWND_MESSAGE ,0 ,0 ,this);
        if (!messageWindow) {
            DEBUG_PRINT("Failed to create message-only window.\n");
            return;
        }

        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
        ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

        NotificationFilter.dbcc_size = sizeof(NotificationFilter);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        //can be used KSCATEGORY_CAPTURE. But KSCATEGORY_CAPTURE - gets audio/video messages device arrival and removed (2 messages)
        NotificationFilter.dbcc_classguid  = KSCATEGORY_VIDEO;


        hDevNotify = RegisterDeviceNotificationW(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

        MSG msg;
        GetMessage(&msg, messageWindow, 0, 0);

        while ( !stopMessageThread && GetMessage(&msg, messageWindow, 0, 0) > 0 ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void MediaFoundation_CameraNotifications::CameraWasRemoved(DEV_BROADCAST_HDR *pHdr){
        DEV_BROADCAST_DEVICEINTERFACE_W *pDi = (DEV_BROADCAST_DEVICEINTERFACE_W*)pHdr;
        UniqueId * uniqId = new MediaFoundation_UniqueId(pDi->dbcc_name);
        for (int i = 0; i < devicesVector.size(); i++) {
            if ( *uniqId == *devicesVector.at(i)->getUniqueId() ) {
                notif_cb(devicesVector.at(i), CameraPlugStatus::CAMERA_DISCONNECTED);
                devicesVector.erase(devicesVector.begin() + i);
                break;
            }
        }
    }

    void MediaFoundation_CameraNotifications::CameraWasConnected(DEV_BROADCAST_HDR *pHdr){
        DEV_BROADCAST_DEVICEINTERFACE_W *pDi = (DEV_BROADCAST_DEVICEINTERFACE_W*)pHdr;
        UniqueId * uniqId = new MediaFoundation_UniqueId(pDi->dbcc_name);
        std::vector <CameraInformation*> camerasBuf = backend->getAvailableCameras();
        for (int i = 0; i < camerasBuf.size(); i++) {
            if ( *camerasBuf.at(i)->getUniqueId() == *uniqId ) {
                CameraInformation * camNotifRes = new CameraInformation(*camerasBuf.at(i)); // creates a copy of object
                notif_cb(camNotifRes, CameraPlugStatus::CAMERA_CONNECTED);
                devicesVector.push_back(camerasBuf.at(i));
            } else {
                delete camerasBuf.at(i);
            }
        }
    }
}// namespace webcam_capture
