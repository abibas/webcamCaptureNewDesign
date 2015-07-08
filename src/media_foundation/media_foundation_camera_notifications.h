#ifndef MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
#define MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H

#include <backend_interface.h>

#include <thread>
#include <Windows.h>
#include <dbt.h>

namespace webcam_capture {

class MediaFoundation_Backend;

class MediaFoundation_CameraNotifications
{
public:
    MediaFoundation_CameraNotifications();
    ~MediaFoundation_CameraNotifications();
    void Start(notifications_callback cb);
    void Stop();
private:
    void MessageLoop();

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    //std::vector<CameraInformation*> GetAvailableCamerasWithLinks();
    void CameraWasRemoved(DEV_BROADCAST_HDR *pHdr);
    void CameraWasConnected(DEV_BROADCAST_HDR *pHdr);


    MediaFoundation_Backend *backend;
    std::vector<CameraInformation *> devicesVector;
    HDEVNOTIFY hDevNotify;
    WNDCLASS windowClass;
    HWND messageWindow;

    bool stopMessageThread;
    notifications_callback notif_cb;    //TODO to make threads synchronization
    std::thread messageLoopThread;
};
}

#endif // MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
