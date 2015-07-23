#ifndef AV_FOUNDATION_BACKEND_H
#define AV_FOUNDATION_BACKEND_H

#include "av_foundation_interface.h"
#include <backend_interface.h>
#include <camera_information.h>
#include <camera_interface.h>

#include <vector>
#include <memory>

namespace webcam_capture {

class AVFoundation_Backend : public BackendInterface
{
public:
    AVFoundation_Backend();
    static void DeinitBackend(void *);
    std::vector<CameraInformation> getAvailableCameras() const;
    std::unique_ptr<CameraInterface> getCamera(const CameraInformation &information) const;
    int setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback);

private:
    std::shared_ptr<void> mfDeinitializer; 
};

} // namespace webcam_capture

#endif // AV_FOUNDATION_BACKEND_H
