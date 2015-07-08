#ifndef WINAPI_SHARED_CAMERA_NOTIFICATIONS_H
#define WINAPI_SHARED_CAMERA_NOTIFICATIONS_H

#include <backend_interface.h>

#include <thread>
#include <Windows.h>
#include <dbt.h>

namespace webcam_capture {

class MediaFoundation_Backend;

class WinapiShared_CameraNotifications
{
public:
    WinapiShared_CameraNotifications(BackendImplementation implementation);
    ~WinapiShared_CameraNotifications();
    void Start(notifications_callback cb);
    void Stop();
    BackendInterface* getBackend();
    void setBackend(BackendInterface *backendInterface);
    BackendImplementation getBackendImplementation();

private:
    void MessageLoop();

    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CameraWasRemoved(DEV_BROADCAST_HDR *pHdr);
    void CameraWasConnected(DEV_BROADCAST_HDR *pHdr);


    BackendInterface *backend;
    BackendImplementation backendImplementation;
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
