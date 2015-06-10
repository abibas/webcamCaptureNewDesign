#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <stdio.h>
#include <string>
#include <vector>
#include <functional>

#include "camera_interface.h"
#include "camera_information.h"

namespace webcam_capture {
    /**
    * notifications_callback Typedef for nnotifications callback
    */
    typedef std::function<void(CameraInformation& information)> notifications_callback;

    /**
     * Contains Interface for Backend realization
     */
    class BackendInterface {
    public:
        BackendInterface();
        virtual ~BackendInterface();
        /**
         * @return Vectod of avaliable cameras
         */
        virtual std::vector<CameraInformation> getAvaliableCameras() const = 0;
        /**
         * @param information Camera information with Id and Name
         * @return New camera instance
         */
        virtual CameraInterface* getCamera(const CameraInformation &information) const = 0;
        /**
         * @param n_callback Notifications callback
         */
        virtual void setAvaliableCamerasChangedCallback(notifications_callback n_callback) = 0; //TODO to realize
    };

} // namespace webcam_capture

#endif
