#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H
namespace webcamCapture  {

    struct PixelBuffer{
       PixelBuffer();
       int setup(int w, int h, int fmt);    /* Set the strides, widths, heights, nbyte values for the given pixel format (CA_UYVY422, CA_YUV420P    etc..) and video frame size.. Returns 0 on success otherwise < 0. */
       uint8_t* pixels;                     /* When data is one continuous block of member you can use this, otherwise it points to the same location as plane[0]. */
       uint8_t* plane[3];                   /* Pointers to the pixel data; when we're a planar format all members are set, if packets only plane[0] */
       size_t stride[3];                    /* The number of bytes you should jump per row when reading the pixel data. Note that some buffer may have extra bytse at the end for memory alignment. */
       size_t width[3];                     /* The width; when planar each plane will have it's own value; otherwise only the first index is set. */
       size_t height[3];                    /* The height; when planar each plane will have it's own value; otherwise only the first index is set. */
       size_t offset[3];                    /* When the data is planar but packed, these contains the byte offsets from the first byte / plane. e.g. you can use this with YUV420P. */
       size_t nbytes;                       /* The total number of bytes that make up the frame. This doesn't have to be one continuous array when the data is planar. */
       int pixel_format;                    /* The pixel format of the buffer; e.g. CA_YUYV422, CA_UYVY422, CA_JPEG_OPENDML, etc.. */
    };

} // namespace webcamCapture

#endif // PIXEL_BUFFER_H

