#include "av_foundation_backend.h"
#include "av_foundation_camera.h"
#include "../utils.h"
#include "av_foundation_unique_id.h"
//#include "av_foundation_utils.h"

#include <string>

using namespace std::placeholders; //for std::bind _1

namespace webcam_capture {

AVFoundation_Backend::AVFoundation_Backend()
    : BackendInterface(BackendImplementation::MediaFoundation),
      mfDeinitializer(this, AVFoundation_Backend::DeinitBackend),
      avFoundationBackendObjectiveCInterface(NULL),
      connectionStatusCallback(nullptr)
{

}

void AVFoundation_Backend::DeinitBackend(void *)
{
    DEBUG_PRINT("AVFoundation_Backend Successfully deinited.\n");
}

std::vector<CameraInformation> AVFoundation_Backend::getAvailableCameras() const
{
    std::vector<AVFoundationDeviceInfo> devicesInfo;
    webcam_capture_av_get_devices(devicesInfo);
    std::vector<CameraInformation> result;
    for (int i = 0; i < devicesInfo.size(); i++) {
        std::shared_ptr<webcam_capture::AVFoundation_UniqueId> uniqueId =
                std::make_shared<webcam_capture::AVFoundation_UniqueId>(devicesInfo.at(i).deviceId);
        CameraInformation found_device(uniqueId, devicesInfo.at(i).deviceName);
        result.push_back(found_device);
    }
    return result;
}

std::unique_ptr<webcam_capture::CameraInterface> AVFoundation_Backend::getCamera(const CameraInformation &information) const
{
    return AVFoundation_Camera::createCamera(mfDeinitializer, information);
}

int AVFoundation_Backend::setAvaliableCamerasChangedCallback(ConnectionStatusCallback callback)
{
    connectionStatusCallback = callback;
    //IF n_callback is null_ptr or n_callback function is empty
    if (!callback) {
        if (avFoundationBackendObjectiveCInterface) {
            webcam_capture_av_stop_camera_notifications(avFoundationBackendObjectiveCInterface);
            webcam_capture_backend_dealloc(avFoundationBackendObjectiveCInterface);
            avFoundationBackendObjectiveCInterface = NULL;
        }
        return -1;      //TODO Err code
    }

    //notifications already started
    if (avFoundationBackendObjectiveCInterface) {
        DEBUG_PRINT("AVFoundation_Backend notification manager already started.\n");
        return -2;
    }
    avFoundationBackendObjectiveCInterface = webcam_capture_backend_alloc();
    webcam_capture_av_start_camera_notifications(avFoundationBackendObjectiveCInterface,
                                                 std::bind(&AVFoundation_Backend::AVFoundationCameraConnected, this, _1, _2),
                                                 std::bind(&AVFoundation_Backend::AVFoundationCameraDisconnected, this, _1, _2));
    return 1; //TODO ERR code (success)
}

void AVFoundation_Backend::AVFoundationCameraConnected(std::string name, std::string id)
{
    if (connectionStatusCallback) {
        std::shared_ptr<webcam_capture::AVFoundation_UniqueId> uniqueId =
                std::make_shared<webcam_capture::AVFoundation_UniqueId>(id);
        CameraInformation device(uniqueId, name);
        connectionStatusCallback(device, CameraConnectionStatus::Connected);
    }
}

void AVFoundation_Backend::AVFoundationCameraDisconnected(std::string name, std::string id) {
    if (connectionStatusCallback) {
        std::shared_ptr<webcam_capture::AVFoundation_UniqueId> uniqueId =
                std::make_shared<webcam_capture::AVFoundation_UniqueId>(id);
        CameraInformation device(uniqueId, name);
        connectionStatusCallback(device, CameraConnectionStatus::Disconnected);
    }
}

} // namespace webcam_capture
