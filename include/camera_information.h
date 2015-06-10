#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H

namespace webcamCapture {

    class CameraInformation {
    public:
      CameraInformation(int cameraId, std::string cameraName) : cameraId(cameraId), cameraName (cameraName) {}
      ~CameraInformation();

      int getDeviceId() const { return cameraId; }
      std::string getCameraName() const { return cameraName; }

    private:
      const int cameraId;
      const std::string cameraName;
    };

} // namespace webcamCapture

#endif // CAMERA_INFORMATION_H

