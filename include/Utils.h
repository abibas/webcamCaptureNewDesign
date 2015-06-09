#ifndef VIDEO_CAPTURE_UTILS_H
#define VIDEO_CAPTURE_UTILS_H

#include <stdint.h>
#include <string>
#include "Types.h"

namespace ca {
  
  int fps_from_rational(uint64_t num, uint64_t den); // TODO to replace this and carefully debug this method... maybe this is don't need
  std::string format_to_string(int fmt);
  
} // namespace ca

#endif
