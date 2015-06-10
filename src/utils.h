#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <string>

namespace webcam_capture {
  
  int fps_from_rational(uint64_t num, uint64_t den); // TODO to research what this method is doing
  std::string format_to_string(int fmt);             // TODO to remove this method.
  
} // namespace webcam_capture

#endif
