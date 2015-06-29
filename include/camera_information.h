/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H
#include <string.h>
#include <unique_id_interface.h>

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
      CameraInformation(UniqueIdInterface *uniqueId, std::string cameraName) : uniqueId(uniqueId), cameraName (cameraName) {}
      ~CameraInformation() {}

      /**
       * @return Camera Id
       */
      //int getDeviceId() const { return cameraId; }


      /**
       * @return Camera unique Id
       */
      UniqueIdInterface * getUniqueId() { return uniqueId; }

      /**
       * @return Camera Name
       */
      std::string getCameraName() const { return cameraName; }

    private:
      //const int cameraId;
      std::string cameraName;
      UniqueIdInterface * uniqueId;

    };

} // namespace webcam_capture

#endif // CAMERA_INFORMATION_H

