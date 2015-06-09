#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <functional>
#include <stdint.h>

namespace ca  {

    enum class VideoProperty { Brightness, Contrast, Saturation};
    enum class BackendImplementation {MediaFoundation, V4L, AVFoundation};
    enum class Format { UYVY422 = 1,    /* Cb Y0 Cr Y1 */
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
                        MJPEG           /* MJPEG 2*/
                      };

    struct PixelBuffer{
       PixelBuffer();
       uint8_t* pixels;                     /* When data is one continuous block of member you can use this, otherwise it points to the same location as plane[0]. */
       uint8_t* plane[3];                   /* Pointers to the pixel data; when we're a planar format all members are set, if packets only plane[0] */
       size_t stride[3];                    /* The number of bytes you should jump per row when reading the pixel data. Note that some buffer may have extra bytse at the end for memory alignment. */
       size_t width[3];                     /* The width; when planar each plane will have it's own value; otherwise only the first index is set. */
       size_t height[3];                    /* The height; when planar each plane will have it's own value; otherwise only the first index is set. */
       size_t offset[3];                    /* When the data is planar but packed, these contains the byte offsets from the first byte / plane. e.g. you can use this with YUV420P. */
       size_t nbytes;                       /* The total number of bytes that make up the frame. This doesn't have to be one continuous array when the data is planar. */
       int pixel_format;                    /* The pixel format of the buffer; e.g. CA_YUYV422, CA_UYVY422, CA_JPEG_OPENDML, etc.. */
    };

    class VideoPropertyRange {
        public:
           VideoPropertyRange (int min, int max, int delta, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(delta),
               defaultValue(defaultValue) {}
           ~VideoPropertyRange();
           int getMinValue() const { return minValue; }
           int getMaxValue() const { return maxValue; }
           int getStepValue() const { return stepValue; }
           int getDefaultValue() const { return defaultValue; }
        private:
           const int minValue; 			//min Value
           const int maxValue;			//max Value
           const int stepValue;         //step Value
           const int defaultValue;      //default Value
    };

    class CameraInformation {
    public:
      CameraInformation(int cameraId, std::string cameraName) : cameraId(cameraId), cameraName (cameraName) {}
      ~CameraInformation();

      int getDeviceId() const { return cameraId; }
      std::string getCameraName() const { return cameraName; }

    private:
      const int cameraId;
      const std::string cameraName;
    };

    class Capability {
    public:
       int getWidth() const { return width; }
       int getHeight() const { return height; }
       int getPixelFormat() const { return pixelFormat; }
       int getFps() const { return fps; }
    private:
       Capability(int width, int height, int pixelFormat, int fps,
                  int capabilityIndex, int fpsIndex, int pixelFormatIndex, std::string description) :
                    width(width),
                    height(height),
                    pixelFormat(pixelFormat),
                    fps(fps),
                    capabilityIndex(capabilityIndex),
                    fpsIndex(fpsIndex),
                    pixelFormatIndex(pixelFormatIndex),
                    description(description) {}
       ~Capability();

       const int width;                 /* Width for this capability. */
       const int height;                /* Height for this capability. */
       const int pixelFormat;           /* The pixel format for this capability, one of (CA_*)  */
       const int fps;                   /* The FPS, see CA_FPS_* above. */

       /* Set by the capturer implementation */
       const int capabilityIndex;       /* Used by the implementation. Is the ID of this specific capability */
       const int fpsIndex;              /* Used by the implementation, can be an index to an FPS array that is provided by the implementation */
       const int pixelFormatIndex;      /* Used by the implementation, represents an index to the pixel format for the implementation */
       const std::string description;   /* A capture driver can add some additional information here. */

       friend class MediaFoundation_Camera;  //to move to ifdef. TODO
    };



    //callback tepydefs
    typedef std::function<void(PixelBuffer& buffer)> frame_callback;
    typedef std::function<void(CameraInformation& information)> notifications_callback;
} // namespace ca

#endif // TYPES_H

