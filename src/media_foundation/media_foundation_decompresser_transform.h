#ifndef MEDIA_FOUNDATION_DECOMPRESSER_TRANSFORM_H
#define MEDIA_FOUNDATION_DECOMPRESSER_TRANSFORM_H

#include "media_foundation_pixel_format_transform.h"

class IMFActivate;

namespace webcam_capture {

class MediaFoundation_DecompresserTransform final : public MediaFoundation_PixelFormatTransform
{
public:
    static std::unique_ptr<MediaFoundation_DecompresserTransform> getInstance(int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result);

    ~MediaFoundation_DecompresserTransform();

private:
    MediaFoundation_DecompresserTransform(std::unique_ptr<MediaFoundation_PixelFormatTransform>& pixelFormatTransform, IMFActivate *activate);

    IMFActivate *activate;
};

} // namespace webcam_capture

#endif // MEDIA_FOUNDATION_DECOMPRESSER_TRANSFORM_H
