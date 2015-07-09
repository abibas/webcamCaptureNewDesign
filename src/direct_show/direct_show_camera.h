/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef DIRECT_SHOW_CAMERA_H
#define DIRECT_SHOW_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00          /* Default state */
#define CA_STATE_CAPTURING 0x01     /* The user started captureing */

#include <windows.h>
#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include
#include <iostream>

#include "../utils.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "../include/capability.h"
#include "../include/pixel_buffer.h"
#include "../include/video_property.h"
#include "../include/video_property_range.h"
//#include "media_foundation_utils.h"
#include "direct_show_callback.h"

namespace webcam_capture {

typedef std::function<void(PixelBuffer &buffer)> frame_callback;

class DirectShow_Camera : public CameraInterface
{
public:
    ~DirectShow_Camera();
    static CameraInterface *createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(const CapabilityFormat &capabilityFormat, const CapabilityResolution &capabilityResolution,
              const CapabilityFps &capabilityFps, frame_callback cb);
    int stop();
    PixelBuffer *CaptureFrame();
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange *videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    DirectShow_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

public:
    std::shared_ptr<void> mfDeinitializer;
    PixelBuffer pixel_buffer;
    DirectShow_Callback *ds_callback;
    int state;
    CameraInformation information;
    frame_callback cb_frame;
};

} // namespace webcam_capture
#endif
