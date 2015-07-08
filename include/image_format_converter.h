#ifndef IMAGE_FORMAT_CONVERTER
#define IMAGE_FORMAT_CONVERTER

#include <pixel_buffer.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * @brief Class of frames converting
 */
class WEBCAM_CAPTURE_EXPORT ImageFormatConverter
{
public:
    /**
     * @param ConvertToRGB input frame
     * @return Converted frame
     */
    static PixelBuffer convertToRGB(const PixelBuffer &);

    /**
     * @param ConvertToRGB input frame
     * @return Converted frame
     */
    static PixelBuffer convertToYUV(const PixelBuffer &);
private:
    /**
     * Static class (Java-ism)
     */
    ImageFormatConverter();
};

} // namespace webcam_capture

#endif // IMAGE_FORMAT_CONVERTER

