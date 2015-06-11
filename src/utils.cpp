#include "utils.h"
#include <format.h>

namespace webcam_capture {

  int fps_from_rational(uint64_t num, uint64_t den) {

    char buf[128] = { 0 } ;
    double r = 1.0 / ( double(num) / double(den) );
    float fps = 0.0f;

    sprintf(buf, "%2.02f", r);
    sscanf(buf, "%f", &fps);
    int v = (fps * 100);

    return v;
  }

  std::string format_to_string(int fmt) {
    switch(fmt) {
      case Format::UYVY422:         return "CA_UYVY422";
      case Format::YUYV422:         return "CA_YUYV422";
      case Format::YUV422P:         return "CA_YUV422P";
      case Format::YUV420P:         return "CA_YUV420P";
      case Format::YUV420BP:        return "CA_YUV420BP";
      case Format::YUVJ420P:        return "CA_YUVJ420P";
      case Format::YUVJ420BP:       return "CA_YUVJ420BP";
      case Format::ARGB32:          return "CA_ARGB32";
      case Format::BGRA32:          return "CA_BGRA32";
      case Format::RGB24:           return "CA_RGB24";
      case Format::JPEG_OPENDML:    return "CA_JPEG_OPENDML";
      case Format::H264:            return "CA_H264";
      case Format::MJPEG:           return "CA_MJPEG";
      default:                      return "UNKNOWN_FORMAT";
    }
  }

} // namespace webcam_capture
