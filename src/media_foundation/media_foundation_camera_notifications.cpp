#include "media_foundation_camera_notifications.h"
#include "../utils.h"

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <dbt.h>
#include <ks.h>

#pragma comment(lib, "user32.lib")

namespace webcam_capture {

    LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      if (uMsg == WM_DEVICECHANGE) {
          if (wParam == DBT_DEVICEARRIVAL) {
              DEBUG_PRINT("A device or piece of media has been inserted and is now available.\n");
          }
          if (wParam == DBT_DEVICEREMOVECOMPLETE) {
              DEBUG_PRINT("A device or piece of media has been removed.\n");
          }
      }
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }


    MediaFoundation_CameraNotifications::MediaFoundation_CameraNotifications()
        :   threadStop(FALSE),
            notif_cb(nullptr),
            messageLoopThread(nullptr)
    {
    }

    MediaFoundation_CameraNotifications::~MediaFoundation_CameraNotifications(){
    }

    void MediaFoundation_CameraNotifications::Start(notifications_callback cb){
        notif_cb = cb;
        messageLoopThread(&MediaFoundation_CameraNotifications::MessageLoop);
    }

    void MediaFoundation_CameraNotifications::Stop(){
        notif_cb = nullptr;
    }

    void MediaFoundation_CameraNotifications::MessageLoop(){

        //create stream to move to new stream
        WNDCLASS windowClass = {};
        windowClass.lpfnWndProc = WindowProcedure;
        LPCWSTR windowClassName = L"CameraNotificationsMessageOnlyWindow";
        windowClass.lpszClassName = windowClassName;

        if (!RegisterClass(&windowClass)) {
            DEBUG_PRINT("Failed to register window class.\n");
            return;
        }
        HWND messageWindow = CreateWindow(windowClassName, 0, 0, 0, 0, 0, 0, HWND_MESSAGE, 0, 0, 0);
        if (!messageWindow) {
            DEBUG_PRINT("Failed to create message-only window.\n");
            return;
        }

        DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
        ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));

        NotificationFilter.dbcc_size = sizeof(NotificationFilter);
        NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        NotificationFilter.dbcc_classguid  = KSCATEGORY_CAPTURE;


        //NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;

        HDEVNOTIFY hDevNotify = RegisterDeviceNotification(messageWindow, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

        //mesagesDispatchThread()

        MSG msg;
        while (GetMessage(&msg, 0, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
