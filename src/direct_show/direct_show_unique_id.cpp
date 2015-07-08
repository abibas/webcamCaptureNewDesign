#include "direct_show_unique_id.h"

namespace webcam_capture {

DirectShow_UniqueId::DirectShow_UniqueId(WCHAR *uniqueId)
    :   UniqueId(BackendImplementation::DirectShow),
        uniqueId(uniqueId)
{
}

DirectShow_UniqueId::~DirectShow_UniqueId()
{
}

bool DirectShow_UniqueId::operator==(const UniqueId &other)
{
    DirectShow_UniqueId otherUniqueId = static_cast<const DirectShow_UniqueId &>(other);
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

bool DirectShow_UniqueId::operator!=(const UniqueId &other)
{
    return !(*this == other);
}

WCHAR *DirectShow_UniqueId::getId()
{
    return uniqueId;
}
}// namespace webcam_capture

