#include "media_foundation_camera.h"
#include <iostream>

namespace webcam_capture {

    MediaFoundation_Camera::MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information)
        :information(information)
        ,mfDeinitializer(mfDeinitializer)
        ,state(CA_STATE_NONE)
        ,mf_callback(NULL)
        ,imf_media_source(NULL)
        ,imf_source_reader(NULL)
    {
    }

    MediaFoundation_Camera::~MediaFoundation_Camera(){
        // Close and stop
        if(state & CA_STATE_CAPTURING) {
            stop();
        }

        if(state & CA_STATE_OPENED) {
            close();
        }
    }

    int MediaFoundation_Camera::open(){
        if(state & CA_STATE_OPENED) {
          DEBUG_PRINT("Error: already opened.\n");  //CAMERA_ALREADY_OPENED
          return -1; //TODO Err code
        }

        if(imf_media_source) {
            DEBUG_PRINT("Error: already opened the media source.\n");
            return -2;        //TODO Err code
        }

        // Create the MediaSource
        if(createVideoDeviceSource(information.getDeviceId(), &imf_media_source) < 0) {
            DEBUG_PRINT("Error: cannot create the media device source.\n");
            return -3;      //TODO Err code
        }

        state |= CA_STATE_OPENED;
        return 1;      //TODO Err code
    }

    int MediaFoundation_Camera::close(){

        if( !(state & CA_STATE_OPENED) ) {
          DEBUG_PRINT("Error:doesn't opened.\n");  //CAMERA_DOESNT_OPENED
          return -1; //TODO Err code
        }

        if(state & CA_STATE_CAPTURING) {
          stop();
        }

        safeReleaseMediaFoundation(&imf_media_source);

        state &= ~CA_STATE_OPENED;

        return 1;      //TODO Err code
    }

    int MediaFoundation_Camera::start(const Capability &capability, frame_callback cb){
        cb_frame = cb;

        if(!(state & CA_STATE_OPENED)) {
          DEBUG_PRINT("Error: cannot start captureing because you haven't opened the device successfully.\n");
          return -2;      //TODO Err code
        }

        if(state & CA_STATE_CAPTURING) {
          DEBUG_PRINT("Error: cannot start capture because we are already capturing.\n");
          return -3;      //TODO Err code
        }

        // Set the media format, width, height
        std::vector<Capability> capabilities;
        if(getVideoCapabilities(imf_media_source, capabilities) < 0) {
            DEBUG_PRINT("Error: cannot create the capabilities list to open the device.\n");
            safeReleaseMediaFoundation(&imf_media_source);
            return -4;      //TODO Err code
        }

        if(capability.getCapabilityIndex() >= capabilities.size()) {
            DEBUG_PRINT("Error: invalid capability ID, cannot open the device.\n");
            safeReleaseMediaFoundation(&imf_media_source);
            return -5;      //TODO Err code
        }

        //TODO we dont need this. (we already have the Capability in the input params
        Capability cap = capabilities.at(capability.getCapabilityIndex());
        if(cap.getPixelFormat() == Format::UNKNOWN) {
            DEBUG_PRINT("Error: cannot set a pixel format for UNKNOWN.\n");
            safeReleaseMediaFoundation(&imf_media_source);
            return -6;      //TODO Err code
        }

        if(setDeviceFormat(imf_media_source, (DWORD)cap.getPixelFormatIndex()) < 0) {
            DEBUG_PRINT("Error: cannot set the device format.\n");
            safeReleaseMediaFoundation(&imf_media_source);
            return -7;      //TODO Err code
        }

        // Create the source reader.
        MediaFoundation_Callback::createInstance(this, &mf_callback);
        if(createSourceReader(imf_media_source, mf_callback, &imf_source_reader) < 0) {
            DEBUG_PRINT("Error: cannot create the source reader.\n");
            safeReleaseMediaFoundation(&mf_callback);
            safeReleaseMediaFoundation(&imf_media_source);
            return -8;      //TODO Err code
        }

        // Set the source reader format.
        if(setReaderFormat(imf_source_reader, cap) < 0) {
            DEBUG_PRINT("Error: cannot set the reader format.\n");
            safeReleaseMediaFoundation(&mf_callback);
            safeReleaseMediaFoundation(&imf_media_source);
            return -9;      //TODO Err code
        }

        pixel_buffer.height[0] = cap.getHeight();
        pixel_buffer.width[0] = cap.getWidth();
        pixel_buffer.pixel_format = cap.getPixelFormat();


        // Kick off the capture stream.
        HRESULT hr = imf_source_reader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);
        if(FAILED(hr)) {
          if(hr == MF_E_INVALIDREQUEST) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDREQUEST.\n");
          }
          else if(hr == MF_E_INVALIDSTREAMNUMBER) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDSTREAMNUMBER.\n");
          }
          else if(hr == MF_E_NOTACCEPTING) {
            DEBUG_PRINT("ReadSample returned MF_E_NOTACCEPTING.\n");
          }
          else if(hr == E_INVALIDARG) {
            DEBUG_PRINT("ReadSample returned E_INVALIDARG.\n");
          }
          else if(hr == E_POINTER) {
            DEBUG_PRINT("ReadSample returned E_POINTER.\n");
          }
          else {
            DEBUG_PRINT("ReadSample - unhandled result.\n");
          }
          DEBUG_PRINT("Error: while trying to ReadSample() on the imf_source_reader. \n");
          #ifdef DEBUG_VERSION
            std::cout << "Error: " << std::hex << hr << std::endl;
          #endif
          return -4;      //TODO Err code
        }

        state |= CA_STATE_CAPTURING;

        return 1;      //TODO Err code
    }

    int MediaFoundation_Camera::stop(){
        if(!imf_source_reader) {
          DEBUG_PRINT("Error: Cannot stop capture because it seems that the device hasn't been opened yet.\n");
          return -1;    //TODO Err code
        }

        if(!state & CA_STATE_CAPTURING) {
          DEBUG_PRINT("Error: Cannot stop capture because we're not capturing yet.\n");
          return -2;    //TODO Err code
        }

        state &= ~CA_STATE_CAPTURING;

        safeReleaseMediaFoundation(&imf_source_reader);
        safeReleaseMediaFoundation(&mf_callback);

        return 1;   //TODO Err code
    }

    PixelBuffer* MediaFoundation_Camera::CaptureFrame(){
        //TODO to realise method
        return NULL;
    }

