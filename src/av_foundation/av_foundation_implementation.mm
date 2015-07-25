/* -*-C-*- */
#import "av_foundation_implementation.h"
#include "av_foundation_unique_id.h"
#include <backend_implementation.h>

@implementation AVFoundation_Implementation 

// Initializes the capturer
- (id) init {

  self = [super init];

  if(self) {
//    cb_frame = nil;
//    cb_user= nil;
    session = nil;
    input = nil;
    output = nil;
    pixel_format = 0;
    is_pixel_buffer_set = 0;
  }

  return self;
}

// Deallocate all retained objects
- (void) dealloc {

  [self closeDevice];

//  cb_frame = nil;
//  cb_user = nil;
//  pixel_buffer.user = NULL;

  [super dealloc];
}



  
// Get a list with all devices
+ (int) getDevices: (std::vector<webcam_capture::CameraInformation>&) result {

  NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

  for(AVCaptureDevice* dev in devices) {

    std::string name = [[dev localizedName] UTF8String];
    std::string uniqueIdLink = [[dev uniqueID] UTF8String];

    //TODO fix error - can't do "make_shared"
//    std::shared_ptr<webcam_capture::AVFoundation_UniqueId> uniqueId =
//            std::make_shared<webcam_capture::AVFoundation_UniqueId>(uniqueIdLink);

    webcam_capture::AVFoundation_UniqueId *uniqueId =
            new webcam_capture::AVFoundation_UniqueId(uniqueIdLink);
    webcam_capture::CameraInformation found_device(uniqueId, name);
    result.push_back(found_device);
  }
    
  return (int) result.size();
}

/* C-interface */
/* -------------------------------------- */

void* webcam_capture_av_alloc() {
  return (void*)[[AVFoundation_Implementation alloc] init];
}

void webcam_capture_av_dealloc(void* cap) {
  if(cap) {
    [(id)cap dealloc];
  }
}

int webcam_capture_av_get_devices(std::vector<webcam_capture::CameraInformation>& result) {
  return [AVFoundation_Implementation getDevices:result];
}

@end
