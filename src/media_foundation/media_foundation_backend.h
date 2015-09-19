#ifndef MEDIA_FOUNDATION_BACKEND_H
#define MEDIA_FOUNDATION_BACKEND_H

#include "../winapi_shared/winapi_shared_camera_notifications.h"

#include <backend_interface.h>
#include <camera_information.h>
#include <camera_interface.h>

#include <vector>
#include <memory>

namespace webcam_capture {

class MediaFoundation_Backend : public BackendInterface
{
public:
    /**
     * Returns BackendInterface pointer on success, null on failure.
     */
    static std::unique_ptr<BackendInterface> create();
    std::vector<CameraInformation> getAvailableCameras() const;
    std::unique_ptr<CameraInterface> getCamera(const CameraInformation &information) const;
    int setCameraConnectionStateCallback(CameraConnectionStateCallback callback);

private:
    MediaFoundation_Backend(std::shared_ptr<void> &deinitializer);
    static void deinitialize(void *, bool deinitializeCom);

    std::shared_ptr<void> deinitializer;
    WinapiShared_CameraNotifications notificationManager;
};

} // namespace webcam_capture

#endif // MEDIA_FOUNDATION_BACKEND_H
