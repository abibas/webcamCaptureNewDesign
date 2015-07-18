#include "direct_show_utils.h"

#include <format.h>

#include <dshow.h>
#include <wmcodecdsp.h>

namespace webcam_capture {

// available formats - https://msdn.microsoft.com/en-us/library/windows/desktop/dd407353(v=vs.85).aspx
// Convert the DirectShow format to one we can use.
Format direct_show_video_format_to_capture_format(GUID guid)
{
    /// Analog Video Subtypes
    if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_NTSC_M)) {
        return Format::AnalogVideo_NTSC_M;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_B)) {
        return Format::AnalogVideo_PAL_B;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_D)) {
        return Format::AnalogVideo_PAL_D;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_G)) {
        return Format::AnalogVideo_PAL_G;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_H)) {
        return Format::AnalogVideo_PAL_H;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_I)) {
        return Format::AnalogVideo_PAL_I;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_M)) {
        return Format::AnalogVideo_PAL_M;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_PAL_N)) {
        return Format::AnalogVideo_PAL_N;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_B)) {
        return Format::AnalogVideo_SECAM_B;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_D)) {
        return Format::AnalogVideo_SECAM_D;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_G)) {
        return Format::AnalogVideo_SECAM_G;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_H)) {
        return Format::AnalogVideo_SECAM_H;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_K)) {
        return Format::AnalogVideo_SECAM_K;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_K1)) {
        return Format::AnalogVideo_SECAM_K1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AnalogVideo_SECAM_L)) {
        return Format::AnalogVideo_SECAM_L;
    }

    /// DirectX Video Acceleration Video Subtypes
    else if (IsEqualGUID(guid, MEDIASUBTYPE_AI44)) {
        return Format::AI44;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IA44)) {
        return Format::IA44;
    }

    /// DV Video Subtypes
    //  Consumer Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_dvsl)) {
        return Format::DVSL;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvsd)) {
        return Format::DVSD;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvhd)) {
        return Format::DVHD;
    }
    //  Professional Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_dv25)) {
        return Format::DV25;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dv50)) {
        return Format::DV50;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_dvh1)) {
        return Format::DVH1;
    }

    ///H.264 Video Types
    else if (IsEqualGUID(guid, MEDIASUBTYPE_H264)) {
        return Format::H264;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_AVC1)) {
        return Format::AVC1;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_h264)) {
        return Format::H264;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_X264)) {
        return Format::H264;
    //Equivalent to MEDIASUBTYPE_H264, with a different FOURCC.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_x264)) {
        return Format::H264;
    }

    ///Uncompressed RGB Video Subtypes
    // Uncompressed RGB formats with no alpha channel.
    else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB1)) {
        return Format::RGB1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB4)) {
        return Format::RGB4;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB8)) {
        return Format::RGB8;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB555)) {
        return Format::RGB555;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB565)) {
        return Format::RGB565;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB24)) {
        return Format::RGB24;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32)) {
        return Format::RGB32;
    }
    //  Uncompressed RGB formats with alpha channel.
    else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555)) {
        return Format::ARGB1555;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32)) {
        return Format::ARGB32;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444)) {
        return Format::ARGB4444;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_A2R10G10B10)) {
        return Format::A2R10G10B10;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_A2B10G10R10)) {
        return Format::A2B10G10R10;
    }



    ///Video Mixing Renderer Video Subtypes
    // VMR-7 Subtypes
    else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32_D3D_DX7_RT)) {
        return Format::RGB32_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB16_D3D_DX7_RT)) {
        return Format::RGB16_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32_D3D_DX7_RT)) {
        return Format::ARGB32_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444_D3D_DX7_RT)) {
        return Format::ARGB4444_D3D_DX7_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555_D3D_DX7_RT)) {
        return Format::ARGB1555_D3D_DX7_RT;
    // VMR-9 Subtypes
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB32_D3D_DX9_RT)) {
        return Format::RGB32_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_RGB16_D3D_DX9_RT)) {
        return Format::RGB16_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB32_D3D_DX9_RT)) {
        return Format::ARGB32_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB4444_D3D_DX9_RT)) {
        return Format::ARGB4444_D3D_DX9_RT;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_ARGB1555_D3D_DX9_RT)) {
        return Format::ARGB1555_D3D_DX9_RT;
    }


    /// YUV Video Subtypes
    // Recommended YUV Formats
    else if (IsEqualGUID(guid, MEDIASUBTYPE_AYUV))     {
        return Format::AYUV;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YUY2)) {
        return Format::YUY2;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_UYVY)) {
        return Format::UYVY;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC1)) {
        return Format::IMC1;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC2)) {
        return Format::IMC2;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC3)) {
        return Format::IMC3;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IMC4)) {
        return Format::IMC4;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YV12)) {
        return Format::YV12;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_NV12)) {
        return Format::NV12;

    // Other YUV Format Types
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_I420)) {
        return Format::I420;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IF09)) {
        return Format::IF09;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IYUV)) {
        return Format::IYUV;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y211)) {
        return Format::Y211;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y411)) {
        return Format::Y411;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Y41P)) {
        return Format::Y41P;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YVU9)) {
        return Format::YVU9;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_YVYU)) {
        return Format::YVYU;

    /// Miscellaneous Video Subtypes
    // The following table lists some additional video subtypes.
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CFCC)) {
        return Format::CFCC;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CLJR)) {
        return Format::CLJR;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CPLA)) {
        return Format::CPLA;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_CLPL)) {
        return Format::CLPL;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_IJPG)) {
        return Format::IJPG;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_MDVF)) {
        return Format::MDVF;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_MJPG)) {
        return Format::MJPG;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Overlay)) {
        return Format::Overlay;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_Plum)) {
        return Format::Plum;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTJpeg)) {
        return Format::QTJpeg;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTMovie)) {
        return Format::QTMovie;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTRle)) {
        return Format::QTRle;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTRpza)) {
        return Format::QTRpza;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_QTSmc)) {
        return Format::QTSmc;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_TVMJ)) {
        return Format::TVMJ;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_VPVBI)) {
        return Format::VPVBI;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_VPVideo)) {
        return Format::VPVideo;
    } else if (IsEqualGUID(guid, MEDIASUBTYPE_WAKE)) {
        return Format::WAKE;
    }

    else {
        return Format::UNKNOWN;
    }
}
} // namespace webcam_capture
