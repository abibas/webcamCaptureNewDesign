#ifndef BACKEND_FACTORY_H
#define BACKEND_FACTORY_H

#include <backend_implementation.h>

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

#include <memory>
#include <vector>

namespace webcam_capture {

class BackendInterface;

/**
 * Provides access to backends and information of their availability.
 */
#ifdef _WIN32
class WEBCAM_CAPTURE_EXPORT BackendFactory
#elif __APPLE__
class BackendFactory
#endif
{
public:
    /**
     * Creates a backend instance backed by a certain backend implementation.
     * @param implementation Implementation backing the backend.
     * @return BackentInterface instance backed by specified backend implementation on success, null on failure.
     */
    static std::unique_ptr<BackendInterface> getBackend(BackendImplementation implementation);

    /**
     * @return List of backends the library was built with support of.
     */
    static std::vector<BackendImplementation> getAvailableBackends();

private:
    BackendFactory() = delete;
};

} // namespace webcam_capture

#endif // BACKEND_FACTORY_H
