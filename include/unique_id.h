#ifndef UNIQUE_ID
#define UNIQUE_ID

#include <backend_implementation.h>
#include <webcam_capture_export.h>

namespace webcam_capture {

    class WEBCAM_CAPTURE_EXPORT UniqueId {
    public:
        UniqueId(BackendImplementation implementation);
        virtual ~UniqueId();

        /**
         * @param other Param to compare with
         * @return Bool result
         */
        virtual bool operator==(const UniqueId& other);

        /**
         * @param other Param to compare with
         * @return Bool result
         */
        virtual bool operator!=(const UniqueId& other);
    protected:
        BackendImplementation implementation;
    };
}

#endif // UNIQUE_ID
