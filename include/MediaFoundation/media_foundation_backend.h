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

#include "../utils.h"
#include "../backend_interface.h"
#include "../camera_interface.h"
#include "../camera_information.h"

#include "media_foundation_utils.h"
#include "media foundation_callback.h"

namespace webcamCapture {
    typedef std::function<void(CameraInformation& information)> notifications_callback;

    class MediaFoundation_Backend : public BackendInterface {
    public:
        MediaFoundation_Backend(); // creates std::shared_ptr<void*> mfDeinitializer
        ~MediaFoundation_Backend();
        std::vector<CameraInformation> getAvailableCameras() const;
        CameraInterface* getCamera(const CameraInformation &information) const;
        void setAvailableCamerasChangedCallback(notifications_callback cb_notif); //TODO
    private:
        notifications_callback cb_notif;
        std::shared_ptr<void*> mfDeinitializer;
    };

} // namespace webcamCapture
#endif
