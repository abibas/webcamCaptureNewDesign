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

#include <capability.h>
#include <vector>
#include <stdint.h>
#include <string>
#include <iostream> ///for cerr

namespace webcam_capture {

int fps_from_rational(uint64_t num, uint64_t den); // convert fps from UINT32 high, low - to 1 int value.

int check_inputed_capability_params( const std::vector<CapabilityFormat> &capabilities,
                                     const CapabilityFormat &capabilityFormat,
                                     const CapabilityResolution &capabilityResolution,
                                     const CapabilityFps &capabilityFps ); // check the Capability inputed params

} // namespace webcam_capture

#endif
