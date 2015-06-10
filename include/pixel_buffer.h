#ifndef PIXEL_BUFFER_H
#define PIXEL_BUFFER_H
#include "format.h"

namespace webcam_capture  {
    /**
     *  Structure representing datatype for captured frames
     */
    struct PixelBuffer{
       PixelBuffer();
       /**
        * @param w Sets frame width
        * @param h Sets frame height
        * @param fmt Set frame format
        */
       void setup(int w, int h, int fmt);

       /**
        *  When data is one continuous block of member you can use this, otherwise it points to the same location as plane[0].
        */
       uint8_t* pixels;

       /**
        * Pointers to the pixel data; when we're a planar format all members are set, if packets only plane[0]
        */
       uint8_t* plane[3];

       /**
        *  The number of bytes you should jump per row when reading the pixel data. Note that some buffer may have extra bytse at the end for memory alignment.
        */
       size_t stride[3];

       /**
        * The width; when planar each plane will have it's own value; otherwise only the first index is set.
        */
       size_t width[3];

       /**
        * The height; when planar each plane will have it's own value; otherwise only the first index is set.
        */
       size_t height[3];

       /**
        * When the data is planar but packed, these contains the byte offsets from the first byte / plane. e.g. you can use this with YUV420P.
        */
       size_t offset[3];

       /**
        * The total number of bytes that make up the frame. This doesn't have to be one continuous array when the data is planar.
        */
       size_t nbytes;

       /**
        * The pixel buffer format one of the the Format enum.
        */
       Format pixel_format;
    };

} // namespace webcam_capture

#endif // PIXEL_BUFFER_H

