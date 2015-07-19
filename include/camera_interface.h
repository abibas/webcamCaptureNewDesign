/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAMERA_INTERFACE_H
#define CAMERA_INTERFACE_H

#include <functional>
#include <memory>
#include <vector>

#include <camera_information.h>
#include <capability.h>
#include <pixel_buffer.h>
#include <video_property.h>
#include <video_property_range.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

typedef std::function<void(PixelBuffer &buffer)> frame_callback;

/**
 * Common interface of backend implementations.
 * Provides access to video capturing and camera information.
 */
class WEBCAM_CAPTURE_EXPORT CameraInterface
{
public:
    CameraInterface() {}
    virtual ~CameraInterface() {}

    /**
     * Starts video capture in specified format, resolution and frame rate.
     * @param pixelFormat Pixel format in which you want to capture the video frames.
     * @param width Width part of resolution of the captured video frames.
     * @param height Height part of resolution of the captured video frames.
     * @param fps Frame rate of capturing.
     * @param cb Callback with the captured video frame data.
     * @return TODO(nurupo): add enum for: already in use, already started, invalid combination of capabilities, unknown error.
     */
    virtual int start(Format pixelFormat, int width, int height, float fps, frame_callback cb) = 0;

    /**
     * Stops video capture.
     * @return TODO(nurupo): add enum for: not started. research into more reasons of failure.
     */
    virtual int stop() = 0;         //TODO to add enum with error codes

    /**
     * Takes a still photo.
     * @return Photo image data.
     */
    virtual std::unique_ptr<PixelBuffer> CaptureFrame() = 0;  //TODO

    /**
     * Gets information about a video property range.
     * @param property Property you want to look up the range of.
     * @param range Property range that will be set on success.
     * @return true on success, false on failure.
     */
    virtual bool getPropertyRange(VideoProperty property, VideoPropertyRange &range) = 0;  // TODO

    /**
     * Gets value of a property.
     * @param property Property to look up the value of.
     * @return Value of the propety.
     */
    virtual int getProperty(VideoProperty property) = 0; //TODO       //TODO to add enum with error codes

    /**
     * Sets value of a property.
     * @param property Property to set the value of.
     * @param value Value to set.
     * @return true on success, false on failure.
     */
    virtual bool setProperty(const VideoProperty property, const int value) = 0; //TODO

    /**
     * Gets information about camera's capabilities, i.e. supported pixel formats, resolutions and FPS.
     * @return Capabilities the camera supports.
     */
    virtual std::vector<CapabilityFormat> getCapabilities() = 0;
};

} // namespace webcam_capture

#endif // CAMERA_INTERFACE_H
