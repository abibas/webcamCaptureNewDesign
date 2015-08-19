#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <backend_implementation.h>

#ifdef _WIN32
    #include <webcam_capture_export.h>
#elif __APPLE__
    //nothing to include
#endif

namespace webcam_capture {

/**
 * Uniquely identifies a camera.
 * While the actual data is hidden by the backend implementations, you can still use it for comparison.
 */
class WEBCAM_CAPTURE_EXPORT UniqueId
{
public:
    UniqueId(const UniqueId &) = delete;
    virtual ~UniqueId();

    UniqueId &operator=(const UniqueId &) = delete;

    bool operator==(const UniqueId &other) const;
    bool operator!=(const UniqueId &other) const;

protected:
    UniqueId(BackendImplementation implementation);

    virtual bool equals(const UniqueId &other) const;

    const BackendImplementation implementation;
};

} // namespace webcam_capture

#endif // UNIQUE_ID_H
