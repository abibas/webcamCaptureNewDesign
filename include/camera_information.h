/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H

#include <string.h>

#include <unique_id.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Contains informationa about single camera
 */
class WEBCAM_CAPTURE_EXPORT CameraInformation
{
public:
    /**
    * @param cameraId Camera Id
    * @param cameraName Camera Name
    */
    CameraInformation(UniqueId *uniqueId, std::string cameraName) : uniqueId(uniqueId), cameraName(cameraName) {}
    ~CameraInformation() {}


    /**
     * @return Camera unique Id
     */
    UniqueId *getUniqueId() const
    {
        return uniqueId;
    }

    /**
     * @return Camera Name
     */
    std::string getCameraName() const
    {
        return cameraName;
    }

private:
    std::string cameraName;
    UniqueId *uniqueId;
};
} // namespace webcam_capture

#endif // CAMERA_INFORMATION_H

