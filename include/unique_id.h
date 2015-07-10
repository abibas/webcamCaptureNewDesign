#ifndef UNIQUE_ID_H
#define UNIQUE_ID_H

#include <backend_implementation.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

/**
 * Uniquely identifies a camera.
 * While the actual data is hidden by the backend implementations, you can still use it for comparison.
 */
class WEBCAM_CAPTURE_EXPORT UniqueId
{
public:
    UniqueId(BackendImplementation implementation);
    virtual ~UniqueId();

    virtual bool operator==(const UniqueId &other);
    virtual bool operator!=(const UniqueId &other);

protected:
    BackendImplementation implementation;
};

} // namespace webcam_capture

#endif // UNIQUE_ID_H