// ---- Capabilities ----
    std::vector<Capability> MediaFoundation_Camera::getCapabilities(){
        std::vector<Capability> result;

        if( !(state & CA_STATE_OPENED) ) {
          DEBUG_PRINT("Error:doesn't opened.\n");  //CAMERA_DOESNT_OPENED
          return result;
        }

        getVideoCapabilities(imf_media_source, result);

        return result;
    }



    VideoPropertyRange MediaFoundation_Camera::getPropertyRange(VideoProperty property){
        if( !(state & CA_STATE_OPENED) ) {
          DEBUG_PRINT("Error:doesn't opened.\n");  //CAMERA_DOESNT_OPENED
          VideoPropertyRange vpr(0,0,0,0);///TODO to return error value
          return vpr;///TODO to return error value
        }

        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        long lMin, lMax, lStep, lDefault, lCaps;

        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");
            VideoPropertyRange vpr(0,0,0,0);///TODO to return error value
            return vpr;///TODO to return error value
        }
        switch (property){
            case VideoProperty::Brightness : {
                ampProperty = VideoProcAmp_Brightness;
                break;
            }
            case VideoProperty::Contrast : {
                ampProperty = VideoProcAmp_Contrast;
                break;
            }
            case VideoProperty::Saturation : {
                ampProperty = VideoProcAmp_Saturation;
                break;
            }
            default: {
                DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
                VideoPropertyRange vpr(0,0,0,0);///TODO to return error value
                return vpr;///TODO to return error value
            }
        }

       hr = pProcAmp->GetRange(ampProperty, &lMin, &lMax, &lStep, &lDefault, &lCaps);
       if (FAILED(hr)){
           DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
           VideoPropertyRange vpr(0,0,0,0);///TODO to return error value
           return vpr;///TODO to return error value
       }

       VideoPropertyRange vpr(lMin, lMax, lStep, lDefault);
       return vpr;
    }



    int MediaFoundation_Camera::getProperty(VideoProperty property){
        if( !(state & CA_STATE_OPENED) ) {
          DEBUG_PRINT("Error:doesn't opened.\n");  //CAMERA_DOESNT_OPENED
          return -99999;///TODO to return error value
        }

        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.\n");
            return -99999;///TODO to return error value
        }

        switch (property){
            case VideoProperty::Brightness : {
                ampProperty = VideoProcAmp_Brightness;
                break;
            }
            case VideoProperty::Contrast : {
                ampProperty = VideoProcAmp_Contrast;
                break;
            }
            case VideoProperty::Saturation : {
                ampProperty = VideoProcAmp_Saturation;
                break;
            }
            default: {
                DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.\n");
                return -99999; ///TODO to return error value
            }
        }
        long value;
        long flags;
        hr = pProcAmp->Get(ampProperty, &value, &flags);
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");            
            return -99999;
        }        
        return value;
    }



    bool MediaFoundation_Camera::setProperty(const VideoProperty property, const int value){
        if( !(state & CA_STATE_OPENED) ) {
          DEBUG_PRINT("Error:doesn't opened.\n");  //CAMERA_DOESNT_OPENED
          return false;///TODO to return error value
        }

        IAMVideoProcAmp *pProcAmp = NULL;
        VideoProcAmpProperty ampProperty;
        HRESULT hr = imf_media_source->QueryInterface(IID_PPV_ARGS(&pProcAmp));
        if (FAILED(hr)){
            DEBUG_PRINT("Can't get IAMVideoProcAmp object. SetProperty failed.\n");            
            return false;
        }
        switch (property){
            case VideoProperty::Brightness : {
                ampProperty = VideoProcAmp_Brightness;
                break;
            }
            case VideoProperty::Contrast : {
                ampProperty = VideoProcAmp_Contrast;
                break;
            }
            case VideoProperty::Saturation : {
                ampProperty = VideoProcAmp_Saturation;
                break;
            }
            default: {
                return 0; ///TODO to return error value
            }
        }

        long val;
        long flags;
        hr = pProcAmp->Get(ampProperty, &val, &flags);
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.\n");            
            return false;
        }

        hr = pProcAmp->Set(ampProperty, value, flags);
        if (FAILED(hr)){
            DEBUG_PRINT("Error during IAMVideoProcAmp->Set. SetProperty failed.\n");            
            return false;
        }        
        return true;
    }


    /* PLATFORM SDK SPECIFIC */
    /* -------------------------------------- */

    int MediaFoundation_Camera::setDeviceFormat(IMFMediaSource* source, DWORD formatIndex) {

      IMFPresentationDescriptor* pres_desc = NULL;
      IMFStreamDescriptor* stream_desc = NULL;
      IMFMediaTypeHandler* handler = NULL;
      IMFMediaType* type = NULL;
      int result = 1; //TODO Err code

      HRESULT hr = source->CreatePresentationDescriptor(&pres_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("source->CreatePresentationDescriptor() failed.\n");
        result = -1;        //TODO Err code
        goto done;
      }

      BOOL selected;
      hr = pres_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("pres_desc->GetStreamDescriptorByIndex failed.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      hr = stream_desc->GetMediaTypeHandler(&handler);
      if(FAILED(hr)) {
        DEBUG_PRINT("stream_desc->GetMediaTypehandler() failed.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      hr = handler->GetMediaTypeByIndex(formatIndex, &type);
      if(FAILED(hr)) {
        DEBUG_PRINT("hander->GetMediaTypeByIndex failed.\n");
        result = -4;        //TODO Err code
        goto done;
      }

      hr = handler->SetCurrentMediaType(type);
      if(FAILED(hr)) {
        DEBUG_PRINT("handler->SetCurrentMediaType failed.\n");
        result = -5;        //TODO Err code
        goto done;
      }

    done:
      safeReleaseMediaFoundation(&pres_desc);
      safeReleaseMediaFoundation(&stream_desc);
      safeReleaseMediaFoundation(&handler);
      safeReleaseMediaFoundation(&type);
      return result;
    }

    int MediaFoundation_Camera::createSourceReader(IMFMediaSource* mediaSource,  IMFSourceReaderCallback* callback, IMFSourceReader** sourceReader) {

      if(mediaSource == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the IMFMediaSource passed into this function is not valid.\n");
        return -1;         //TODO Err code
      }

      if(callback == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the calls back passed into this function is not valid.\n");
        return -2;        //TODO Err code
      }

      HRESULT hr = S_OK;
      IMFAttributes* attrs = NULL;
      int result = 1;        //TODO Err code

      hr = MFCreateAttributes(&attrs, 2);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create attributes for the media source reader.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      hr = attrs->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: SetUnknown() failed on the source reader");
        result = -4;        //TODO Err code
        goto done;
      }

      /// This attribute gives such result - source reader does not shut down the media source.
      hr = attrs->SetUINT32(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, TRUE);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: SetUINT32() failed on the source reader");
        result = -5;        //TODO Err code
        goto done;
      }

      // Create a source reader which sets up the pipeline for us so we get access to the pixels
      hr = MFCreateSourceReaderFromMediaSource(mediaSource, attrs, sourceReader);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: while creating a source reader.\n");
        result = -6;        //TODO Err code
        goto done;
      }

    done:
      safeReleaseMediaFoundation(&attrs);
      return result;
    }

    int MediaFoundation_Camera::setReaderFormat(IMFSourceReader* reader, Capability& cap) {

      DWORD media_type_index = 0;
      int result = -1;        //TODO Err code
      HRESULT hr = S_OK;

      while(SUCCEEDED(hr)) {

        Format pixelFormat;
        int width;
        int height;
        int frameRate;
        int minFps;
        int maxFps;

        IMFMediaType* type = NULL;
        hr = reader->GetNativeMediaType(0, media_type_index, &type);

        if(SUCCEEDED(hr)) {

          // PIXELFORMAT
          PROPVARIANT var;
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_SUBTYPE, &var);
            if(SUCCEEDED(hr)) {
              pixelFormat = media_foundation_video_format_to_capture_format(*var.puuid);
            }
          }
          PropVariantClear(&var);

          // SIZE
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
            if(SUCCEEDED(hr)) {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              width = high;
              height = low;
            }
          }

          PropVariantClear(&var);


          //!!!! FOR DEBUG!!!!!!
          //get FPS
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_FRAME_RATE, &var);
            if(SUCCEEDED(hr)) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                frameRate = fps_from_rational(low, high);
            }
          }
          PropVariantClear(&var);
          ///// get Fps END

          //get min FPS
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MIN, &var);
            if(SUCCEEDED(hr)) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                minFps = fps_from_rational(low, high);
            }
          }
          PropVariantClear(&var);
          ///// get Fps END

          //get maxFPS
          PropVariantInit(&var);
          {
            hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var);
            if(SUCCEEDED(hr)) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                maxFps = fps_from_rational(low, high);
            }
          }
          PropVariantClear(&var);
          ///// get Fps END
          ///// !!!FOR DEBUG END

          // When the output media type of the source reader matches our specs, set it!
          if(width == cap.getWidth()
             && height == cap.getHeight()
             && pixelFormat == cap.getPixelFormat())
            {
              ///TODO !!! now it's workaround and set's only max FPS value
              hr = reader->SetCurrentMediaType(0, NULL, type);
              if(FAILED(hr)) {
                DEBUG_PRINT("Error: Failed to set the current media type for the given settings.\n");
              }
              else {
                hr = S_OK;
                result = 1;        //TODO Err code
              }
            }
          //type->Release();  // tmp moved down and wrapped around safeReleaseMediaFoundation()
        }
        else {
          break;
        }

        safeReleaseMediaFoundation(&type);

        ++media_type_index;
      }

      return result;
    }

    /**
     * Get capabilities for the given IMFMediaSource which represents
     * a video capture device.
     *
     * @param IMFMediaSource* source [in]               Pointer to the video capture source.
     * @param std::vector<AVCapability>& caps [out]     This will be filled with capabilites
     */
    int MediaFoundation_Camera::getVideoCapabilities(IMFMediaSource* source, std::vector<Capability>& caps) {

      IMFPresentationDescriptor* presentation_desc = NULL;
      IMFStreamDescriptor* stream_desc = NULL;
      IMFMediaTypeHandler* media_handler = NULL;
      IMFMediaType* type = NULL;
      int result = 1;        //TODO Err code

      HRESULT hr = source->CreatePresentationDescriptor(&presentation_desc);
      if (hr == MF_E_SHUTDOWN)
      {
         DEBUG_PRINT("Error: The media source's Shutdown method has been called.\n");
         goto done;
      }
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get presentation descriptor.\n");
        result = -1;        //TODO Err code
        goto done;
      }

      BOOL selected;
      hr = presentation_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get stream descriptor.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      hr = stream_desc->GetMediaTypeHandler(&media_handler);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type handler.\n");
        result = -3;        //TODO Err code
        goto done;
      }

      DWORD types_count = 0;
      hr = media_handler->GetMediaTypeCount(&types_count);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.\n");
        result = -4;        //TODO Err code
        goto done;
      }

  #if 0
      // The list of supported types is not garantueed to return everything :)
      // this was a test to check if some types that are supported by my test-webcam
      // were supported when I check them manually. (they didn't).
      // See the Remark here for more info: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970473(v=vs.85).aspx
      IMFMediaType* test_type = NULL;
      MFCreateMediaType(&test_type);
      if(test_type) {
        GUID types[] = { MFVideoFormat_UYVY,
                         MFVideoFormat_I420,
                         MFVideoFormat_IYUV,
                         MFVideoFormat_NV12,
                         MFVideoFormat_YUY2,
                         MFVideoFormat_Y42T,
                         MFVideoFormat_RGB24 } ;

        test_type->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        for(int i = 0; i < 7; ++i) {
          test_type->SetGUID(MF_MT_SUBTYPE, types[i]);
          hr = media_handler->IsMediaTypeSupported(test_type, NULL);
          if(hr != S_OK) {
            DEBUG_PRINT("> Not supported: %d\n" );
          }
          else {
            DEBUG_PRINT("> Yes, supported: %d\n", i);
          }
        }
      }
      safeReleaseMediaFoundation(&test_type);
  #endif

      // Loop over all the types
      PROPVARIANT var;
      for(DWORD i = 0; i < types_count; ++i) {

        Format pixelFormat;
        int pixelFormatIndex;
        int width;
        int height;
        int minFps;
        int maxFps;
        int currentFps;
        int currentFpsIndex;
        int capabilityIndex;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot get media type by index.\n");
          result = -5;        //TODO Err code
          goto done;
        }

        UINT32 attr_count = 0;
        hr = type->GetCount(&attr_count);
        if(FAILED(hr)) {
          DEBUG_PRINT("Error: cannot type param count.\n");
          result = -6;        //TODO Err code
          goto done;
        }

        if(attr_count > 0) {
          for(UINT32 j = 0; j < attr_count; ++j) {

            GUID guid = { 0 };
            PropVariantInit(&var);

            hr = type->GetItemByIndex(j, &guid, &var);
            if(FAILED(hr)) {
              DEBUG_PRINT("Error: cannot get item by index.\n");
              result = -7;        //TODO Err code
              goto done;
            }

            if(guid == MF_MT_SUBTYPE && var.vt == VT_CLSID) {
              pixelFormat = media_foundation_video_format_to_capture_format(*var.puuid);
              pixelFormatIndex = j;
            }
            else if(guid == MF_MT_FRAME_SIZE) {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              width = (int)high;
              height = (int)low;
            }
            else if( guid == MF_MT_FRAME_RATE_RANGE_MIN ) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                minFps = fps_from_rational(low, high);
            }
            else if ( guid == MF_MT_FRAME_RATE_RANGE_MAX ) {
                UINT32 high = 0;
                UINT32 low =  0;
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                maxFps = fps_from_rational(low, high);
            }
            else if ( guid == MF_MT_FRAME_RATE )
            {
              UINT32 high = 0;
              UINT32 low =  0;
              Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
              currentFps = fps_from_rational(low, high);
              currentFps = j;
            }

            PropVariantClear(&var);
          }

          capabilityIndex = i;
          //TODO to get ability to set variable FPS rate
          Capability cap (width, height, pixelFormat,minFps, maxFps, currentFps, capabilityIndex, currentFpsIndex, pixelFormatIndex, "");
          caps.push_back(cap);
        }

        safeReleaseMediaFoundation(&type);
      }

    done:
      safeReleaseMediaFoundation(&presentation_desc);
      safeReleaseMediaFoundation(&stream_desc);
      safeReleaseMediaFoundation(&media_handler);
      safeReleaseMediaFoundation(&type);
      PropVariantClear(&var);
      return result;
    }

    /**
     * Create and active the given `device`.
     *
     * @param int device [in]            The device index for which you want to get an
     *                                   activated IMFMediaSource object. This function
     *                                   allocates this object and increases the reference
     *                                   count. When you're ready with this object, make sure
     *                                   to call `safeReleaseMediaFoundation(&source)`
     *
     * @param IMFMediaSource** [out]     We allocate and activate the device for the
     *                                   given `device` parameter. When ready, call
     *                                   `safeReleaseMediaFoundation(&source)` to free memory.
     */
    int MediaFoundation_Camera::createVideoDeviceSource(const int device, IMFMediaSource** source) {

      int result = 1;  //TODO Err code
      IMFAttributes* config = NULL;
      IMFActivate** devices = NULL;
      UINT32 count = 0;

      HRESULT hr = MFCreateAttributes(&config, 1);
      if(FAILED(hr)) {
        result = -1;        //TODO Err code
        goto done;
      }

      // Filter on capture devices
      hr = config->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                           MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the GUID on the IMFAttributes*.\n");
        result = -2;        //TODO Err code
        goto done;
      }

      // Enumerate devices.
      hr = MFEnumDeviceSources(config, &devices, &count);
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get EnumDeviceSources.\n");
        result = -3;        //TODO Err code
        goto done;
      }
      if(count == 0 || device > count) { //TODO bug (>= count)becouse devices numerates from 0.
        result = -4;        //TODO Err code
        goto done;
      }

      // Make sure the given source is free/released.
      safeReleaseMediaFoundation(source);

      // Activate the capture device.
      hr = devices[device]->ActivateObject(IID_PPV_ARGS(source));
      if(FAILED(hr)) {
        DEBUG_PRINT("Error: cannot activate the object.");
        result = -5;        //TODO Err code
        goto done;
      }

      result = true;

    done:

      safeReleaseMediaFoundation(&config);
      for(DWORD i = 0; i < count; ++i) {
        safeReleaseMediaFoundation(&devices[i]);
      }
      CoTaskMemFree(devices);

      return result;
    }

} // namespace webcam_capture
