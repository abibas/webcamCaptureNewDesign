#ifndef WINAPI_SHARED_UNIQUE_ID_H
#define WINAPI_SHARED_UNIQUE_ID_H
#include <unique_id.h>
#include <windows.h>

namespace webcam_capture {

class WinapiShared_UniqueId : public UniqueId
{
public:
    WinapiShared_UniqueId(WCHAR *uniqueId, BackendImplementation implementation);
    ~WinapiShared_UniqueId();

    WCHAR *getId();
    bool operator==(const UniqueId &other) override;
    bool operator!=(const UniqueId &other) override;
private:    
    WCHAR *uniqueId; //TODO To consider do we need to save links in WCHAR* - we just could use std::wstring.
};
}

#endif // MEDIA_FOUNDATION_UNIQUE_ID_H
