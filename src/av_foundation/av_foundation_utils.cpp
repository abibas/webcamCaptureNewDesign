#include "av_foundation_utils.h"

namespace webcam_capture {

// Convert the MF format to one we can use.
PixelFormat av_foundation_video_format_to_capture_format(CMPixelFormatType avPixFormat)
{
    switch(avPixFormat) {
        case kCMPixelFormat_32ARGB :                    { return PixelFormat::ARGB32; }
        case kCMPixelFormat_32BGRA :                    { return PixelFormat::BGRA32; }
        case kCMPixelFormat_24RGB :                     { return PixelFormat::RGB24; }
        case kCMPixelFormat_16BE555 :                   { return PixelFormat::BE16_555; }
        case kCMPixelFormat_16BE565 :                   { return PixelFormat::BE16_565; }
        case kCMPixelFormat_16LE555 :                   { return PixelFormat::LE16_555; }
        case kCMPixelFormat_16LE565 :                   { return PixelFormat::LE16_565; }
        case kCMPixelFormat_16LE5551 :                  { return PixelFormat::LE16_5551; }
        case kCMPixelFormat_422YpCbCr8 :                { return PixelFormat::UYVY; }
        case kCMPixelFormat_422YpCbCr8_yuvs :           { return PixelFormat::YUYV; }
        case kCMPixelFormat_444YpCbCr8 :                { return PixelFormat::v308; }
        case kCMPixelFormat_4444YpCbCrA8 :              { return PixelFormat::v408; }
        case kCMPixelFormat_422YpCbCr16 :               { return PixelFormat::v216; }
        case kCMPixelFormat_422YpCbCr10 :               { return PixelFormat::v210; }
        case kCMPixelFormat_444YpCbCr10 :               { return PixelFormat::v410; }
        case kCMPixelFormat_8IndexedGray_WhiteIsZero :  { return PixelFormat::IndexedGray8_WhiteIsZero; }
        //video codec types
        case kCMVideoCodecType_Animation :              { return PixelFormat::rle; }
        case kCMVideoCodecType_Cinepak :                { return PixelFormat::cvid; }
        case kCMVideoCodecType_JPEG :                   { return PixelFormat::jpeg; }
        case kCMVideoCodecType_JPEG_OpenDML :           { return PixelFormat::dmb1; }
        case kCMVideoCodecType_SorensonVideo :          { return PixelFormat::SVQ1; }
        case kCMVideoCodecType_SorensonVideo3 :         { return PixelFormat::SVQ3; }
        case kCMVideoCodecType_H263 :                   { return PixelFormat::H263; }
        case kCMVideoCodecType_H264 :                   { return PixelFormat::avc1; }
        case kCMVideoCodecType_MPEG4Video :             { return PixelFormat::mp4v; }
        case kCMVideoCodecType_MPEG2Video :             { return PixelFormat::mp2v; }
        case kCMVideoCodecType_MPEG1Video :             { return PixelFormat::mp1v; }
        case kCMVideoCodecType_DVCNTSC :                { return PixelFormat::dvc; }
        case kCMVideoCodecType_DVCPAL :                 { return PixelFormat::dvcp; }
        case kCMVideoCodecType_DVCProPAL :              { return PixelFormat::dvpp; }
        case kCMVideoCodecType_DVCPro50NTSC :           { return PixelFormat::dv5n; }
        case kCMVideoCodecType_DVCPro50PAL:             { return PixelFormat::dv5p; }
        case kCMVideoCodecType_DVCPROHD720p60:          { return PixelFormat::dvhp; }
        case kCMVideoCodecType_DVCPROHD720p50:          { return PixelFormat::dvhq; }
        case kCMVideoCodecType_DVCPROHD1080i60:         { return PixelFormat::dvh6; }
        case kCMVideoCodecType_DVCPROHD1080i50:         { return PixelFormat::dvh5; }
        case kCMVideoCodecType_DVCPROHD1080p30:         { return PixelFormat::dvh3; }
        case kCMVideoCodecType_DVCPROHD1080p25:         { return PixelFormat::dvh2; }
    default: {
      return PixelFormat::UNKNOWN;
    }
  };
}

} // namespace webcam_capture
