#ifndef VIDEO_PROPERTY_H
#define VIDEO_PROPERTY_H

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

namespace webcam_capture  {

/**
 *  Supported video properties.
 */

#ifdef _WIN32
    enum class WEBCAM_CAPTURE_EXPORT VideoProperty {
#elif __APPLE__
    enum class VideoProperty {
#endif
    Brightness,
    Contrast,
    Saturation
};

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_H

