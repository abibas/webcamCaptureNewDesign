#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#include <backend_interface.h>
#include "av_foundation_utils.h"

@interface AVFoundation_Notifications_Implementation : NSObject<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    webcam_capture::AVFoundationConnectionStatusCallback cb_cameraConnected;
    webcam_capture::AVFoundationConnectionStatusCallback cb_cameraDisconnected;
    std::vector<webcam_capture::AVFoundationDeviceInfo> deviceList;
}

- (id) init;
- (void) dealloc;
- (int) startCameraNotifications: (webcam_capture::AVFoundationConnectionStatusCallback)cameraConectedCallBack discCallback: (webcam_capture::AVFoundationConnectionStatusCallback)cameraDisconnectedCallBack;
- (int) stopCameraNotifications;
- (void) cameraAdded:(NSNotification *)notification;
- (void) cameraRemoved:(NSNotification *)notification;

@end
