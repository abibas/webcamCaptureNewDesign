#ifndef UTILS_H
#define UTILS_H

#ifdef WEBCAM_CAPTURE_DEBUG

    // apparently VC2013 doesn't support C++11's __func__
    #if defined(__FUNCTION__)
        #define FUNCTION __FUNCTION__
    #elif defined(__FUNC__)
        #define FUNCTION __FUNC__
    #else
        #define FUNCTION "function-name-unavailable"
    #endif

        #include <chrono>
        #include <iomanip>
        #include <iostream>

        #define DEBUG_PRINT(msg) \
                { \
                    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); \
                    std::cerr << "[" << std::put_time(std::localtime(&now), "%H:%M:%S") << "] " \
                              << "(" << FUNCTION  << ")" << std::endl \
                              << "-- " << msg \
                              << std::endl << std::endl \
                              << std::flush; \
                }
#else
    #define DEBUG_PRINT(msg)
#endif

#define FPS_FROM_RATIONAL(x, y) (static_cast<float>(x)/(y))
#define FPS_EQUAL(x, y) ((((x) + 0.008) > (y)) && (((x) - 0.008) < (y)))

#endif // UTILS_H
