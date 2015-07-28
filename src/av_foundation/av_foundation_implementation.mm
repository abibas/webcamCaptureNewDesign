/* -*-C-*- */
#import "av_foundation_implementation.h"
#include <backend_implementation.h>
#include <pixel_format.h>
#include "av_foundation_utils.h"
#include "../utils.h"


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
+ (int) getDevices: (std::vector<webcam_capture::AVFoundationDeviceInfo>&) result {

    NSArray* devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

    for(AVCaptureDevice* dev in devices) {

        std::string name = [[dev localizedName] UTF8String];
        std::string uniqueId = [[dev uniqueID] UTF8String];
        result.push_back(webcam_capture::AVFoundationDeviceInfo(name, uniqueId));
  }
    
    return (int) result.size();
}

// Get all the capabilities for the given device
- (int) getCapabilities: (std::vector<webcam_capture::AVFoundationCapability>&) result {

    //CapabilityTreeBuilder capabilityBuilder;
    for(AVCaptureDeviceFormat* format in [currentDevice formats]) {
        if(![[format mediaType] isEqualToString: AVMediaTypeVideo]) {
          continue;
        }
        CMFormatDescriptionRef fmt_description = [format formatDescription];
        CMPixelFormatType fmt_sub_type = CMFormatDescriptionGetMediaSubType(fmt_description);
        webcam_capture::PixelFormat pxt = webcam_capture::av_foundation_video_format_to_capture_format(fmt_sub_type);
        CMVideoDimensions resolution = CMVideoFormatDescriptionGetDimensions([format formatDescription]);

        for(AVFrameRateRange* fps in [format videoSupportedFrameRateRanges]) {
            Float64 maxFrameRate = [fps maxFrameRate];
            Float64 minFrameRate = [fps minFrameRate];
            result.push_back(webcam_capture::AVFoundationCapability(pxt, resolution.width, resolution.height, minFrameRate, maxFrameRate));
            if ( !FPS_EQUAL(minFrameRate, maxFrameRate) ) {
                //TODO to check. Seems that minFps = maxFps all the time
            }
        }
    }
  return (int) result.size();
}

