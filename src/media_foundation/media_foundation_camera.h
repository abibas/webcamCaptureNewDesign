/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CAMERA_H
#define MEDIA_FOUNDATION_CAMERA_H

#include <camera_information.h>
#include <camera_interface.h>
#include <capability.h>
#include <frame.h>
#include <video_property.h>
#include <video_property_range.h>

#include <memory>
#include <string>
#include <vector>

struct IMFMediaSource;
struct IMFSourceReader;
struct IMFSourceReaderCallback;

namespace webcam_capture {

class MediaFoundation_Callback;

class MediaFoundation_Camera : public CameraInterface
{
public:
    ~MediaFoundation_Camera();
    static std::unique_ptr<CameraInterface> create(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb);
    int stop();
    std::unique_ptr<Frame> captureFrame();  //TODO
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information,
                           IMFMediaSource *mediaSource);

    //// SDK functions
    int createSourceReader(IMFMediaSource *mediaSource, IMFSourceReaderCallback *callback,
                           IMFSourceReader **sourceReader) const;
    static int createVideoDeviceSource(const int device, IMFMediaSource **source); //TODO outdated method - to remove
    static int createVideoDeviceSource(const std::wstring &pszSymbolicLink, IMFMediaSource **ppSource);
    int getVideoCapabilities(IMFMediaSource *source, std::vector<CapabilityFormat> &capFormatVector) const;
    int setDeviceFormat(IMFMediaSource *source, const int width, const int height, const PixelFormat pixelFormat,
                        const float fps) const;
    int setReaderFormat(IMFSourceReader *reader, const int width, const int height, const PixelFormat pixelFormat, const float fps) const;

public:
    std::shared_ptr<void> mfDeinitializer;
    Frame frame;
    MediaFoundation_Callback *mfCallback;
    IMFMediaSource *imfMediaSource;
    IMFSourceReader *imfSourceReader;
    bool capturing;
    CameraInformation information;
    FrameCallback frameCallback;
};

} // namespace webcam_capture

#endif //MEDIA_FOUNDATION_CAMERA_H
