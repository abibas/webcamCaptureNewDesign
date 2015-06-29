#ifndef MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
#define MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H

#include <backend_interface.h>

#include <thread>

#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <dbt.h>
#include <ks.h>

namespace webcam_capture {
    class MediaFoundation_CameraNotifications
    {
    public:
        MediaFoundation_CameraNotifications();
        ~MediaFoundation_CameraNotifications();
        void Start(notifications_callback cb);
        void Stop();
    private:
        void MessageLoop();

        ///If we are using Media Foundation in new stream in have to be inited in those stream.
        bool InitMediaFoundation();
        void DeinitMediaFoundation();

        static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        std::vector<CameraInformation*> GetAvailableCamerasWithLinks();
        void CameraWasRemoved(DEV_BROADCAST_HDR *pHdr);
        void CameraWasConnected(DEV_BROADCAST_HDR *pHdr);


        //unordered map of WCHAR* - symbolic link and CameraInformation
        //move WCHAR to std::string
        std::vector<CameraInformation*> devicesVector;
        HDEVNOTIFY hDevNotify;
        WNDCLASS windowClass;
        HWND messageWindow;
        notifications_callback notif_cb;    //TODO to make threads synchronization
        std::thread messageLoopThread;
    };
}

#endif // MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
