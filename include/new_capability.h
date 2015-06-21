#ifndef NEW_CAPABILITY
#define NEW_CAPABILITY
#include <vector>
#include <format.h>

namespace webcam_capture  {

    class MediaFoundation_Camera;   //to move to ifdef. TODO

    /**
    * Contains valid values of a FPS
    */
    class CapabilityFPS {
    public:
        ~CapabilityFPS() {}

        /**
         * @return Capability FPS
         */
        int getFps() { return fps; }
    private:

        /**
         * @param fps Capability FPS
         */
        CapabilityFPS(int fps) :
            fps(fps) {}

        /**
         * @brief fps Fps value
         */
        const int fps;

        friend class MediaFoundation_Camera;  //to move to ifdef. TODO
    };



    /**
    * Contains valid values of a width, height and fpses vector
    */
    class CapabilityResolution {
    public:
        ~CapabilityResolution() {}

        /**
         * @return CapabilityResolution width
         */
        int getWidth() { return width; }

        /**
         * @return CapabilityResolution height
         */
        int getHeight() { return height; }

        /**
         * @return CapabilityResolution FPS vector
         */
        std::vector<CapabilityFPS> getFpsVector() { return fpses; }
    private:

        /**
         * @param width CapbilityResolution width
         * @param height CapabilityResolution height
         * @param fpses CapabilityResolution fpses vector
         */
        CapabilityResolution (int width, int height, std::vector<CapabilityFPS> fpses) :
            width(width),
            height(height),
            fpses(fpses) {}

        /**
         * @param capabilityFps CapabilityFPS
         */
        void addCapabilityFPS (CapabilityFPS capabilityFps) { fpses.push_back(capabilityFps); }  // TODO change name

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
        std::vector<CapabilityFPS> fpses;

        friend class MediaFoundation_Camera;  //to move to ifdef. TODO
    };


    /**
    * Contains valid values of a PixelFormat and Resolutions vector
    */
    class CapabilityFormat{
    public:
        ~CapabilityFormat() {}
        /**
         * @return Capability pixel format
         */
        Format getPixelFormat() { return pixelFormat; }
        /**
         * @return Capability format index
         */
        int getPixelFormatIndex() { return pixelFormatIndex; }
        /**
         * @return Capability Resolutions vector
         */
        std::vector<CapabilityResolution> getResolutionsVector() { return resolutions; }
    private:

        /**
         * @param pixelFormat Capability pixel format
         * @param pixelFormatIndex The pixel format one of the Format enum.
         * @param resolutions
         */
        CapabilityFormat (Format pixelFormat, int pixelFormatIndex, std::vector<CapabilityResolution> resolutions):
            pixelFormat(pixelFormat),
            pixelFormatIndex(pixelFormatIndex),
            resolutions(resolutions) {}

        /**
         * @param resolution CapabilityResolution
         */
        void addResolution(CapabilityResolution resolution) { resolutions.push_back(resolution); }

        /**
         * pixelFormat The pixel format one of the Format enum
         */
        const Format pixelFormat;

        /**
         * @brief pixelFormatIndex Used by the implementation, represents an index to the pixel format for the implementation
         */
        const int pixelFormatIndex;

        /**
         * @brief resolutions CapabilityResolutions vector
         */
        std::vector<CapabilityResolution> resolutions;

        friend class MediaFoundation_Camera;  //to move to ifdef. TODO
    };

} // namespace webcam_capture

#endif // NEW_CAPABILITY

