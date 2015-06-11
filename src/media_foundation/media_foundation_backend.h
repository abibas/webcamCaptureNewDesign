/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_BACKEND_H
#define MEDIA_FOUNDATION_BACKEND_H

#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfidl.h>              /* e.g. MFEnumDeviceSources */
#include <mfreadwrite.h>
#include <mferror.h>            /* MediaFoundation error codes, MF_E_* */
#include <shlwapi.h>
#include <string>
#include <vector>
#include <memory>               /* std::shared_ptr include */

#include "../utils.h"
#include "../include/backend_interface.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"

#ifdef DEBUG_VERSION
    #define DEBUG_PRINT(x) std::cerr << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

namespace webcam_capture {
    typedef std::function<void(CameraInformation& information)> notifications_callback;

    class MediaFoundation_Backend : public BackendInterface {
    public:
        MediaFoundation_Backend(); // creates std::shared_ptr<void*> mfDeinitializer
        ~MediaFoundation_Backend();
        std::vector<CameraInformation> getAvailableCameras() const;
        CameraInterface* getCamera(const CameraInformation &information) const;
        void setAvaliableCamerasChangedCallback(notifications_callback n_callback); //TODO
    private:
        notifications_callback cb_notif;
        std::shared_ptr<void*> mfDeinitializer;
    };

} // namespace webcam_capture
#endif
