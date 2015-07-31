/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_UTILS_H
#define MEDIA_FOUNDATION_UTILS_H

#include "../utils.h"

#include <pixel_format.h>

#include <cassert>
#include <cstring>
#include <cwchar>
#include <vector>

#include <rpc.h>

#ifdef WEBCAM_CAPTURE_DEBUG
#include <iomanip>
#define DEBUG_PRINT_HR_ERROR(msg, hr) DEBUG_PRINT("Error: " << msg << " " << \
                                                  "Error code: " << \
                                                  "0x" << std::setfill('0') << std::setw(8) << std::hex << hr << ".")
#else
#define DEBUG_PRINT_HR_ERROR(msg, hr) DEBUG_PRINT(msg)
#endif

namespace webcam_capture {

webcam_capture::PixelFormat media_foundation_video_format_to_capture_format(GUID guid);

// Convert a WCHAR to a std::string
template<class T> T string_cast(const wchar_t *src, unsigned int codePage = CP_ACP)
{
    assert(src != 0);
    size_t source_length = std::wcslen(src);

    if (source_length > 0) {
        int length = ::WideCharToMultiByte(codePage, 0, src, (int)source_length, NULL, 0, NULL, NULL);

        if (length == 0) {
            return T();
        }

        std::vector<char> buffer(length);
        ::WideCharToMultiByte(codePage, 0, src, (int)source_length, &buffer[0], length, NULL, NULL);

        return T(buffer.begin(), buffer.end());
    } else {
        return T();
    }
}

/* Safely release the given obj. */
template <class T> void safeReleaseMediaFoundation(T **t)
{
    if (*t) {
        (*t)->Release();
        *t = NULL;
    }
}

} // namespace webcam_capture

#endif
