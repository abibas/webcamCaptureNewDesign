#include <backend_factory.h>

namespace webcam_capture {

    BackendFactory::BackendFactory(){

    }

    BackendInterface* BackendFactory::getBackend(BackendImplementation implementation) {
        switch (implementation) {
            #ifdef MEDIA_FOUNDATION
            case BackendImplementation::MediaFoundation: {
                return new MediaFoundation_Backend();
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
            default: return NULL;
            }
    }

    std::vector<BackendImplementation> BackendFactory::getAvailableBackends(){
        std::vector<BackendImplementation> result;
        #ifdef MEDIA_FOUNDATION
            result.push_back(BackendImplementation::MediaFoundation);
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
