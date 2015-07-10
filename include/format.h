#ifndef FORMAT_H
#define FORMAT_H

#include <webcam_capture_export.h>

/* Media Foundation
 *   - Formats: https://msdn.microsoft.com/en-us/library/windows/desktop/aa370819(v=vs.85).aspx
 *   - Description: https://msdn.microsoft.com/en-us/library/windows/desktop/dd206750(v=vs.85).aspx
 */

namespace webcam_capture {

/**
 * Supported pixel formats.
 */
enum class WEBCAM_CAPTURE_EXPORT Format {
    //Uncompressed RGB Formats
    RGB8,           /* RGB, 8 bits per pixel (bpp)*/
    RGB555,         /* RGB 555, 16 bpp*/
    RGB565,         /* RGB 565, 16 bpp*/
    RGB24,          /* RGB, 24 bpp*/
    RGB32,          /* RGB, 32 bpp*/
    ARGB32,         /* RGB, 32 bpp with alpha channel*/
    //YUV Formats: 8-Bit and Palettized
    AI44,           /* AI44, Sampling 4:4:4, Packed, Bits per channel - Palettized*/
    AYUV,           /* AYUV, Sampling 4:4:4, Packed, Bits per channel - 8*/
    I420,           /* I420, Sampling 4:2:0, Planar, Bits per channel - 8*/
    IYUV,           /* IYUV, Sampling 4:2:0, Planar, Bits per channel - 8*/
    NV11,           /* NV11, Sampling 4:1:1, Planar, Bits per channel - 8*/
    NV12,           /* NV12, Sampling 4:2:0, Planar, Bits per channel - 8*/
    UYVY,           /* UYVY, Sampling 4:2:2, Packed, Bits per channel - 8*/
    Y41P,           /* Y41P, Sampling 4:1:1, Packed, Bits per channel - 8*/
    Y41T,           /* Y41T, Sampling 4:1:1, Packed, Bits per channel - 8*/
    Y42T,           /* Y42T, Sampling 4:2:2, Packed, Bits per channel - 8*/
    YUY2,           /* YUY2, Sampling 4:2:2, Packed, Bits per channel - 8*/
    YV12,           /* YV12, Sampling 4:2:0, Planar, Bits per channel - 8*/
    //YUV Formats: 10-Bit and 16-Bit
    P010,           /* P010, Sampling 4:2:0, Planar, Bits per channel - 10*/
    P016,           /* P016, Sampling 4:2:0, Planar, Bits per channel - 16*/
    P210,           /* P210, Sampling 4:2:2, Planar, Bits per channel - 10*/
    P216,           /* P216, Sampling 4:2:2, Planar, Bits per channel - 16*/
    v210,           /* v210, Sampling 4:2:2, Packed, Bits per channel - 10*/
    v216,           /* v216, Sampling 4:2:2, Packed, Bits per channel - 16*/
    v40,            /* v410 or (v40), Sampling 4:4:4, Packed, Bits per channel - 10*/
    Y210,           /* Y210, Sampling 4:2:2, Packed, Bits per channel - 10*/
    Y216,           /* Y216, Sampling 4:2:2, Packed, Bits per channel - 16*/
    Y40,            /* Y410 or (Y40), Sampling 4:4:4, Packed, Bits per channel - 10*/
    Y416,           /* Y416, Sampling 4:4:4, Packed, Bits per channel - 16*/
    //Encoded Video Types
    DV25,           /* DVCPRO 25 (525-60 or 625-50)*/
    DV50,           /* DVCPRO 50 (525-60 or 625-50)*/
    DVC,            /* DVC/DV Video*/
    DVH1,           /* DVCPRO 100 (1080/60i, 1080/50i, or 720/60P)*/
    DVHD,           /* HD-DVCR (1125-60 or 1250-50)*/
    DVSD,           /* SDL-DVCR (525-60 or 625-50)*/
    DVSL,           /* SD-DVCR (525-60 or 625-50)*/
    H263,           /* H.263 video*/
    H264,           /* H.264 video*/
    H264_ES,        /* H.264 elementary stream*/
    HEVC,           /* The HEVC Main profile and Main Still Picture profile.
                                            Each sample contains one complete picture*/
    HEVC_ES,        /* This media type is the same as HEVC, except media samples contain a fragmented HEVC bitstream.*/
    M4S2,           /* MPEG-4 part 2 video*/
    MJPG,           /* Motion JPEG*/
    MP43,           /* Microsoft MPEG 4 codec version 3. This codec is no longer supported*/
    MP4S,           /* ISO MPEG 4 codec version 1*/
    MP4V,           /* MPEG-4 part 2 video*/
    MPEG2,          /* MPEG-2 video*/
    MPG1,           /* MPEG-1 video*/
    MSS1,           /* Windows Media Screen codec version 1*/
    MSS2,           /* Windows Media Video 9 Screen codec*/
    WMV1,           /* Windows Media Video codec version 7*/
    WMV2,           /* Windows Media Video 8 codec*/
    WMV3,           /* Windows Media Video 9 codec*/
    WVC1,           /* SMPTE 421M ("VC-1")*/
    O420,           /* "420O" 8-bit per channel planar YUV 4:2:0 video*/

    UNKNOWN         /* Unknown format*/
};

} // namespace webcam_capture

#endif // FORMAT_H

