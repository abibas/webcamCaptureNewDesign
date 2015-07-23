#include "av_foundation_backend.h"

#include "../utils.h"

//#include "av_foundation_camera.h"
//#include "av_foundation_utils.h"

#include <string>

namespace webcam_capture {

AVFoundation_Backend::AVFoundation_Backend()
    : BackendInterface(BackendImplementation::MediaFoundation),
      mfDeinitializer(this, AVFoundation_Backend::DeinitBackend)
{

}

void AVFoundation_Backend::DeinitBackend(void *)
{
    DEBUG_PRINT("AVFoundation_Backend Successfully deinited.\n");
}

std::vector<CameraInformation> AVFoundation_Backend::getAvailableCameras() const
{
    std::vector<CameraInformation> result;
    webcam_capture_av_alloc();
    webcam_capture_av_get_devices(result);
    return result;
}

std::unique_ptr<webcam_capture::CameraInterface> AVFoundation_Backend::getCamera(const CameraInformation &information) const
{
    //return AVFoundation_Camera::createCamera(mfDeinitializer, information);
    return nullptr;
}

int AVFoundation_Backend::setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback)
{
    return 1;
}

} // namespace webcam_capture
