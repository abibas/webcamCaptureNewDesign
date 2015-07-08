/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAPABILITY_H
#define CAPABILITY_H

#include <vector>

#include <format.h>
#include <webcam_capture_export.h>

namespace webcam_capture  {

    /**
    * Contains valid values of a FPS
    */
    class WEBCAM_CAPTURE_EXPORT CapabilityFps {
    public:
        /**
         * @param fps Capability FPS
         */
        CapabilityFps(int fps) :
            fps(fps) {}

        ~CapabilityFps() {}

        /**
         * @return Capability FPS
         */
        int getFps() const { return fps; }
    private:
        /**
         * @brief fps Fps value
         */
        const int fps;
    };



    /**
    * Contains valid values of a width, height and fpses vector
    */
    class WEBCAM_CAPTURE_EXPORT CapabilityResolution {
    public:
        /**
         * @param width CapbilityResolution width
         * @param height CapabilityResolution height
         * @param fpses CapabilityResolution fpses vector
         */
        CapabilityResolution (int width, int height, std::vector<CapabilityFps> fpses) :
            width(width),
            height(height),
            fpses(fpses) {}

        ~CapabilityResolution() {}

        /**
         * @return CapabilityResolution width
         */
        int getWidth() const { return width; }

        /**
         * @return CapabilityResolution height
         */
        int getHeight() const { return height; }

        /**
         * @return CapabilityResolution FPS vector
         */
        const  std::vector<CapabilityFps>& getFpses() const { return fpses; }
    private:
        /**
         * @brief width Resolution width
         */
        const int width;
        /**
         * @brief height Resolution height
         */
        const int height;

        /**
         * @brief Resolution fps vector
         */
        const std::vector<CapabilityFps> fpses;
    };


    /**
    * Contains valid values of a PixelFormat and Resolutions vector
    */
    class WEBCAM_CAPTURE_EXPORT CapabilityFormat{
    public:
        /**
         * @param pixelFormat Capability pixel format
         * @param pixelFormatIndex The pixel format one of the Format enum.
         * @param resolutions
         */
        CapabilityFormat (Format pixelFormat, std::vector<CapabilityResolution> resolutions):
            pixelFormat(pixelFormat),
            resolutions(resolutions) {}

        ~CapabilityFormat() {}

        /**
         * @return Capability pixel format
         */
        Format getPixelFormat() const { return pixelFormat; }

        /**
         * @return Capability Resolutions vector
         */
        const std::vector<CapabilityResolution>& getResolutions() const { return resolutions; }

    private:
        /**
         * pixelFormat The pixel format one of the Format enum
         */
        const Format pixelFormat;

        /**
         * @brief resolutions CapabilityResolutions vector
         */
        const std::vector<CapabilityResolution> resolutions;
    };
} // namespace webcam_capture

#endif // CAPABILITY_H

