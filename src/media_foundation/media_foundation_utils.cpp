#include "media_foundation_utils.h"

#include "../utils.h"

#include <pixel_format.h>

#include <guiddef.h>
#include <mfapi.h>

#ifdef WEBCAM_CAPTURE_DEBUG
    #include <objbase.h>
    #include <string>
#endif

namespace webcam_capture {

const std::vector<std::pair<PixelFormat, GUID>> MediaFoundation_Utils::pixelFormatMappingList =
{
    /// Uncompressed RGB Formats
    {PixelFormat::RGB8,     MFVideoFormat_RGB8},
    {PixelFormat::RGB555,   MFVideoFormat_RGB555},
    {PixelFormat::RGB565,   MFVideoFormat_RGB565},
    {PixelFormat::RGB24,    MFVideoFormat_RGB24},
    {PixelFormat::RGB32,    MFVideoFormat_RGB32},
    {PixelFormat::ARGB32,   MFVideoFormat_ARGB32},

    /// YUV Formats: 8-Bit and Palettized
    {PixelFormat::AI44,     MFVideoFormat_AI44},
    {PixelFormat::AYUV,     MFVideoFormat_AYUV},
    {PixelFormat::I420,     MFVideoFormat_I420},
    {PixelFormat::IYUV,     MFVideoFormat_IYUV},
    {PixelFormat::NV11,     MFVideoFormat_NV11},
    {PixelFormat::NV12,     MFVideoFormat_NV12},
    {PixelFormat::UYVY,     MFVideoFormat_UYVY},
    {PixelFormat::Y41P,     MFVideoFormat_Y41P},
    {PixelFormat::Y41T,     MFVideoFormat_Y41T},
    {PixelFormat::Y42T,     MFVideoFormat_Y42T},
    {PixelFormat::YUY2,     MFVideoFormat_YUY2},
    {PixelFormat::YV12,     MFVideoFormat_YV12},

    /// YUV Formats: 10-Bit and 16-Bit
    {PixelFormat::P010,     MFVideoFormat_P010},
    {PixelFormat::P016,     MFVideoFormat_P016},
    {PixelFormat::P210,     MFVideoFormat_P210},
    {PixelFormat::P216,     MFVideoFormat_P216},
    {PixelFormat::v210,     MFVideoFormat_v210},
    {PixelFormat::v216,     MFVideoFormat_v216},
    {PixelFormat::v410,     MFVideoFormat_v410},
    {PixelFormat::Y210,     MFVideoFormat_Y210},
    {PixelFormat::Y216,     MFVideoFormat_Y216},
    {PixelFormat::Y410,     MFVideoFormat_Y410},
    {PixelFormat::Y416,     MFVideoFormat_Y416},

    /// Encoded Video Types
    {PixelFormat::DV25,     MFVideoFormat_DV25},
    {PixelFormat::DV50,     MFVideoFormat_DV50},
    {PixelFormat::DVC,      MFVideoFormat_DVC},
    {PixelFormat::DVH1,     MFVideoFormat_DVH1},
    {PixelFormat::DVHD,     MFVideoFormat_DVHD},
    {PixelFormat::DVSD,     MFVideoFormat_DVSD},
    {PixelFormat::DVSL,     MFVideoFormat_DVSL},
#if (WINVER >= _WIN32_WINNT_WIN8)
    {PixelFormat::H263,     MFVideoFormat_H263},
#endif
    {PixelFormat::H264,     MFVideoFormat_H264},
    {PixelFormat::H264_ES,  MFVideoFormat_H264_ES},
    {PixelFormat::M4S2,     MFVideoFormat_M4S2},
    {PixelFormat::MJPG,     MFVideoFormat_MJPG},
    {PixelFormat::MP43,     MFVideoFormat_MP43},
    {PixelFormat::MP4S,     MFVideoFormat_MP4S},
    {PixelFormat::MP4V,     MFVideoFormat_MP4V},
    {PixelFormat::MPEG2,    MFVideoFormat_MPEG2},
    {PixelFormat::MPG1,     MFVideoFormat_MPG1},
    {PixelFormat::MSS1,     MFVideoFormat_MSS1},
    {PixelFormat::MSS2,     MFVideoFormat_MSS2},
    {PixelFormat::WMV1,     MFVideoFormat_WMV1},
    {PixelFormat::WMV2,     MFVideoFormat_WMV2},
    {PixelFormat::WMV3,     MFVideoFormat_WMV3},
    {PixelFormat::WVC1,     MFVideoFormat_WVC1},
    {PixelFormat::O420,     MFVideoFormat_420O}
};

// Convert the MF format to one we can use.
PixelFormat MediaFoundation_Utils::videoFormatToPixelFormat(const GUID &guid)
{
    for (auto &&pair : pixelFormatMappingList) {
        if (IsEqualGUID(guid, pair.second)) {
            return pair.first;
        }
    }

#ifdef WEBCAM_CAPTURE_DEBUG
    LPOLESTR guidString = NULL;
    HRESULT hr = StringFromCLSID(guid, &guidString);

    if (SUCCEEDED(hr)) {
        DEBUG_PRINT("Unknown pixel format, GUID: " << MediaFoundation_Utils::string_cast<std::string>(guidString));
    } else {
        DEBUG_PRINT("Unknown pixel format.");
    }

    CoTaskMemFree(guidString);
#endif

    return PixelFormat::UNKNOWN;
}

bool MediaFoundation_Utils::pixelFormatToVideoFormat(PixelFormat pixelFormat, GUID &guid)
{
    for (auto &&pair : pixelFormatMappingList) {
        if (pixelFormat == pair.first) {
            guid = pair.second;
            return true;
        }
    }

    return false;
}

} // namespace webcam_capture
