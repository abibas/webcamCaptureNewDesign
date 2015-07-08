#include <backend_factory.h>

#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION
#include "../src/media_foundation/media_foundation_backend.h"
#endif
#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW
#include "../src/direct_show/direct_show_backend.h"
#endif
#ifdef V4L
#endif
#ifdef AV_FOUNDATION
#endif


namespace webcam_capture {

BackendFactory::BackendFactory()
{

}

BackendInterface *BackendFactory::getBackend(BackendImplementation implementation)
{
    switch (implementation) {
#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION

        case BackendImplementation::MediaFoundation: {
            return new MediaFoundation_Backend();
        }

#endif
#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW

        case BackendImplementation::DirectShow: {
            return new DirectShow_Backend();
        }

#endif
#ifdef V4L

        case BackendImplementation::v4l : {
            return new V4L_Backend();
        }

#endif
#ifdef AV_FOUNDATION

        case BackendImplementation::AVFoundation : {
            return new AV_Foundation_Backend();
        }

#endif

        default:
            return NULL;
    }
}

std::vector<BackendImplementation> BackendFactory::getAvailableBackends()
{
    std::vector<BackendImplementation> result;
#ifdef WEBCAM_CAPTURE_BACKEND_MEDIA_FOUNDATION
    result.push_back(BackendImplementation::MediaFoundation);
#endif
#ifdef WEBCAM_CAPTURE_BACKEND_DIRECT_SHOW
    result.push_back(BackendImplementation::DirectShow);
#endif
#ifdef V4L
    result.push_back(BackendImplementation::v4l);
#endif

#ifdef AV_FOUNDATION
    result.push_back(BackendImplementation::AVFoundation);
#endif

    return result;
}

} // namespace webcam_capture
