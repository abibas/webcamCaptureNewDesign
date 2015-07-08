#ifndef DIRECT_SHOW_UNIQUE_ID_H
#define DIRECT_SHOW_UNIQUE_ID_H
#include <unique_id.h>
#include <windows.h>

namespace webcam_capture {

class DirectShow_UniqueId : public UniqueId
{
public:
    DirectShow_UniqueId(WCHAR *uniqueId);
    ~DirectShow_UniqueId();

    WCHAR *getId();
    bool operator==(const UniqueId &other) override;
    bool operator!=(const UniqueId &other) override;
private:
    WCHAR *uniqueId;
};
}

#endif // MEDIA_FOUNDATION_UNIQUE_ID_H
