#include <backend_factory.h>

namespace webcam_capture {

    BackendFactory::BackendFactory(){

    }

    static BackendInterface* BackendFactory::getBackend(BackendImplementation implementation) {
        //TODO Create Backend
        return NULL;
    }

    static std::vector<BackendImplementation> BackendFactory::getAvailableBackends(){
        //TODO get by ifdef current Available Frameworks (BackendImplemantations)
        std::vector<BackendImplementation> x;
        return x;
    }

} // namespace webcam_capture
