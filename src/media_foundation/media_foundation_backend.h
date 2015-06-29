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
#include <functional>

#include "../utils.h"
#include <backend_interface.h>
#include <camera_interface.h>
#include <camera_information.h>
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"
#include "media_foundation_camera_notifications.h"

namespace webcam_capture {

    class MediaFoundation_Backend : public BackendInterface {
    public:
        MediaFoundation_Backend(); // creates std::shared_ptr<void*> mfDeinitializer
        ~MediaFoundation_Backend();
        static void DeinitBackend(void*);
        std::vector<CameraInformation> getAvailableCameras() const;
        CameraInterface* getCamera(CameraInformation &information) const;
        int setAvaliableCamerasChangedCallback(notifications_callback n_callback); //TODO
    private:
        std::shared_ptr<void> mfDeinitializer;
        MediaFoundation_CameraNotifications * notificationManager;
    };
} // namespace webcam_capture
#endif
