#ifndef BACKEND_IMPLEMENTATION_H
#define BACKEND_IMPLEMENTATION_H

namespace webcam_capture  {

    /**
     * Enum of supported video capture frameworks
     */
    enum class BackendImplementation {MediaFoundation, v4l, AVFoundation};

} // namespace webcam_capture


#endif // BACKEND_IMPLEMENTATION_H

