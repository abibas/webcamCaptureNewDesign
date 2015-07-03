/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef UTILS_H
#define UTILS_H

#ifdef WEBCAM_CAPTURE_DEBUG
    #define DEBUG_PRINT(x) std::cerr << x << std::endl
#else
    #define DEBUG_PRINT(x)
#endif

#include <stdint.h>
#include <string>

namespace webcam_capture {
  
    int fps_from_rational(uint64_t num, uint64_t den); // convert fps from UINT32 high, low - to 1 int value.
  
} // namespace webcam_capture

#endif
