/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H

#include <functional>
#include <vector>
#include <memory>

#include <capability.h>
#include <pixel_buffer.h>
#include <camera_information.h>
#include <video_property_range.h>
#include <video_property.h>
#include <new_capability.h>

namespace webcam_capture {
    /**
     * frame_callback Typedef for frames callback
     */
    typedef std::function<void(PixelBuffer& buffer)> frame_callback;

    /**
     * Contains Interface for Camera realization
     */
    class CameraInterface {
    public:
        CameraInterface() {}
        virtual ~CameraInterface() {}

        /**
         * open Open the camera device
         * @param capability Capability of frames capturing
         * @param cb Frame callback
         * @return Stutas code //TODO do we need the status code ?? If Yes - we have to add error's enum
         */
        virtual int open() = 0; //TODO to add enum with error codes
        /**
         * close Close camera device
         * @return Status code
         */
        virtual int close() = 0;        //TODO to add enum with error codes
        /**
         * start Start capturing. Captured frames thows in frame callback
         * @return Status code
         */
        virtual int start(const CapabilityFormat &capabilityFormat, const CapabilityResolution &capabilityResolution,
                          const CapabilityFps &capabilityFps, frame_callback cb) = 0; //TODO to add enum with error codes
        /**
         * stop Stop capturing
         * @return Status code
         */
        virtual int stop() = 0;         //TODO to add enum with error codes
        /**
         * CaptureFrame Capture one single frame
         * @return Captured frame
         */
        virtual PixelBuffer* CaptureFrame() = 0;  //TODO

        // ----Capabilities----
        /**
         * @param property Property is one of the VideoPropery enum
         * @return Video property range
         */
        virtual VideoPropertyRange getPropertyRange(VideoProperty property) = 0; // TODO

        /**
         * @param property Property is one of the VideoPropery enum
         * @return Property current value
         */
        virtual int getProperty(VideoProperty property) = 0; //TODO       //TODO to add enum with error codes

        /**
         * @param property Property is one of the VideoPropery enum
         * @param value Property value
         * @return Success status
         */
        virtual bool setProperty(const VideoProperty property,const int value) = 0; //TODO

        /**
         * @return Capabilities vector
         */
        virtual std::vector<CapabilityFormat> getCapabilities() = 0;
    };

} // namespace webcam_capture

#endif
