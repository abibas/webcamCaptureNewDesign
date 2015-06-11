#ifndef BACKEND_FACTORY
#define BACKEND_FACTORY
#include <backend_interface.h>
#include <backend_implementation.h>

#ifdef MEDIA_FOUNDATION
    #include "../src/media_foundation/media_foundation_backend.h"
#endif
#ifdef V4L
#endif
#ifdef AV_FOUNDATION
#endif


namespace webcam_capture {

    /**
     *  The class representing the factory for BackendInterface entities creating.
     */
    class BackendFactory {
    public:
        /**
         * @param Specifies a backend you want to use
         * @return creates specific <framework>_backend and if returns BackendInterface*. Returns nullptr if error (if such BackendImplementations doesn't support)
         */      
        static BackendInterface* getBackend(BackendImplementation implementation);
        /**
         * @return Vector of avaliable backend implementations
         */
        static std::vector<BackendImplementation> getAvailableBackends();
    private:
        BackendFactory();
    };

} // namespace webcam_capture

#endif // BACKEND_FACTORY

