#ifndef MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
#define MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H

#include <backend_interface.h>

#include <thread>

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
        bool threadStop;
        notifications_callback notif_cb;
        std::thread messageLoopThread;
    };
}

#endif // MEDIA_FONDATION_CAMERA_NOTIFICATIONS_H
