#include "media_foundation_camera.h"
#include <iostream>

namespace webcam_capture {

    MediaFoundation_Camera::MediaFoundation_Camera(std::shared_ptr<void*> mfDeinitializer, const CameraInformation &information){
    }

    MediaFoundation_Camera::~MediaFoundation_Camera(){

    }

    int MediaFoundation_Camera::open(const Capability &capability, frame_callback cb){
        return 0;
    }

    int MediaFoundation_Camera::close(){
        return 0;
    }

    int MediaFoundation_Camera::start(){
        return 0;
    }

    int MediaFoundation_Camera::stop(){
        return 0;
    }

    PixelBuffer* MediaFoundation_Camera::CaptureFrame(){
        return NULL;
    }

// ---- Capabilities ----
    VideoPropertyRange MediaFoundation_Camera::getPropertyRange(VideoProperty property) const{
        VideoPropertyRange vpr(0,0,0,0);
        return vpr;
    }

    int MediaFoundation_Camera::getProperty(VideoProperty property) const{
        return 0;
    }
    bool MediaFoundation_Camera::setProperty(const VideoProperty property, const int value){
        return false;
    }

    std::vector<Capability> MediaFoundation_Camera::getCapabilities() const{
        std::vector<Capability> x;
        return x;
    }

} // namespace webcam_capture
