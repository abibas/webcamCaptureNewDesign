#ifndef IMAGE_FORMAT_CONVERTER_H
#define IMAGE_FORMAT_CONVERTER_H

#include <frame.h>

#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Handles conversion of pixel formats.
 */
class WEBCAM_CAPTURE_EXPORT ImageFormatConverter
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
    ImageFormatConverter();
};

} // namespace webcam_capture

#endif // IMAGE_FORMAT_CONVERTER_H

