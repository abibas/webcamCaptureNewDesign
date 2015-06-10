#ifndef CAPABILITY_H
#define CAPABILITY_H
#include "format.h"


namespace webcam_capture  {
    /**
     * Contains valid values of a Capability
     */
    class Capability {
    public:

        /**
        * @return Capbility width
        */
       int getWidth() const { return width; }

       /**
        * @return Capability height
        */
       int getHeight() const { return height; }

       /**
        * @return Capability pixe; format
        */
       Format getPixelFormat() const { return pixelFormat; }

       /**
        * @return Capability FPS
        */
       int getFps() const { return fps; }
    private:

       /**
        * @param width Capbility width
        * @param height Capability height
        * @param pixelFormat Capability pixel format
        * @param fps Capability FPS
        * @param capabilityIndex Capablity index
        * @param fpsIndex Capability fps index
        * @param pixelFormatIndex The pixel format one of the Format enum.
        * @param description Capability description
        */
       Capability(int width, int height, Format pixelFormat, int fps,
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

       const int width;
       const int height;
       /**
        * pixelFormat The pixel format one of the Format enum
        */
       const Format pixelFormat;
       const int fps;

       /* Set by the capturer implementation */
       /**
        * capabilityIndex Used by the implementation. Is the ID of this specific capability
        */
       const int capabilityIndex;
       /**
        * fpsIndex Used by the implementation, can be an index to an FPS array that is provided by the implementation
        */
       const int fpsIndex;
       /**
        * pixelFormatIndex Used by the implementation, represents an index to the pixel format for the implementation
        */
       const int pixelFormatIndex;
       /**
        * description A capture driver can add some additional information here
        */
       const std::string description;

       friend class MediaFoundation_Camera;  //to move to ifdef. TODO
    };

} // namespace webcam_capture

#endif // CAPABILITY_H

