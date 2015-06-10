#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H
#include <string.h>

namespace webcam_capture {

    /**
     * Contains informationa about single camera
     */
    class CameraInformation {
    public:
       /**
       * @param cameraId Camera Id
       * @param cameraName Camera Name
       */
      CameraInformation(int cameraId, std::string cameraName) : cameraId(cameraId), cameraName (cameraName) {}
      ~CameraInformation();

      /**
       * @return Camera Id
       */
      int getDeviceId() const { return cameraId; }
      /**
       * @return Camera Name
       */
      std::string getCameraName() const { return cameraName; }

    private:
      const int cameraId;
      const std::string cameraName;
    };

} // namespace webcam_capture

#endif // CAMERA_INFORMATION_H

