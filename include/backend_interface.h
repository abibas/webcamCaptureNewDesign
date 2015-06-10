#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <stdio.h>
#include <string>
#include <vector>
#include <functional>

#include "camera_interface.h"
#include "camera_information.h"

namespace webcamCapture {

    typedef std::function<void(CameraInformation& information)> notifications_callback;

    class BackendInterface {
    public:
        BackendInterface();
        virtual ~BackendInterface();
        virtual std::vector<CameraInformation> getAvaliableCameras() const = 0;
        virtual CameraInterface* getCamera(const CameraInformation &information) const = 0;
        virtual void setAvaliableCamerasChangedCallback(notifications_callback n_callback) = 0; //TODO to realize
    };

} // namespace webcamCapture

#endif
