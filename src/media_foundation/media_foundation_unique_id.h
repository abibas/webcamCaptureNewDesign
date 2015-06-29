#ifndef MEDIA_FOUNDATION_UNIQUE_ID_H
#define MEDIA_FOUNDATION_UNIQUE_ID_H
#include <unique_id_interface.h>

#include <windows.h>

namespace webcam_capture {

    class MediaFoundation_UniqueId : public UniqueIdInterface {
    public:
        MediaFoundation_UniqueId(WCHAR *uniqueId);
        ~MediaFoundation_UniqueId();

        WCHAR * getId() { return uniqueId; }
        bool operator==(UniqueIdInterface * other);
        bool operator!=(UniqueIdInterface * other);
    private:
        WCHAR *uniqueId;
    };
}

#endif // MEDIA_FOUNDATION_UNIQUE_ID_H
