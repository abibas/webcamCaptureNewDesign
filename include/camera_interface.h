#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H

#include <functional>
#include <vector>

#include "capability.h"
#include "pixel_buffer.h"
#include "video_property_range.h"
#include "video_property.h"

namespace webcamCapture {

    typedef std::function<void(PixelBuffer& buffer)> frame_callback;

    class CameraInterface {
    public:
        CameraInterface();
        virtual ~CameraInterface();
        virtual int open(const Capability &capability, frame_callback cb) = 0;
        virtual int close() = 0;
        virtual int start() = 0;
        virtual int stop() = 0;
        virtual PixelBuffer* CaptureFrame() = 0;  //TODO
        // ----Capabilities----
        virtual VideoPropertyRange getPropertyRange(VideoProperty property) const = 0; // TODO
        virtual int getProperty(VideoProperty property) const = 0; //TODO
        virtual bool setProperty(const VideoProperty property,const int value) = 0; //TODO
        virtual std::vector<Capability> getCapabilities() const = 0;
    };

} // namespace webcamCapture

#endif
