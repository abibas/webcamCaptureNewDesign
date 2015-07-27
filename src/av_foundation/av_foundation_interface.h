/*

  C-Interface for the AVFoundation webcam grabber.

 */
#ifndef VIDEO_CAPTURE_AV_FOUNDATION_INTERFACE_H
#define VIDEO_CAPTURE_AV_FOUNDATION_INTERFACE_H

#include <vector>
#include <camera_information.h>
#include <capability.h>
#include "av_foundation_utils.h"

void* webcam_capture_av_alloc(std::string deviceId);       /* Allocate the AVFoundation implementation. */
void webcam_capture_av_dealloc(void* cap);                 /* Deallocate the AVFoundation implementation */
int webcam_capture_av_get_devices(std::vector<webcam_capture::AVFoundationDeviceInfo>& result); /* Get available devices. */
int webcam_capture_av_get_capabilities(void* cap, std::vector<webcam_capture::AVFoundationCapability>& result); /* Get available capability formats */

#endif
