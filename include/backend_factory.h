#ifndef BACKEND_FACTORY_H
#define BACKEND_FACTORY_H

#include <backend_implementation.h>
#include <backend_interface.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Provides access to backends and information of their availability.
 */
class WEBCAM_CAPTURE_EXPORT BackendFactory
{
public:
    /**
     * Creates a backend instance backed by a certain backend implementation.
     * You are responsible for deleting the returned object.
     * @param Implementation backing the backend.
     * @return BackentInterface instance backed by specified backend implementation on success, null on failure.
     */
    static BackendInterface *getBackend(BackendImplementation implementation);

    /**
     * @return List of backends the library was built with support of.
     */
    static std::vector<BackendImplementation> getAvailableBackends();
private:
    BackendFactory();
};

} // namespace webcam_capture

#endif // BACKEND_FACTORY_H
