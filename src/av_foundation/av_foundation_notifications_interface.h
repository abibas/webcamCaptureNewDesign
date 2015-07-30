/*

  C-Interface for the AVFoundation camera connected/disconnected notifications.

 */
#ifndef VIDEO_CAPTURE_AV_FOUNDATION_NOTIFICATIONS_INTERFACE_H
#define VIDEO_CAPTURE_AV_FOUNDATION_NOTIFICATIONS_INTERFACE_H

#include <backend_interface.h>
#include "av_foundation_utils.h"

void* webcam_capture_backend_alloc();
void webcam_capture_backend_dealloc(void* cap);
int webcam_capture_av_start_camera_notifications(void* cap, webcam_capture::AVFoundationConnectionStatusCallback cb_cameraConnected,
                                                            webcam_capture::AVFoundationConnectionStatusCallback cb_cameraDisconnected);
int webcam_capture_av_stop_camera_notifications(void* cap);

#endif
