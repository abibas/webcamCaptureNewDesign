#ifndef CAPABILITY_H
#define CAPABILITY_H

namespace webcamCapture  {

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

} // namespace webcamCapture

#endif // CAPABILITY_H

