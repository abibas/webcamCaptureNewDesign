/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef CAPABILITY_H
#define CAPABILITY_H
#include <format.h>


namespace webcam_capture  {

    class MediaFoundation_Camera;   //to move to ifdef. TODO

     /**
     * Contains valid values of a Capability
     */
    class Capability {
    public:
        ~Capability() {}
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
        * @return Capability maximum fps
        */
       int getMaxFps() const { return maxFps; }

       /**
        * @return Capability minimum fps
        */
       int getMinFps() const { return minFps; }

       /**
        * @return Capability current fps
        */
       int getCurrentFps() const { return currentFps; }

       /**
        * @param fps Capability carrent fps
        */
       void setCurrentFps(int fps) { currentFps = fps; }


       /**
        * @return Capability index
        */
       int getCapabilityIndex() const { return capabilityIndex; }

       /**
        * @return Pixel format index
        */
       int getPixelFormatIndex() const { return pixelFormatIndex; }
    private:

       /**
        * @param width Capbility width
        * @param height Capability height
        * @param pixelFormat Capability pixel format
        * @param fps Capability FPS
        * @param capabilityIndex Capablity index
        * @param currentFpsIndex Capability fps index
        * @param pixelFormatIndex The pixel format one of the Format enum.
        * @param description Capability description
        */
       Capability(int width, int height, Format pixelFormat, int minFps, int maxFps, int currentFps,
                  int capabilityIndex, int currentFpsIndex, int pixelFormatIndex, std::string description) :
                    width(width),
                    height(height),
                    pixelFormat(pixelFormat),
                    minFps(minFps),
                    maxFps(maxFps),
                    currentFps(currentFps),
                    capabilityIndex(capabilityIndex),
                    currentFpsIndex(currentFpsIndex),
                    pixelFormatIndex(pixelFormatIndex),
                    description(description) {}              
       const int width;
       const int height;
       /**
        * pixelFormat The pixel format one of the Format enum
        */
       const Format pixelFormat;

       /**
        * maxFps Maximum fps value supported by cam
        */
       const int maxFps;

       /**
        * maxFps Minimum fps value supported by cam
        */
       const int minFps;

       /**
        * maxFps Current fps value
        */
       int currentFps;

       /* Set by the capturer implementation */
       /**
        * capabilityIndex Used by the implementation. Is the ID of this specific capability
        */
       const int capabilityIndex;
       /**
        * fpsIndex Used by the implementation, can be an index to an FPS array that is provided by the implementation
        */
       const int currentFpsIndex;
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

