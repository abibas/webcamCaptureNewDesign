#include "media_foundation_unique_id.h"

namespace webcam_capture {

    MediaFoundation_UniqueId::MediaFoundation_UniqueId(WCHAR* uniqueId)
        :   UniqueId(BackendImplementation::MediaFoundation),
            uniqueId(uniqueId) {
    }

    MediaFoundation_UniqueId::~MediaFoundation_UniqueId() {
    }

    bool MediaFoundation_UniqueId::operator==(const UniqueId& other) {
        MediaFoundation_UniqueId otherUniqueId = static_cast<const MediaFoundation_UniqueId &>(other);                
        bool result = false;
        size_t localStrLen = wcslen(uniqueId);
        size_t otherStrLen = wcslen(otherUniqueId.getId());
        WCHAR * localStr = new WCHAR[localStrLen];
        WCHAR * otherStr = new WCHAR[otherStrLen];
        //need to compare string without this "{...}" value.
        wcsncpy(localStr, uniqueId, localStrLen); //copy string
        wcsncpy(otherStr, otherUniqueId.getId(), otherStrLen); //copy string
        localStr = wcstok(localStr, L"{");
        otherStr = wcstok(otherStr, L"{");
        if (_wcsicmp(localStr, otherStr) == 0) result = true; //if equals
        delete[] localStr;
        delete[] otherStr;
        return result;
    }

    bool MediaFoundation_UniqueId::operator!=(const UniqueId& other) {
        return !(*this == other);
    }

    WCHAR * MediaFoundation_UniqueId::getId(){
        return uniqueId;
    }
}// namespace webcam_capture

