#ifndef BACKEND_FACTORY
#define BACKEND_FACTORY
#include "backend_interface.h"
#include "backend_implementation.h"

namespace webcam_capture {

    /**
     *  The class representing the factory for BackendInterface entities creating.
     */
    class BackendFactory {
    public:
        /**
         * @param implementation - recieves BackendImplementatuion value from enum
         * @return BackendInterface entity, created from <framework>_backend.
         */
        static BackendInterface* getBackend(BackendImplementation implementation); // создает определенный Backend (mfBackend например) и возвращает как BackendInterface*. возвращает nullptr на ошибку (если такой BackendImplementation неподдерживается например)
        /**
         * @return vector of avaliable BackendInterfaces
         */
        static std::vector<BackendImplementation> getAvailableBackends();
    private:
        BackendFactory();
    };

} // namespace webcam_capture

#endif // BACKEND_FACTORY

