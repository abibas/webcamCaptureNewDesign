/* -*-C-*- */
#import "av_foundation_implementation.h"
#include "av_foundation_unique_id.h"
#include <backend_implementation.h>
#include <pixel_format.h>
#include "av_foundation_utils.h"
#include "../utils.h"
using namespace webcam_capture;

@implementation AVFoundation_Implementation 

// Initializes the capturer
- (id) init: (std::string&) deviceId {

    self = [super init];

    if(self) {
        cb_frame = nil;
        session = nil;
        input = nil;
        output = nil;
        pixel_format = 0;
        is_pixel_buffer_set = 0;
    }

    NSString *devId = [NSString stringWithUTF8String:deviceId.c_str()];
    currentDevice = [AVCaptureDevice deviceWithUniqueID:devId];

    return self;
}

// Deallocate all retained objects
- (void) dealloc {

    //[self closeDevice]; - TODO

    cb_frame = nil;

    [super dealloc];
}



  
// Get a list with all devices
+ (int) getDevices: (std::vector<CameraInformation>&) result {

    NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

    for(AVCaptureDevice* dev in devices) {

        std::string name = [[dev localizedName] UTF8String];
        std::string uniqueIdLink = [[dev uniqueID] UTF8String];

//TODO fix error - can't do "make_shared"
//    std::shared_ptr<webcam_capture::AVFoundation_UniqueId> uniqueId =
//            std::make_shared<webcam_capture::AVFoundation_UniqueId>(uniqueIdLink);

        AVFoundation_UniqueId *uniqueId =
                new AVFoundation_UniqueId(uniqueIdLink);
        CameraInformation found_device(uniqueId, name);
        result.push_back(found_device);
  }
    
    return (int) result.size();
}

// Get all the capabilities for the given device
- (int) getCapabilities: (std::vector<AVCapabilityInfo>&) result {

    //CapabilityTreeBuilder capabilityBuilder;
    for(AVCaptureDeviceFormat* format in [currentDevice formats]) {
        if(![[format mediaType] isEqualToString: AVMediaTypeVideo]) {
          continue;
        }
        CMFormatDescriptionRef fmt_description = [format formatDescription];
        CMPixelFormatType fmt_sub_type = CMFormatDescriptionGetMediaSubType(fmt_description);
        PixelFormat pxt = av_foundation_video_format_to_capture_format(fmt_sub_type);
        CMVideoDimensions resolution = CMVideoFormatDescriptionGetDimensions([format formatDescription]);

        for(AVFrameRateRange* fps in [format videoSupportedFrameRateRanges]) {
            Float64 maxFrameRate = [fps maxFrameRate];
            Float64 minFrameRate = [fps minFrameRate];
            result.push_back(AVCapabilityInfo(pxt, resolution.width, resolution.height, minFrameRate, maxFrameRate));
            if ( !FPS_EQUAL(minFrameRate, maxFrameRate) ) {
                //TODO to check. Seems that minFps = maxFps all the time
            }
        }
    }
/*
  AVCaptureDevice* d = [self getCaptureDevice: dev];
  if(d == nil) {
    return -1;
  }

  int fps_dx = 0;
  int fmt_dx = 0;

  for(AVCaptureDeviceFormat* f in [d formats]) {

    if(![[f mediaType] isEqualToString: AVMediaTypeVideo]) {
      fmt_dx++;
      continue;
    }

    CMFormatDescriptionRef fmt_description = [f formatDescription];
    CMPixelFormatType fmt_sub_type = CMFormatDescriptionGetMediaSubType(fmt_description);
    CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions([f formatDescription]);
    fps_dx = 0;

    for(AVFrameRateRange* fps in [f videoSupportedFrameRateRanges]) {

      ca::Capability cap;

      if ([fps minFrameRate] != [fps maxFrameRate]) {
#if !defined(NDEBUG)
        printf("@todo -  Need to handle a capability with different min/max framerates. This works but need more testing. min framerate: %f, max framerate: %f\n", (float)[fps minFrameRate], (float)[fps maxFrameRate]);
#endif
        cap.fps = ca::fps_from_rational((uint64_t)1, (uint64_t) [fps maxFrameRate]);
      }
      else {
        CMTime dur = [fps maxFrameDuration];
        cap.fps = ca::fps_from_rational((uint64_t)dur.value, (uint64_t)dur.timescale);
      }

      cap.width = dims.width;
      cap.height = dims.height;
      cap.pixel_format = [self getCapturePixelFormat: fmt_sub_type];
      cap.user = (void*) f;
      cap.capability_index = (int) result.size();
      cap.fps_index = fps_dx;
      cap.pixel_format_index = fmt_dx;

      result.push_back(cap);

      ++fps_dx;
    }

    fmt_dx++;
  }
 */
  return (int) result.size();
}

/* C-interface */
/* -------------------------------------- */

void* webcam_capture_av_alloc(std::string deviceId) {
    return (void*)[[AVFoundation_Implementation alloc] init:deviceId];
}

void webcam_capture_av_dealloc(void* cap) {
    if(cap) {
        [(id)cap dealloc];
    }
}

int webcam_capture_av_get_devices(std::vector<CameraInformation>& result) {
    return [AVFoundation_Implementation getDevices:result];
}

int webcam_capture_av_get_capabilities(void* cap, std::vector<AVCapabilityInfo>& result) {
    return [(id)cap getCapabilities:result];
}

@end
