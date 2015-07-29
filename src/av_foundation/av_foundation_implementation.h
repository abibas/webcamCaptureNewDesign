/*

  AVFoundation_Implementation
  ----------------

  This class implements a video capturer for Mac using AVFoundation. The 
  AVFoundation works by creating a session manager to which we add an input
  device. The input device contains a list of capabilities. These capabilities
  are represented using width/height/fps/pixel_format etc.. The VideoCapture library
  allows you to select a specific video capture capability. 

  Though on Mac there is another layer we need to take of. Besides the input devices
  (the webcam) it has a class for output data. This output data class can convert the
  raw incoming frames into a different pixel format. For example, when the input data
  from the webcam are JPEG frames, then the output object (AVCaptureVideoDataOutput)
  can convert this into YUV422 for you. 

 */
#include <TargetConditionals.h>
#include <vector>
#include <camera_information.h>
#include <frame.h>
#include <camera_interface.h>
#include "av_foundation_utils.h"
#include <frame.h>
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

// resource: https://webrtc.googlecode.com/svn/trunk/webrtc/modules/video_capture/ios/video_capture_ios_objc.mm
// resource: https://developer.apple.com/library/ios/documentation/AudioVideo/Conceptual/AVFoundationPG/AVFoundationPG.pdf

@interface AVFoundation_Implementation : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
{
  AVCaptureSession* session;                                                                  /* Manages the state of the input device. */
  AVCaptureDeviceInput* input;                                                                /* Concrete instance of `AVDeviceInput`,  represents the input device (webcam). */
  AVCaptureVideoDataOutput* output;                                                           /* Concrete instance of `AVDeviceOutput`, used to get the video frames. */
  AVCaptureDevice* currentDevice;
  int pixel_format;                                                                           /* The pixel format in which we're capturing, is used in the callback to fill the PixelBuffer. This is a VideoCapture pixel format as defined in Types.h */
  bool is_frame_capabilities_set;                                                                    /* Some information of the `pixel_buffer` member can only be set in the frame callback, but we don't want to set it every time we get a new frame, this flag is used for that. */                                                                           /* User data that's will be passed into `cb_frame()` */
  webcam_capture::FrameCallback cb_frame;
  webcam_capture::Frame frame;
}

- (id) init: (std::string) deviceId;                                                          /* Initialize the AVImplementation object. */
- (void) dealloc;                                                                             /* Deallocate all objects that we retained. */
- (int) getDevices: (std::vector<webcam_capture::CameraInformation>&) result;                 /* Get a list with all the found capture devices. */
- (int) getCapabilities: (std::vector<webcam_capture::AVFoundationCapability>&) result;
- (int) startCapturing: (webcam_capture::PixelFormat&) pixelFormat width: (int) w
                    height: (int) h fps: (float) fps frameCB: (webcam_capture::FrameCallback) cb;
- (void) captureOutput: (AVCaptureOutput*) captureOutput didOutputSampleBuffer: (CMSampleBufferRef) sampleBuffer
      fromConnection: (AVCaptureConnection*) connection;
- (int) stopCapturing;
@end
