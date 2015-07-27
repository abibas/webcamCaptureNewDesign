#include "../capability_tree_builder.h"
#include "av_foundation_camera.h"
#include "av_foundation_interface.h"
#include "av_foundation_unique_id.h"
#include "../capability_tree_builder.h"

namespace webcam_capture {

AVFoundation_Camera::AVFoundation_Camera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
    : information(information)
    , mfDeinitializer(mfDeinitializer)
    , state(CA_STATE_NONE)
    , avFoundationInterface(NULL)
{
    AVFoundation_UniqueId * avUniqId = (AVFoundation_UniqueId*) information.getUniqueId();
    avFoundationInterface = webcam_capture_av_alloc(avUniqId->getId());
    if(!avFoundationInterface) {
        DEBUG_PRINT("Error: cannot create AVFoundation_Interface objective-c object.\n");
    }
}

std::unique_ptr<CameraInterface> AVFoundation_Camera::createCamera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
{
    return std::unique_ptr<AVFoundation_Camera>(new AVFoundation_Camera(mfDeinitializer, information));
}

AVFoundation_Camera::~AVFoundation_Camera()
{
    // Stop capturing
    if (state & CA_STATE_CAPTURING) {
        stop();
    }

    if(avFoundationInterface) {
      webcam_capture_av_dealloc(avFoundationInterface);
      avFoundationInterface = NULL;
    }
}

int AVFoundation_Camera::start(PixelFormat pixelFormat,
                                  int width,
                                  int height, float fps,
                                  FrameCallback cb)
{
    if (!cb) {
        DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.\n");
        return -1;      //TODO Err code
    }

    if (state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: cannot start capture because we are already capturing.\n");
        return -2;      //TODO Err code
    }

    cb_frame = cb;


    return 1;      //TODO Err code
}

int AVFoundation_Camera::stop()
{
    if (!state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: Cannot stop capture because we're not capturing yet.\n");
        return -1;    //TODO Err code
    }

    state &= ~CA_STATE_CAPTURING;

    return 1;   //TODO Err code
}

std::unique_ptr<Frame> AVFoundation_Camera::CaptureFrame()
{
    //TODO to realise method
    return nullptr;
}

// ---- Capabilities ----
std::vector<CapabilityFormat> AVFoundation_Camera::getCapabilities()
{
    std::vector<AVCapabilityInfo> avCapInfos;
    webcam_capture_av_get_capabilities(avFoundationInterface, avCapInfos);
    CapabilityTreeBuilder capabilityBuilder;
    for (int i = 0; i < avCapInfos.size(); i++) {
        capabilityBuilder.addCapability(avCapInfos.at(i).pixFormat,
                                    avCapInfos.at(i).width,
                                    avCapInfos.at(i).height,
                                    {avCapInfos.at(i).minFps, avCapInfos.at(i).maxFps, avCapInfos.at(i).maxFps});
    }
    return capabilityBuilder.build();
}



bool AVFoundation_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange)
{
    return true;
}



int AVFoundation_Camera::getProperty(VideoProperty property)
{
    int value;
    return value;
}



bool AVFoundation_Camera::setProperty(const VideoProperty property, const int value)
{
    return true;
}


} // namespace webcam_capture
