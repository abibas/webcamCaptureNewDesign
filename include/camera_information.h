#ifndef CAMERA_INFORMATION_H
#define CAMERA_INFORMATION_H

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

#include <memory>
#include <string>

namespace webcam_capture {

class UniqueId;

/**
 * Provides a brief description of a camera.
 */
#ifdef _WIN32
    class WEBCAM_CAPTURE_EXPORT CameraInformation
#elif __APPLE__
    class CameraInformation
#endif
{
public:
    CameraInformation(std::shared_ptr<UniqueId> uniqueId, std::string cameraName) : uniqueId(uniqueId),
        cameraName(cameraName) {}
    ~CameraInformation() {}

    /**
     * @return Unique camera id identifying a camera.
     */
    std::shared_ptr<UniqueId> getUniqueId() const
    {
        return uniqueId;
    }

    /**
     * @return User-friendly camera name in system-specific string encoding.
     */
    std::string getCameraName() const
    {
        return cameraName;
    }

private:
    std::string cameraName;
    std::shared_ptr<UniqueId> uniqueId;
};

} // namespace webcam_capture

#endif // CAMERA_INFORMATION_H

