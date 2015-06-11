/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */
#include <iostream>

#include "media_foundation_backend.h"

namespace webcam_capture {

  MediaFoundation_Backend::MediaFoundation_Backend()
  {
  }

  MediaFoundation_Backend::~MediaFoundation_Backend() {
  }

  std::vector<CameraInformation> MediaFoundation_Backend::getAvailableCameras() const{
      std::vector<CameraInformation> x;
      return x;
  }

  CameraInterface* MediaFoundation_Backend::getCamera(const CameraInformation &information) const{
      return NULL;
  }

  void MediaFoundation_Backend::setAvailableCamerasChangedCallback(notifications_callback cb_notif){

  }

} // namespace webcam_capture
