#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <stdio.h>
#include <string>
#include <vector>
#include "Types.h"

namespace ca {
  
  class BackendInterface {
  public:
    BackendInterface();
    virtual ~BackendInterface();
    virtual std::vector<CameraInformation> getAvaliableCameras() const = 0;
    virtual Camera* getCamera(const CameraInformation &information) const = 0;
    virtual void setAvaliableCamerasChangedCallback(notifications_callback n_callback) = 0; //TODO to realize
} // namespace ca

#endif