// start video capturing
- (int) startCapturing: (webcam_capture::PixelFormat&) pixelFormat width: (int) w height: (int) h fps: (float) fps frameCB: (webcam_capture::FrameCallback) cb{
    cb_frame = cb;
    if(input != nil) {
      DEBUG_PRINT("Error: device already opened.\n");
      return -1;
    }

    /* Get the input device */
    NSError* err = nil;
    input = [AVCaptureDeviceInput deviceInputWithDevice: currentDevice error:&err];
    if(input == nil) {
        DEBUG_PRINT("Error: device already opened.\n");
        return -2;
    }

    [input retain];

    /* Create session */
    session = [[AVCaptureSession alloc] init];
    if(session == nil) {
        DEBUG_PRINT("Error: cannot create the capture session.\n");
        return -4;
    }

    [session beginConfiguration];
    {
        /* Get the capabilities for this device */
//        webcam_capture::AVFoundationCapability cap();
        std::vector<webcam_capture::AVFoundationCapability> capabilities;
        [self getCapabilities: capabilities];
        assert(capabilities.size() > 0); /// TODO

        //get format type
        AVCaptureDeviceFormat* avFormat;
        AVFrameRateRange* avFrameRate;
        CMPixelFormatType fmt_sub_type;
        bool isCapabilitiesValid = false;
        for(avFormat in [currentDevice formats]) {
            if(![[avFormat mediaType] isEqualToString: AVMediaTypeVideo]) {
              continue;
            }
            CMFormatDescriptionRef fmt_description = [avFormat formatDescription];
            fmt_sub_type = CMFormatDescriptionGetMediaSubType(fmt_description);
            webcam_capture::PixelFormat pxt = webcam_capture::av_foundation_video_format_to_capture_format(fmt_sub_type);
            CMVideoDimensions resolution = CMVideoFormatDescriptionGetDimensions([avFormat formatDescription]);

            if (pxt == pixelFormat &&
                resolution.width == w &&
                resolution.height == h) {
                for(avFrameRate in [avFormat videoSupportedFrameRateRanges]) {
                    Float64 maxFrameRate = [avFrameRate maxFrameRate];
                    if ( FPS_EQUAL(maxFrameRate,fps)) {
                        isCapabilitiesValid = true;
                        break;
                    }
                }
                break;
            }
        }

        if ( !isCapabilitiesValid ) {
            DEBUG_PRINT("Error: cannot create the capture session.\n");
            return -5;
        }

        /* Add the input */
        if([session canAddInput: input] == NO) {
            DEBUG_PRINT("Error: cannot add the device input to the session manager.\n");
            return -6;
        }

        [session addInput:input];

        /* Set the AVCaptureDeviceFormat and minFrameDuration */
        if(avFormat != nil) {
            NSError* err = nil;
            [currentDevice lockForConfiguration: &err];
            if(err) {
                DEBUG_PRINT("Error: cannot lock the capture device for configuration.\n");
                return -7;
            }
            [currentDevice setActiveFormat: avFormat];
//            [currentDevice setActiveVideoMinFrameDuration: [avFrameRate minFrameDuration]];
            [currentDevice unlockForConfiguration];

        }
        else {
            DEBUG_PRINT("Error: cannot find the capture format.\n");
            return  -8;
        }

        /* Create the output handler (needed for getOutputFormatormats) */
        if(output == nil) {
            output = [[AVCaptureVideoDataOutput alloc] init];
            if(!output) {
                DEBUG_PRINT("Error: cannot allocate the video data output\n");
                return -9;
            }
            [output retain];
        }

        /* Configure output */
        /* Tell the AVCaptureVideoDataOutput to convert the incoming data if necessary (and specify w/h) */
        [output setVideoSettings: [NSDictionary dictionaryWithObjectsAndKeys:
                                 [NSNumber numberWithInt:fmt_sub_type], kCVPixelBufferPixelFormatTypeKey,
                                 [NSNumber numberWithInteger:w], (id)kCVPixelBufferWidthKey,
                                 [NSNumber numberWithInteger:h], (id)kCVPixelBufferHeightKey,
                                 nil]];

      /*
         Cache the currently used pixel format that is used to fill the PixelBuffer
         that we pass to the callback. We retrieve the pixel format from the current
         device to make sure that we're getting the one which is actually used.
      */

        /* Setup framegrabber callback */
        dispatch_queue_t queue = dispatch_queue_create("Video Queue", DISPATCH_QUEUE_SERIAL);
        [output setSampleBufferDelegate:self queue:queue];

        if ([session canAddOutput:output] == NO) {
            DEBUG_PRINT("Error: cannot add the given output.\n");
            dispatch_release(queue);
            [session commitConfiguration];
            return -10;
        }

        dispatch_release(queue);
        [session addOutput:output];
    }

    [session commitConfiguration];

    //Start capture
    [session startRunning];
    return 1;
}

