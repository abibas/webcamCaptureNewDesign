/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef FORMAT_H
#define FORMAT_H

namespace webcam_capture {
    /**
     * Enum of supported video capture formats
     */
    enum class Format { UYVY422,        /* Cb Y0 Cr Y1 */
                        YUYV422,        /* Y0 Cb Y1 Cr */
                        YUV422P,        /* YUV422 Planar */
                        YUV420P,        /* YUV420 Planar */
                        YUV420BP,       /* YUV420 Bi Planar */
                        YUVJ420P,       /* YUV420 Planar Full Range (JPEG), J comes from the JPEG. (values 0-255 used) */
                        YUVJ420BP,      /* YUV420 Bi-Planer Full Range (JPEG), J comes fro the JPEG. (values: luma = [16,235], chroma=[16,240]) */
                        ARGB32,         /* ARGB 8:8:8:8 32bpp, ARGBARGBARGB... */
                        BGRA32,         /* BGRA 8:8:8:8 32bpp, BGRABGRABGRA... */
                        RGBA32,         /* RGBA 8:8:8:8 32bpp. */
                        RGB24,          /* RGB 8:8:8 24bit */
                        JPEG_OPENDML,   /* JPEG with Open-DML extensions */
                        H264,           /* H264 */
                        MJPEG,          /* MJPEG 2*/
                        UNKNOWN         /* Unknown format*/
                      };
} // namespace webcam_capture

#endif // FORMAT_H

