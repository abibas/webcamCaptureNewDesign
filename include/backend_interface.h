#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <camera_information.h>
#include <backend_implementation.h>

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

#include <functional>
#include <memory>
#include <vector>

namespace webcam_capture {

class CameraInterface;

enum class WEBCAM_CAPTURE_EXPORT CameraConnectionState {
    Connected, // camera was connected to the system and it's available for use
    Disconnected // camera was disconnected from the system and you should stop using it
};

typedef std::function<void(CameraInformation information, CameraConnectionState state)> CameraConnectionStateCallback;

/**
 * Common interface of backend implementations.
 * Provides access to cameras and information of their availability.
 */
class WEBCAM_CAPTURE_EXPORT BackendInterface
{
public:
    BackendInterface(BackendImplementation implementation) : implementation(implementation) {}
    virtual ~BackendInterface() {}

    /**
     * @return Underlying implementation this backend uses.
     */
    BackendImplementation getImplementation() const
    {
        return implementation;
    }

    /**
     * @return Short information about available cameras.
     */
    virtual std::vector<CameraInformation> getAvailableCameras() const = 0;

    /**
     * Creates a camera instance representing a specific camera.
     * Camera instance will function properly with its backend instance deleted.
     * Backend and camera instances should be created in the same thread and can be used only within that thread.
     * If you use them in different threads, the behaviour is undefined.
     * @param information Previously received camera information for the camera you want to access.
     * @return CameraInterface instance representing a specific camera on success, null on failure.
     */
    virtual std::unique_ptr<CameraInterface> getCamera(const CameraInformation &information) const = 0;

    /**
     * Sets a callback function that is be called when a camera is connected/disconencted to/from the system.
     * You can use the UniqueId member of CameraInformation for comparison against your camera instances in order
     * to check if any of them was removed from the system.
     * @param callback Callback function. Use nullptr or a default-constructed ConnectionStateCallback to unset
     * the previously set callback function.
     * @return FIXME(nurupo): the return value should be wither an enum or bool.
     */
    virtual int setCameraConnectionStateCallback(CameraConnectionStateCallback callback) = 0;

protected:
    const BackendImplementation implementation;
};

} // namespace webcam_capture

#endif // BACKEND_INTERFACE_H