/* Capture callback. */
- (void) captureOutput: (AVCaptureOutput*) captureOutput
 didOutputSampleBuffer: (CMSampleBufferRef) sampleBuffer
        fromConnection: (AVCaptureConnection*) connection
{

  if (cb_frame == nullptr) {
    DEBUG_PRINT("Error: capturing frames but the `cb_frame` callback is not set, not supposed to happen. Stopping\n");
    ///TO STOP capturing
  }

  CMFormatDescriptionRef desc = CMSampleBufferGetFormatDescription(sampleBuffer);
  FourCharCode fcc = CMFormatDescriptionGetMediaSubType(desc);
  CMBlockBufferRef block_buffer = CMSampleBufferGetDataBuffer(sampleBuffer);
  CMPixelFormatType pix_fmt = CMFormatDescriptionGetMediaSubType(desc);
  /* ------------------------------------------------------------------------------------------ */
  /* Compressed Formats                                                                         */
  /* ------------------------------------------------------------------------------------------ */
  if (kCMVideoCodecType_JPEG_OpenDML == fcc) {

    if (NULL == block_buffer) {
      printf("Error: failed to get access to the block buffer for JPEG data.\n");
      return;
    }

    size_t len_at_offset = 0;
    size_t total_length = 0;
    char* data_pointer = NULL;
    bool is_contiguous = false;
    OSStatus status;

    status = CMBlockBufferGetDataPointer(block_buffer, 0, &len_at_offset, &total_length, &data_pointer);

    if (kCMBlockBufferNoErr != status) {
      printf("Error: failed to get a pointer to the data pointer.\n");
      return;
    }

    if (len_at_offset != total_length) {
      printf("Error: the length at the offset of the data pointer is not the same as the total frame size. We're not handling this situation yet.\n");
      return;
    }

    is_contiguous = CMBlockBufferIsRangeContiguous(block_buffer,  0, total_length);
    if (false == is_contiguous) {
      printf("Error: the received datablock is not contiguous which we expect it to be.\n");
      return;
    }

    if (0 == is_pixel_buffer_set) {
      CMFormatDescriptionRef desc = CMSampleBufferGetFormatDescription(sampleBuffer);
      CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions(desc);

      frame.width[0] = dims.width;
      frame.height[0] = dims.height;
      frame.nbytes = total_length;
      frame.pixel_format = webcam_capture::av_foundation_video_format_to_capture_format(pix_fmt);

      is_pixel_buffer_set = 1;
    }

    frame.plane[0] = (uint8_t*)data_pointer;

    cb_frame(frame);

    return;
  }

  /* ------------------------------------------------------------------------------------------ */
  /* Uncompressed Formats                                                                       */
  /* ------------------------------------------------------------------------------------------ */
  CVPixelBufferRef buffer = CMSampleBufferGetImageBuffer(sampleBuffer);
  if (NULL == buffer) {
    CMFormatDescriptionRef desc = CMSampleBufferGetFormatDescription(sampleBuffer);
    CMVideoDimensions dims = CMVideoFormatDescriptionGetDimensions(desc);
    CMMediaType media_type = CMFormatDescriptionGetMediaType(desc);
    FourCharCode fcc = CMFormatDescriptionGetMediaSubType(desc);
    char* fcc_ptr = (char*)&fcc;
    printf("Error: the returned CVImageBufferRef is NULL. Stopping, sampleBuffer is %p.\n", sampleBuffer);
    printf("Error: CMSampleBufferGetSampleSize() = %lu\n", CMSampleBufferGetTotalSampleSize(sampleBuffer));
    printf("Error: CMVideoDimensions, width: %d, height: %d\n", dims.width, dims.height);
    printf("Error: CMMediaType, is video: %c\n", media_type == kCMMediaType_Video ? 'y' : 'c');
    printf("Error: FourCharCode: %c%c%c%c\n", fcc_ptr[3], fcc_ptr[2], fcc_ptr[1], fcc_ptr[1]);
    return;
  }

  /* Fill the pixel_buffer member with some info that won't change per frame. */
  if (0 == is_pixel_buffer_set) {

    CMFormatDescriptionRef desc = CMSampleBufferGetFormatDescription(sampleBuffer);
    CMPixelFormatType pix_fmt = CMFormatDescriptionGetMediaSubType(desc);

    if (true == CVPixelBufferIsPlanar(buffer)) {
      size_t plane_count = CVPixelBufferGetPlaneCount(buffer);
      if (plane_count > 3) {
        printf("Error: we got a plane count bigger then 3, not supported yet. Stopping.\n");
        exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < plane_count; ++i) {
        frame.width[i] = CVPixelBufferGetWidthOfPlane(buffer, i);
        frame.height[i] = CVPixelBufferGetHeightOfPlane(buffer, i);
        frame.stride[i] = CVPixelBufferGetBytesPerRowOfPlane(buffer, i);
        frame.nbytes += frame.stride[i] * frame.height[i];

        printf("width: %lu, height: %lu, stride: %lu, nbytes: %lu, plane_count: %lu\n",
             frame.width[i],
             frame.height[i],
             frame.stride[i],
             frame.nbytes,
             plane_count);
      }
    }
    else {
      frame.width[0] = CVPixelBufferGetWidth(buffer);
      frame.height[0] = CVPixelBufferGetHeight(buffer);
      frame.stride[0] = CVPixelBufferGetBytesPerRow(buffer);
      frame.nbytes = frame.stride[0] * frame.height[0];
    }

    frame.pixel_format = webcam_capture::av_foundation_video_format_to_capture_format(pix_fmt);

    is_pixel_buffer_set = 1;
  }

  CVPixelBufferLockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);
  {
    if (kCVPixelFormatType_422YpCbCr8_yuvs == pix_fmt ||
        kCVPixelFormatType_422YpCbCr8 == pix_fmt ||
        kCVPixelFormatType_32BGRA == pix_fmt   /* kCVPixelFormatType_32BGRA */
    )
    {
      frame.plane[0] = (uint8_t*)CVPixelBufferGetBaseAddress(buffer);
    }
    else if (kCVPixelFormatType_420YpCbCr8BiPlanarFullRange == pix_fmt ||   /* kCVPixelFormatType_420YpCbCr8BiPlanarFullRange */
             kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange == pix_fmt) /* kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange */
    {
      frame.plane[0] = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 0);
      frame.plane[1] = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(buffer, 1);
    }
    else {
      printf("Error: unhandled or unknown pixel format for the received buffer: %d.\n", pixel_format);
      CVPixelBufferUnlockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);
      return;
    }

    cb_frame(frame);
  }

