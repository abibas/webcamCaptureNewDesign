/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CAMERA_H
#define MEDIA_FOUNDATION_CAMERA_H

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
        MediaFoundation_Camera(std::shared_ptr<void*> mfDeinitializer, const CameraInformation &information);
        ~MediaFoundation_Camera();
        int open(const Capability &capability, frame_callback cb);
        int close();
        int start();
        int stop();
        PixelBuffer* CaptureFrame();  //TODO
        // ---- Capabilities ----
        VideoPropertyRange getPropertyRange(VideoProperty property) const; // TODO
        int getProperty(VideoProperty property) const; //TODO
        bool setProperty(const VideoProperty property, const int value); //TODO
        std::vector<Capability> getCapabilities() const;
    private:
        //// SDK functions
        int createSourceReader(IMFMediaSource* mediaSource, IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader);
        int createVideoDeviceSource(int device, IMFMediaSource** source);
        int getCapabilities(IMFMediaSource* source, std::vector<Capability>& result);
        int setDeviceFormat(IMFMediaSource* source, DWORD formatIndex);
        int setReaderFormat(IMFSourceReader* reader, Capability& cap);

    public:
        std::shared_ptr<void*> mfDeinitializer;
        PixelBuffer pixel_buffer;
        MediaFoundation_Callback* mf_callback;
        IMFMediaSource* imf_media_source;
        IMFSourceReader* imf_source_reader;
        int state;
        CameraInformation information;
        frame_callback cb_frame;
    };
  
  /* Safely release the given obj. */
  template <class T> void safeReleaseMediaFoundation(T **t) {
    if(*t) {
      (*t)->Release();
      *t = NULL;
    }
  }

} // namespace webcam_capture
#endif
