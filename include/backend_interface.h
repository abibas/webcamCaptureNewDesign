/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <functional>
#include <stdio.h>
#include <string>
#include <vector>

#include <camera_information.h>
#include <camera_interface.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

    enum class WEBCAM_CAPTURE_EXPORT CameraPlugStatus {
        CAMERA_CONNECTED,
        CAMERA_DISCONNECTED
    };

    /**
    * notifications_callback Typedef for nnotifications callback
    */
    typedef std::function<void(CameraInformation* information, CameraPlugStatus status)> notifications_callback;

    /**
     * Contains Interface for Backend realization
     */
    class WEBCAM_CAPTURE_EXPORT BackendInterface {
    public:
        BackendInterface() {}
        virtual ~BackendInterface() {}
        /**
         * @return Vectod of avaliable cameras
         */
        virtual std::vector<CameraInformation*> getAvailableCameras() const = 0;
        /**
         * @param information Camera information with Id and Name
         * @return New camera instance
         */
        virtual CameraInterface* getCamera(const CameraInformation &information) const = 0;
        /**
         * @param n_callback Notifications callback
         */
        virtual int setAvaliableCamerasChangedCallback(notifications_callback n_callback) = 0; //TODO to realize
    };

} // namespace webcam_capture

#endif