//  CVPixelBufferUnlockBaseAddress(buffer, kCVPixelBufferLock_ReadOnly);


//  /* --------------------- OLD CODE LEAVING THIS HERE FOR REFERENCE -------------------------- */
//#if 0
//  CMFormatDescriptionRef desc_ref = CMSampleBufferGetFormatDescription(sampleBuffer);

//  int pix_fmt = CMFormatDescriptionGetMediaSubType(desc_ref);
//  if (kCVPixelFormatType_422YpCbCr8 == pix_fmt) {
//    printf("> kCVPixelFormatType_422YpCbCr8 (CA_UYVY422).\n");
//  }
//  else if(kCVPixelFormatType_422YpCbCr8_yuvs == pix_fmt) {
//    printf("> kCVPixelFormatType_422YpCbCr8_yuvs (CA_YUYV422).\n");
//  }
//  else if (kCVPixelFormatType_32BGRA == pix_fmt) {
//    printf("> kCVPixelFormatType_32BGRA.\n");
//  }
//#endif

//#if 0
//  CVImageBufferRef img_ref = CMSampleBufferGetImageBuffer(sampleBuffer);
//  CVPixelBufferLockBaseAddress(img_ref, 0);
//  void* base_address = CVPixelBufferGetBaseAddress(img_ref);

//  // get number of bytes in the image.
//  size_t img_bytes_per_row = CVPixelBufferGetBytesPerRow(img_ref);
//  size_t img_height = CVPixelBufferGetHeight(img_ref);
//  size_t nbytes = img_bytes_per_row * img_height;

//  if(cb_frame) {
//    cb_frame((void*)base_address, nbytes, cb_user);
//  }

//  CVPixelBufferUnlockBaseAddress(img_ref, 0);
//#endif

//#if 0
//  // Some debug info.
//  CMFormatDescriptionRef format_desc_ref = CMSampleBufferGetFormatDescription(sampleBuffer);
//  FourCharCode active_video_type = CMFormatDescriptionGetMediaSubType(format_desc_ref);
//  int av_fmt = CMFormatDescriptionGetMediaSubType(format_desc_ref);
//  if(av_fmt == kCVPixelFormatType_422YpCbCr8) {
//    printf("> kCVPixelFormatType_422YpCbCr8 (CA_UYVY422).\n");
//  }
//  else if(av_fmt == kCVPixelFormatType_422YpCbCr8_yuvs) {
//    printf("> kCVPixelFormatType_422YpCbCr8_yuvs (CA_YUYV422).\n");
//  }
//  else if (av_fmt == kCVPixelFormatType_32BGRA) {
//    printf("> kCVPixelFormatType_32BGRA.\n");
//  }
//#endif

}

- (int) stopCapturing {
    if(output != nil) {
      [output setSampleBufferDelegate:nil queue:dispatch_get_main_queue()];
    }

    if(session != nil) {

      if([session isRunning] == YES) {
        [session stopRunning];
      }

      if(input != nil) {
        [session removeInput: input];
      }

      if(output != nil) {
        [session removeOutput: output];
      }

      [session release];
    }


    if(input != nil) {
      [input release];
    }

    if(output != nil) {
      [output release];
    }

    input = nil;
    output = nil;
    session = nil;
    pixel_format = 0;
    is_pixel_buffer_set = 0;

    [session stopRunning];
    return 1;
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

int webcam_capture_av_get_devices(std::vector<webcam_capture::AVFoundationDeviceInfo>& result) {
    return [AVFoundation_Implementation getDevices:result];
}

int webcam_capture_av_get_capabilities(void* cap, std::vector<webcam_capture::AVFoundationCapability>& result) {
    return [(id)cap getCapabilities:result];
}

int webcam_capture_av_start_capturing(void *cap, webcam_capture::PixelFormat pixelFormat,
                                      int width, int height, float fps,
                                      webcam_capture::FrameCallback cb) {
    return [(id)cap startCapturing:pixelFormat width:width height:height fps:fps frameCB:cb];
}

int webcam_capture_av_stop_capturing(void *cap) {
    return [(id)cap stopCapturing];
}

@end
