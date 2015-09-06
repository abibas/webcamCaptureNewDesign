#ifndef MEDIA_FOUNDATION_COLOR_CONVERTER_TRANSFORM_H
#define MEDIA_FOUNDATION_COLOR_CONVERTER_TRANSFORM_H

#include "media_foundation_pixel_format_transform.h"

/**
 * A simple wrapper around Color Converter DSP
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ff819079%28v=vs.85%29.aspx
 */

namespace webcam_capture {

class MediaFoundation_ColorConverterTransform final : public MediaFoundation_PixelFormatTransform
{
public:

    static std::unique_ptr<MediaFoundation_ColorConverterTransform> getInstance(int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result);

private:
    MediaFoundation_ColorConverterTransform(std::unique_ptr<MediaFoundation_PixelFormatTransform>& pixelFormatTransform);
};

} // namespace webcam_capture

#endif // MEDIA_FOUNDATION_COLOR_CONVERTER_TRANSFORM_H
