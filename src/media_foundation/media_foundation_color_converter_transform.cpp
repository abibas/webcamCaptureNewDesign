#include "media_foundation_color_converter_transform.h"

#include "media_foundation_utils.h"

#include <mftransform.h>
#include <mfapi.h>
#include <wmcodecdsp.h>

namespace webcam_capture {

std::unique_ptr<MediaFoundation_ColorConverterTransform> MediaFoundation_ColorConverterTransform::getInstance(int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result)
{
    CComPtr<IMFTransform> transform;

    HRESULT hr = CoCreateInstance(CLSID_CColorConvertDMO, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&transform));
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to create transform.", hr);
        result = RESULT::FAILURE;
        return nullptr;
    }

    std::unique_ptr<MediaFoundation_PixelFormatTransform> pixelFormatTransform = MediaFoundation_PixelFormatTransform::getInstance(transform, width, height, inputPixelFormat, outputPixelFormat, result);

    if (result != RESULT::OK) {
        return nullptr;
    }

    result = RESULT::OK;

    return std::unique_ptr<MediaFoundation_ColorConverterTransform>(new MediaFoundation_ColorConverterTransform(pixelFormatTransform));
}


MediaFoundation_ColorConverterTransform::MediaFoundation_ColorConverterTransform(std::unique_ptr<MediaFoundation_PixelFormatTransform>& pixelFormatTransform) :
    MediaFoundation_PixelFormatTransform(std::move(*pixelFormatTransform))
{

}

} // namespace webcam_capture
