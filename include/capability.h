#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <vector>

#include <format.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Provides FPS supported by the camera for some resolution and pixel format.
 */
class WEBCAM_CAPTURE_EXPORT CapabilityFps
{
public:
    CapabilityFps(int fps) :
        fps(fps) {}

    ~CapabilityFps() {}

    /**
     * @return FPS supported by the camera for some resolution and pixel format.
     */
    int getFps() const
    {
        return fps;
    }

private:
    const int fps;
};


/**
 * Provides resolution (width and height) supported by the camera for some pixel format,
 * along with a list of supported FPS values for that resolution.
 */
class WEBCAM_CAPTURE_EXPORT CapabilityResolution
{
public:
    CapabilityResolution(int width, int height, std::vector<CapabilityFps> fpses) :
        width(width),
        height(height),
        fpses(fpses) {}

    ~CapabilityResolution() {}

    /**
     * @return Width part of the resolution.
     */
    int getWidth() const
    {
        return width;
    }

    /**
     * @return Height part of the resolution.
     */
    int getHeight() const
    {
        return height;
    }

    /**
     * @return List of supported FPS values for this resolution.
     */
    const  std::vector<CapabilityFps> &getFpses() const
    {
        return fpses;
    }

private:
    const int width;
    const int height;
    const std::vector<CapabilityFps> fpses;
};


/**
 * Provides pixel format supported by the camera,
 * along with a list of supported resolutions for that pixel format.
 */
class WEBCAM_CAPTURE_EXPORT CapabilityFormat
{
public:
    CapabilityFormat(Format pixelFormat, std::vector<CapabilityResolution> resolutions):
        pixelFormat(pixelFormat),
        resolutions(resolutions) {}

    ~CapabilityFormat() {}

    /**
     * @return Pixel format supported by the camera.
     */
    Format getPixelFormat() const
    {
        return pixelFormat;
    }

    /**
     * @return List of supported resolutions for that pixel format.
     */
    const std::vector<CapabilityResolution> &getResolutions() const
    {
        return resolutions;
    }

private:
    const Format pixelFormat;
    const std::vector<CapabilityResolution> resolutions;
};

} // namespace webcam_capture

#endif // CAPABILITY_H

