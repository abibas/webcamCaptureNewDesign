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

} // namespace webcam_capture
