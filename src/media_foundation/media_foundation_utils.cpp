#include "media_foundation_utils.h"
#include <format.h>

namespace webcam_capture {

// Convert the MF format to one we can use.
Format media_foundation_video_format_to_capture_format(GUID guid)
{
    ///Uncompressed RGB Formats
    if (IsEqualGUID(guid, MFVideoFormat_RGB8))     {
        return Format::RGB8;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB555)) {
        return Format::RGB555;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB565)) {
        return Format::RGB565;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB24)) {
        return Format::RGB24;
    } else if (IsEqualGUID(guid, MFVideoFormat_RGB32)) {
        return Format::RGB32;
    } else if (IsEqualGUID(guid, MFVideoFormat_ARGB32)) {
        return Format::ARGB32;
    }
    ///YUV Formats: 8-Bit and Palettized
    else if (IsEqualGUID(guid, MFVideoFormat_AI44)) {
        return Format::AI44;
    } else if (IsEqualGUID(guid, MFVideoFormat_AYUV)) {
        return Format::AYUV;
    } else if (IsEqualGUID(guid, MFVideoFormat_I420)) {
        return Format::I420;
    } else if (IsEqualGUID(guid, MFVideoFormat_IYUV)) {
        return Format::IYUV;
    } else if (IsEqualGUID(guid, MFVideoFormat_NV11)) {
        return Format::NV11;
    } else if (IsEqualGUID(guid, MFVideoFormat_NV12)) {
        return Format::NV12;
    } else if (IsEqualGUID(guid, MFVideoFormat_UYVY)) {
        return Format::UYVY;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y41P)) {
        return Format::Y41P;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y41T)) {
        return Format::Y41T;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y42T)) {
        return Format::Y42T;
    } else if (IsEqualGUID(guid, MFVideoFormat_YUY2)) {
        return Format::YUY2;
    } else if (IsEqualGUID(guid, MFVideoFormat_YV12)) {
        return Format::YV12;
    }
    ///YUV Formats: 10-Bit and 16-Bit
    else if (IsEqualGUID(guid, MFVideoFormat_P010)) {
        return Format::P010;
    } else if (IsEqualGUID(guid, MFVideoFormat_P016)) {
        return Format::P016;
    } else if (IsEqualGUID(guid, MFVideoFormat_P210)) {
        return Format::P210;
    } else if (IsEqualGUID(guid, MFVideoFormat_P216)) {
        return Format::P216;
    } else if (IsEqualGUID(guid, MFVideoFormat_v210)) {
        return Format::v210;
    } else if (IsEqualGUID(guid, MFVideoFormat_v216)) {
        return Format::v216;
    } else if (IsEqualGUID(guid, MFVideoFormat_v410)) {
        return Format::v40;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y210)) {
        return Format::Y210;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y216)) {
        return Format::Y216;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y410)) {
        return Format::Y40;
    } else if (IsEqualGUID(guid, MFVideoFormat_Y416)) {
        return Format::Y416;
    }
    ///Encoded Video Types
    else if (IsEqualGUID(guid, MFVideoFormat_DV25)) {
        return Format::DV25;
    } else if (IsEqualGUID(guid, MFVideoFormat_DV50)) {
        return Format::DV50;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVC)) {
        return Format::DVC;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVH1)) {
        return Format::DVH1;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVHD)) {
        return Format::DVHD;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVSD)) {
        return Format::DVSD;
    } else if (IsEqualGUID(guid, MFVideoFormat_DVSL)) {
        return Format::DVSL;
    }

#if (WINVER >= _WIN32_WINNT_WIN8)
    else if (IsEqualGUID(guid, MFVideoFormat_H263)) {
        return Format::H263;
    }

#endif
    else if (IsEqualGUID(guid, MFVideoFormat_H264)) {
        return Format::H264;
    } else if (IsEqualGUID(guid, MFVideoFormat_H264_ES)) {
        return Format::H264_ES;
    } else if (IsEqualGUID(guid, MFVideoFormat_HEVC)) {
        return Format::HEVC;
    } else if (IsEqualGUID(guid, MFVideoFormat_HEVC_ES)) {
        return Format::HEVC_ES;
    } else if (IsEqualGUID(guid, MFVideoFormat_M4S2)) {
        return Format::M4S2;
    } else if (IsEqualGUID(guid, MFVideoFormat_MJPG)) {
        return Format::MJPG;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP43)) {
        return Format::MP43;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP4S)) {
        return Format::MP4S;
    } else if (IsEqualGUID(guid, MFVideoFormat_MP4V)) {
        return Format::MP4V;
    } else if (IsEqualGUID(guid, MFVideoFormat_MPEG2)) {
        return Format::MPEG2;
    } else if (IsEqualGUID(guid, MFVideoFormat_MPG1)) {
        return Format::MPG1;
    } else if (IsEqualGUID(guid, MFVideoFormat_MSS1)) {
        return Format::MSS1;
    } else if (IsEqualGUID(guid, MFVideoFormat_MSS2)) {
        return Format::MSS2;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV1)) {
        return Format::WMV1;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV2)) {
        return Format::WMV2;
    } else if (IsEqualGUID(guid, MFVideoFormat_WMV3)) {
        return Format::WMV3;
    } else if (IsEqualGUID(guid, MFVideoFormat_WVC1)) {
        return Format::WVC1;
    } else if (IsEqualGUID(guid, MFVideoFormat_420O)) {
        return Format::O420;
    } else {
        return Format::UNKNOWN;
    }
}
} // namespace webcam_capture
