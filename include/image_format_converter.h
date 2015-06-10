#ifndef IMAGE_FORMAT_CONVERTER
#define IMAGE_FORMAT_CONVERTER
#include "pixel_buffer.h"

namespace webcam_capture {

    /**
     * @brief Class of frames converting
     */
    class ImageFormatConverter{
    public:
        /**
         * @param convertToRGB recieves frame
         * @return converted frame
         */
        static PixelBuffer convertToRGB(const PixelBuffer&);

        /**
         * @param convertToRGB recieves frame
         * @return converted frame
         */
        static PixelBuffer convertToYUV(const PixelBuffer&);
    private:
        /**
         * static class (Java-ism)
         */
        ImageFormatConverter();
    };

} // namespace webcam_capture

#endif // IMAGE_FORMAT_CONVERTER

