/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CAMERA_H
#define MEDIA_FOUNDATION_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00                                                         /* Default state */
#define CA_STATE_CAPTURING 0x01                                                   /* The user started captureing */

#include <windows.h>
#include <mfapi.h>
#include <mfplay.h>
#include <mfidl.h>                                                                   /* e.g. MFEnumDeviceSources */
#include <mfreadwrite.h>
#include <mferror.h>                                                                 /* MediaFoundation error codes, MF_E_*  */
#include <shlwapi.h>
#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include
#include <iostream>

#include "../utils.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "../include/capability.h"
#include "../include/pixel_buffer.h"
#include "../include/video_property.h"
#include "../include/video_property_range.h"
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"

namespace webcam_capture {

    typedef std::function<void(PixelBuffer& buffer)> frame_callback;

    class MediaFoundation_Camera : public CameraInterface {
    public:        
        ~MediaFoundation_Camera();
        static CameraInterface* createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

        int start(const CapabilityFormat &capabilityFormat, const CapabilityResolution &capabilityResolution, const CapabilityFps &capabilityFps, frame_callback cb);
        int stop();
        PixelBuffer* CaptureFrame();  //TODO
        // ---- Capabilities ----
        bool getPropertyRange(VideoProperty property, VideoPropertyRange * videoPropRange);
        int getProperty(VideoProperty property);
        bool setProperty(const VideoProperty property, const int value);
        std::vector<CapabilityFormat> getCapabilities();

    private:
        MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information, IMFMediaSource * mediaSource);

        //// SDK functions
        const int createSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader);
        static int createVideoDeviceSource(const int device, IMFMediaSource** source);
        const int getVideoCapabilities(IMFMediaSource* source, std::vector<CapabilityFormat> &capFormatVector);
        const int setDeviceFormat(IMFMediaSource* source, const int width, const int height, const Format pixelFormat, const int fps);
        const int setReaderFormat(IMFSourceReader* reader, const int width, const int height, const Format pixelFormat, const int fps);
        const int setDeviceFps(IMFMediaSource* source, int fps);

    public:
        std::shared_ptr<void> mfDeinitializer;
        PixelBuffer pixel_buffer;
        MediaFoundation_Callback* mf_callback;
        IMFMediaSource* imf_media_source;
        IMFSourceReader* imf_source_reader;
        int state;
        CameraInformation information;
        frame_callback cb_frame;
    };
  
} // namespace webcam_capture
#endif
