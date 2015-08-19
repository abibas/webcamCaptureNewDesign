#ifndef AV_FOUNDATION_CAMERA_H
#define AV_FOUNDATION_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00                                                         /* Default state */
#define CA_STATE_CAPTURING 0x01                                                   /* The user started captureing */

#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include

#include "../utils.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "../include/capability.h"
#include "../include/frame.h"
#include "../include/video_property.h"
#include "../include/video_property_range.h"

namespace webcam_capture {

class AVFoundation_Camera : public CameraInterface
{
public:
    ~AVFoundation_Camera();
    static std::unique_ptr<CameraInterface> createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb);
    int stop();
    std::unique_ptr<Frame> captureFrame();
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    AVFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

public:
    std::shared_ptr<void> mfDeinitializer;
    Frame frame;
//    AVFoundation_Callback *mf_callback;
    int state;
    CameraInformation information;
    FrameCallback cb_frame;
private:
    void* avFoundationInterface; // the objective-c interface
};

} // namespace webcam_capture
#endif
