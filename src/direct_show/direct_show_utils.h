#ifndef DIRECT_SHOW_UTILS_H
#define DIRECT_SHOW_UTILS_H

#include <guiddef.h>
#include <format.h>

namespace webcam_capture {

webcam_capture::Format direct_show_video_format_to_capture_format(GUID guid);

/* Safely release the given obj. */
template <class T> void safeReleaseDirectShow(T **t)
{
    if (*t) {
        (*t)->Release();
        *t = NULL;
    }
}

} // namespace webcam_capture

#endif
