#ifndef MEDIA_FOUNDATION_BACKEND_H
#define MEDIA_FOUNDATION_BACKEND_H

#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfidl.h>                                                                   /* e.g. MFEnumDeviceSources */
#include <mfreadwrite.h>
#include <mferror.h>                                                                 /* MediaFoundation error codes, MF_E_*  */
#include <shlwapi.h>
#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include

#include "../Types.h"
#include "../Utils.h"
#include "../BackendInterface.h"
#include "MediaFoundation_Utils.h"
#include "MediaFoundation_Callback.h"

namespace ca {

  class MediaFoundation_Backend : public BackendInterface {
  public:
    MediaFoundation_Backend(); // creates std::shared_ptr<void*> mfDeinitializer
    ~MediaFoundation_Backend();
    std::vector<CameraInformation> getAvailableCameras() const;
    Camera* getCamera(const CameraInformation &information) const;
    void setAvailableCamerasChangedCallback(notifications_callback cb_notif); //TODO
  private:
    notifications_callback cb_notif;
    std::shared_ptr<void*> mfDeinitializer;
  };

} // namespace cs
#endif
