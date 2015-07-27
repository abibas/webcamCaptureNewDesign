/* -*-C-*- */
#import "av_foundation_implementation.h"
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
+ (int) getDevices: (std::vector<AVFoundationDeviceInfo>&) result {

    NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

    for(AVCaptureDevice* dev in devices) {

        std::string name = [[dev localizedName] UTF8String];
        std::string uniqueId = [[dev uniqueID] UTF8String];
        result.push_back(AVFoundationDeviceInfo(name, uniqueId));
  }
    
    return (int) result.size();
}

// Get all the capabilities for the given device
- (int) getCapabilities: (std::vector<AVFoundationCapability>&) result {

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
            result.push_back(AVFoundationCapability(pxt, resolution.width, resolution.height, minFrameRate, maxFrameRate));
            if ( !FPS_EQUAL(minFrameRate, maxFrameRate) ) {
                //TODO to check. Seems that minFps = maxFps all the time
            }
        }
    }
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

int webcam_capture_av_get_devices(std::vector<AVFoundationDeviceInfo>& result) {
    return [AVFoundation_Implementation getDevices:result];
}

int webcam_capture_av_get_capabilities(void* cap, std::vector<AVFoundationCapability>& result) {
    return [(id)cap getCapabilities:result];
}

@end
