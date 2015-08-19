/* -*-C-*- */
#import "av_foundation_notifications_implementation.h"
#import "av_foundation_implementation.h"
#include "av_foundation_utils.h"

@implementation AVFoundation_Notifications_Implementation

// Initializes the capturer
- (id) init {

    self = [super init];

    if(self) {
        cb_cameraConnected = nil;
        cb_cameraDisconnected = nil;
    }

    return self;
}

//// Deallocate all retained objects
- (void) dealloc {

    if ( (!cb_cameraConnected) || (!cb_cameraDisconnected) ) {
        [self stopCameraNotifications];
        cb_cameraConnected = nil;
        cb_cameraDisconnected = nil;
    }

    [super dealloc];
}

- (int) startCameraNotifications: (webcam_capture::AVFoundationConnectionStatusCallback)cameraConectedCallBack discCallback : (webcam_capture::AVFoundationConnectionStatusCallback)cameraDisconnectedCallBack {
    cb_cameraConnected = cameraConectedCallBack;
    cb_cameraDisconnected = cameraDisconnectedCallBack;
    [AVFoundation_Implementation getDevices:&deviceList];

    [[NSNotificationCenter defaultCenter] addObserver:self
             selector:@selector(cameraAdded:)
                 name:AVCaptureDeviceWasConnectedNotification
               object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self
             selector:@selector(cameraRemoved:)
                 name:AVCaptureDeviceWasDisconnectedNotification
               object:nil];
    return 1;
}
- (int) stopCameraNotifications {
    deviceList.clear();
    [[NSNotificationCenter defaultCenter] removeObserver:self
                 name:AVCaptureDeviceWasConnectedNotification
               object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self
                 name:AVCaptureDeviceWasDisconnectedNotification
               object:nil];
    cb_cameraConnected = nil;
    cb_cameraDisconnected = nil;

    return 1;
}

- (void) cameraAdded:(NSNotification *)notification {
    if (!cb_cameraConnected) {
        return;
    }
    std::vector<webcam_capture::AVFoundationDeviceInfo> newDeviceList;
    [AVFoundation_Implementation getDevices:&newDeviceList];
    if (deviceList.size() == newDeviceList.size() ) {
        return;
    }

    for (int i = 0; i < newDeviceList.size(); i++) {
        for (int j = 0; j < deviceList.size(); j++) {
            if ( deviceList.at(j).deviceId == newDeviceList.at(i).deviceId ) {
                break;
            }
            if ( j == deviceList.size() -1 ) {  //the last element was checked
                std::string name = newDeviceList.at(i).deviceName;
                std::string id = newDeviceList.at(i).deviceId;
                deviceList.push_back(webcam_capture::AVFoundationDeviceInfo(name, id));
                cb_cameraConnected(name, id);
                return;
            }
        }
    }
}

- (void) cameraRemoved:(NSNotification *)notification {
    if (!cb_cameraDisconnected) {
        return;
    }
    std::vector<webcam_capture::AVFoundationDeviceInfo> newDeviceList;
    [AVFoundation_Implementation getDevices:&newDeviceList];
    if (deviceList.size() == newDeviceList.size() ) {
        return;
    }

    //to found which device was removed

    for (int i = 0; i < deviceList.size(); i++) {
        for (int j = 0; j < newDeviceList.size(); j++) {
            if (deviceList.at(i).deviceId == newDeviceList.at(j).deviceId) {
                break;
            }
            if ( j == newDeviceList.size() - 1) {
                std::string name = deviceList.at(i).deviceName;
                std::string id = deviceList.at(i).deviceId;
                cb_cameraDisconnected(name, id);
                deviceList = newDeviceList;
                return;
            }
        }
    }
}

/* C-interface */
/* -------------------------------------- */

void* webcam_capture_backend_alloc() {
    return (void*)[[AVFoundation_Notifications_Implementation alloc] init];
}

void webcam_capture_backend_dealloc(void* cap) {
    if(cap) {
        [(id)cap dealloc];
    }
}

int webcam_capture_av_start_camera_notifications(void* cap, webcam_capture::AVFoundationConnectionStatusCallback cb_cameraConnected,
                                                            webcam_capture::AVFoundationConnectionStatusCallback cb_cameraDisconnected) {
    return [(id)cap startCameraNotifications:cb_cameraConnected discCallback:cb_cameraDisconnected];
}

int webcam_capture_av_stop_camera_notifications(void* cap) {
    return [(id)cap stopCameraNotifications];
}

@end
