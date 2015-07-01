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
        if (_wcsicmp(uniqueId, otherUniqueId.getId()) == 0) return true; //if equals
        return false;
    }

    bool MediaFoundation_UniqueId::operator!=(const UniqueId& other) {
        return !(*this == other);
    }

    WCHAR * MediaFoundation_UniqueId::getId(){
        return uniqueId;
    }
}// namespace webcam_capture

