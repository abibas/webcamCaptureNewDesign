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

    /**
     * Starts listening for device state change events.
     * @param Callback to call when device state changes.
     * @return true on success, false if already running (was previously stated but wasn't stopped).
     */
    bool start(CameraConnectionStateCallback cb);

    /**
     * Stops listening for device state change events.
     * @return true on success, false if already stopped or never was started.
     */
    bool stop();

private:
    /**
     * Silently fails, preserving the internal state of the thread being running.
     */
    void MessageLoop();
    static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void onCameraConnectionStateChanged(DEV_BROADCAST_HDR *pHdr, CameraConnectionState state);


    BackendImplementation implementation;
    std::unique_ptr<BackendInterface> backend;
    std::vector<CameraInformation> cameraList;
    HDEVNOTIFY deviceNotify;
    WNDCLASS windowClass;
    HWND messageWindow;

    bool threadIsRunning;
    CameraConnectionStateCallback cameraConnectionCallback;
    std::thread messageLoopThread;
};
}

#endif // MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
