#ifndef AV_FOUNDATION_UTILS_H
#define AV_FOUNDATION_UTILS_H

#include <camera_information.h>
#include <CoreMedia/CMFormatDescription.h>
#include <pixel_format.h>
#include <string>
#include <functional>
#include <memory>

namespace webcam_capture {

typedef std::function<void(std::string name, std::string id)> AVFoundationConnectionStatusCallback;

class AVFoundationCapability {
public:
    AVFoundationCapability(PixelFormat pixFormat, int width, int height, float maxFps, float minFps)
                : pixFormat(pixFormat)
                , width(width)
                , height(height)
                , minFps(maxFps)
                , maxFps(minFps) {}

    ~AVFoundationCapability() {}

    PixelFormat pixFormat;
    int width;
    int height;
    float minFps;
    float maxFps;
};

class AVFoundationDeviceInfo {
public:
    AVFoundationDeviceInfo(std::string deviceName, std::string deviceId)
        : deviceName(deviceName)
        , deviceId(deviceId) {}
    ~AVFoundationDeviceInfo() {}

    std::string deviceName;
    std::string deviceId;
};

PixelFormat av_foundation_video_format_to_capture_format(CMPixelFormatType avPixFormat);
} // namespace webcam_capture

#endif
