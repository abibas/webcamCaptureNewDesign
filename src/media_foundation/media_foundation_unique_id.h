#ifndef MEDIA_FOUNDATION_UNIQUE_ID_H
#define MEDIA_FOUNDATION_UNIQUE_ID_H
#include <unique_id.h>

#include <windows.h>

namespace webcam_capture {

class MediaFoundation_UniqueId : public UniqueId
{
public:
    MediaFoundation_UniqueId(WCHAR *uniqueId);
    ~MediaFoundation_UniqueId();

    WCHAR *getId();
    bool operator==(const UniqueId &other) override;
    bool operator!=(const UniqueId &other) override;
private:
    WCHAR *uniqueId;
};
}

#endif // MEDIA_FOUNDATION_UNIQUE_ID_H
