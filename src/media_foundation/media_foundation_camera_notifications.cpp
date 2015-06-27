#include "media_foundation_camera_notifications.h"
#include "../media_foundation/media_foundation_backend.h"
#include "../utils.h"



#pragma comment(lib, "user32.lib")

namespace webcam_capture {

    MediaFoundation_CameraNotifications::MediaFoundation_CameraNotifications()
        :   threadStop(false),
            notif_cb(nullptr),
            messageLoopThread()
    {
    }

    MediaFoundation_CameraNotifications::~MediaFoundation_CameraNotifications(){
        this->Stop();
    }

    void MediaFoundation_CameraNotifications::Start(notifications_callback cb){
        threadStop = false;
        notif_cb = cb;
        messageLoopThread = std::thread(&MediaFoundation_CameraNotifications::MessageLoop, this);
    }

    void MediaFoundation_CameraNotifications::Stop(){
        threadStop = true;
        UnregisterDeviceNotification(hDevNotify);
        if( messageLoopThread.joinable() ) messageLoopThread.join();
        notif_cb = nullptr;
    }

    void MediaFoundation_CameraNotifications::MessageLoop(){
        this->InitMediaFoundation();

        //create stream to move to new stream
        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = &MediaFoundation_CameraNotifications::WindowProcedure;
        LPCWSTR windowClassName = L"CameraNotificationsMessageOnlyWindow";
        windowClass.lpszClassName = windowClassName;

        if (!RegisterClassEx(&windowClass)) {
            DEBUG_PRINT("Failed to register window class.\n");
            return;
        }

        messageWindow = CreateWindowEx(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        if (!messageWindow) {
            DEBUG_PRINT("Failed to create message-only window.\n");
            return;
        }

        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
        ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

        NotificationFilter.dbcc_size = sizeof(NotificationFilter);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        NotificationFilter.dbcc_classguid  = KSCATEGORY_CAPTURE;


        hDevNotify = RegisterDeviceNotification(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

        MSG msg;
        while ( !threadStop && (GetMessage(&msg, messageWindow, 0, 0) > 0) ) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        this->DeinitMediaFoundation();
    }

    LRESULT CALLBACK MediaFoundation_CameraNotifications::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        if (uMsg == WM_DEVICECHANGE) {
          if (wParam == DBT_DEVICEARRIVAL) {
              DEBUG_PRINT("A device or piece of media has been inserted and is now available.\n");
              CameraInformation inf(1,"ddf");
              //notif_cb(inf);
          }
          if (wParam == DBT_DEVICEREMOVECOMPLETE) {
              DEBUG_PRINT("A device or piece of media has been removed.\n");
          }
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    bool MediaFoundation_CameraNotifications::InitMediaFoundation(){
        // Initialize COM
        HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: cannot intialize COM.\n");  //or The COM is already initialized.
        }

        // Initialize MediaFoundation
        hr = MFStartup(MF_VERSION);
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: cannot startup the Media Foundation.\n");
            return false;
        }
        return true;
    }

    void MediaFoundation_CameraNotifications::DeinitMediaFoundation(){
        /* Shutdown MediaFoundation */
        HRESULT hr = MFShutdown();
        if(FAILED(hr)) {
            DEBUG_PRINT("Error: failed to shutdown the MediaFoundation.\n");
        }

        /* Shutdown COM */
        CoUninitialize();
        DEBUG_PRINT("MediaFoundation_Backend Successfully deinited.\n");
    }
}
