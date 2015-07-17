#include "utils.h"
#include <format.h>

namespace webcam_capture {

int fps_from_rational(uint64_t num, uint64_t den)
{
    char buf[128] = { 0 } ;
    double r = 1.0 / (double(num) / double(den));
    float fps = 0.0f;

    sprintf(buf, "%2.02f", r);
    sscanf(buf, "%f", &fps);
    int v = (fps * 100);

    return v;
}

int check_inputed_capability_params( const std::vector<CapabilityFormat> &capabilities,
                                     const CapabilityFormat &capabilityFormat,
                                     const CapabilityResolution &capabilityResolution,
                                     const CapabilityFps &capabilityFps )
{
//check format
    bool isFormatValid = false;
    int formatIndex = 0;
    for (int i = 0; i < capabilities.size(); i++) {
        if (capabilities.at(i).getPixelFormat() == capabilityFormat.getPixelFormat()) {
            formatIndex = i;
            isFormatValid = true;
            break;
        }
    }
    if (!isFormatValid) {
        DEBUG_PRINT("Error: cannot found such capabilityFormat in capabilities.\n");
        return -5;
    }

//chech resolution
    const std::vector<CapabilityResolution> &resolutionVectorBuf = capabilities.at(formatIndex).getResolutions();
    bool isResolutionValid = false;
    int resolutionsIndex = 0;
    for (int j = 0; j < resolutionVectorBuf.size(); j++) {
        if (resolutionVectorBuf.at(j).getHeight()  == capabilityResolution.getHeight() &&
            resolutionVectorBuf.at(j).getWidth() == capabilityResolution.getWidth()) {
            resolutionsIndex = j;
            isResolutionValid = true;
            break;
        }
    }

    if (!isResolutionValid) {
        DEBUG_PRINT("Error: cannot found such capabilityResolution in capabilities.\n");
        return -6;
    }

//check fps
    const std::vector<CapabilityFps> &fpsVectorBuf = resolutionVectorBuf.at(resolutionsIndex).getFpses();
    bool isFpsValid = false;
    for (int k = 0; k < fpsVectorBuf.size(); k++) {
        if (fpsVectorBuf.at(k).getFps() == capabilityFps.getFps()) {
            isFpsValid = true;
        }
    }

    if (!isFpsValid) {
        DEBUG_PRINT("Error: cannot found such capabilityFps in capabilities.\n");
        return -7;
    }

    return 1; //success
}

} // namespace webcam_capture
