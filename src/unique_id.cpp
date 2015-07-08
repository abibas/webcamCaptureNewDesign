#include <unique_id.h>

namespace webcam_capture {

UniqueId::UniqueId(BackendImplementation implementation) : implementation(implementation)
{
    //empty
}

UniqueId::~UniqueId()
{
    //empty
}

bool UniqueId::operator==(const UniqueId &other)
{
    return this->implementation == other.implementation;
}

bool UniqueId::operator!=(const UniqueId &other)
{
    return !(*this == other);
}
}
