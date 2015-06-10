#ifndef BACKEND_IMPLEMENTATION_H
#define BACKEND_IMPLEMENTATION_H

namespace webcam_capture  {

    /**
     * The class BackendImplementation representing enum of supported video capture frameworks.
     */
    enum class BackendImplementation {MediaFoundation, V4L, AVFoundation};

} // namespace webcam_capture


#endif // BACKEND_IMPLEMENTATION_H

