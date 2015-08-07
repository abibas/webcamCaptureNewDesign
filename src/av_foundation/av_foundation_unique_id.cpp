#include "av_foundation_unique_id.h"

namespace webcam_capture {

AVFoundation_UniqueId::AVFoundation_UniqueId(const std::string uniqueId)
    :   UniqueId(BackendImplementation::AVFoundation),
        uniqueId(uniqueId)
{
}

AVFoundation_UniqueId::~AVFoundation_UniqueId()
{
}

bool AVFoundation_UniqueId::equals(const UniqueId &other) const
{
    // "other" must be a UniqueId of the same backend implementation in order to proceed
    if (!UniqueId::equals(other)) {
        return false;
    }

    const AVFoundation_UniqueId &otherUniqueId = static_cast<const AVFoundation_UniqueId &>(other);
    return (uniqueId == otherUniqueId.getId());
}

const std::string &AVFoundation_UniqueId::getId() const
{
    return uniqueId;
}

}// namespace webcam_capture

