#ifndef AV_FOUNDATION_UNIQUE_ID_H
#define AV_FOUNDATION_UNIQUE_ID_H

#include <unique_id.h>
#include <string>

namespace webcam_capture {

class AVFoundation_UniqueId : public UniqueId
{
public:
    AVFoundation_UniqueId(const std::string uniqueId);
    ~AVFoundation_UniqueId();

    const std::string &getId() const;

protected:
    bool equals(const UniqueId &other) const override;

private:    
    std::string uniqueId;
};

} // namespace webcam_capture

#endif // AV_FOUNDATION_UNIQUE_ID_H
