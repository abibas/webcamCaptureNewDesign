#include "winapi_shared_unique_id.h"

namespace webcam_capture {

WinapiShared_UniqueId::WinapiShared_UniqueId(WCHAR* uniqueId, BackendImplementation implementation)
    :   UniqueId(implementation),
        uniqueId(uniqueId)
{
}

WinapiShared_UniqueId::~WinapiShared_UniqueId()
{
}

bool WinapiShared_UniqueId::equals(const UniqueId &other) const
{
    // "other" must be a UniqueId of the same backend implementation in order to proceed
    if (!UniqueId::equals(other)) {
        return false;
    }

    const WinapiShared_UniqueId &otherUniqueId = static_cast<const WinapiShared_UniqueId &>(other);
    bool result = false;
    size_t localStrLen = wcslen(uniqueId);
    size_t otherStrLen = wcslen(otherUniqueId.getId());
    WCHAR *localStr = new WCHAR[localStrLen];
    WCHAR *otherStr = new WCHAR[otherStrLen];
    //need to compare string without this "{...}" value.
    wcsncpy(localStr, uniqueId, localStrLen); //copy string
    wcsncpy(otherStr, otherUniqueId.getId(), otherStrLen); //copy string
    localStr = wcstok(localStr, L"{");
    otherStr = wcstok(otherStr, L"{");

    if (_wcsicmp(localStr, otherStr) == 0) {
        result = true;    //if equals
    }

    delete[] localStr;
    delete[] otherStr;
    return result;
}

WCHAR* WinapiShared_UniqueId::getId() const
{
    return uniqueId;
}

}// namespace webcam_capture

