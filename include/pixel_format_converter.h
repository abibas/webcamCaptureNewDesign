#ifndef PIXEL_FORMAT_CONVERTER_H
#define PIXEL_FORMAT_CONVERTER_H

#include <frame.h>

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

namespace webcam_capture {

/**
 * Handles conversion of frames' pixel formats.
 */
#ifdef _WIN32
    class WEBCAM_CAPTURE_EXPORT PixelFormatConverter
#elif __APPLE__
    class PixelFormatConverter
#endif
{
public:
    /**
     * Converts a video frame to RGB pixel format.
     * @param frame Frame to convert.
     * @return RGB version of the frame.
     */
    static Frame convertToRGB(const Frame &frame);

    /**
     * Converts a video frame to YUV pixel format.
     * @param frame Frame to convert.
     * @return YUV version of the frame.
     */
    static Frame convertToYUV(const Frame &frame);

private:
    PixelFormatConverter();
};

} // namespace webcam_capture

#endif // PIXEL_FORMAT_CONVERTER_H

