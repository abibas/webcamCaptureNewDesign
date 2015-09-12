#include "direct_show_camera.h"
#include "../capability_tree_builder.h"

#include "../winapi_shared/winapi_shared_unique_id.h"
#include "direct_show_utils.h"
#include "../capability_tree_builder.h"

#include <dshow.h>
#include <Dvdmedia.h>

//Include Qedit.h. This header file is not compatible with Microsoft® Direct3D® headers later than version 7
//That's why we creating custom define
DEFINE_GUID(CLSID_SampleGrabber,
0xC1F400A0, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll

DEFINE_GUID(CLSID_NullRenderer,
0xC1F400A4, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll

namespace webcam_capture {

DirectShow_Camera::DirectShow_Camera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
    : information(information)
    , mfDeinitializer(mfDeinitializer)
    , state(CA_STATE_NONE)
    , ds_callback(NULL)
{

}

std::unique_ptr<CameraInterface> DirectShow_Camera::createCamera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
{
    // TODO(nurupo): get std::make_unique to work without makeing code uglier
    return std::unique_ptr<DirectShow_Camera>(new DirectShow_Camera(mfDeinitializer, information));
}

DirectShow_Camera::~DirectShow_Camera()
{
    // Stop capturing
    if (state & CA_STATE_CAPTURING) {
        stop();
    }
}

int DirectShow_Camera::start(PixelFormat pixelFormat,
                              int width,
                              int height, float fps,
                              FrameCallback cb,
                              PixelFormat decodeFormat,
                              PixelFormat decompressFormat)
{
    if (!cb) {
        DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.");
        return -1;      //TODO Err code
    }

    if (state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: cannot start capture because we are already capturing.");
        return -2;      //TODO Err code
    }

    cb_frame = cb;


    frame.width[0] = width;
    frame.height[0] = height;
    frame.pixel_format = pixelFormat;


    HRESULT hr;
    /// 1 step get IMoniker.
    pVideoSel = getIMonikerByUniqueId(information.getUniqueId());
    if (!pVideoSel) {
        return -12;
    }

    /// 2 step Device binding with connection
    hr = pVideoSel->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
    if (FAILED(hr)) {
        pVideoSel->Release();
        return -99;
    }

    /// 3 step Create the Capture Graph Builder.
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild );
    if (FAILED(hr)) {
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    /// 4 step Set capabilities
    int setCapRes = setCapabilities(pVCap, pixelFormat, width, height, fps);
    if ( setCapRes < 0 ) {
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return setCapRes;
    }

    /// 5 step Create the Filter Graph Manager.
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) {
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    /// 6 step Initialize the Filter Graph  for the Capture Graph Builder to use.
    pBuild->SetFiltergraph(pGraph);           

    /// 7 step Attach the graph control
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    if (FAILED(hr)) {
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }    

    /// 8 step  Attach the graph events
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    if (FAILED(hr)) {
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    /// 9 Add the Device Filter to the Graph
    hr = pGraph->AddFilter(pVCap, L"Video Capture");
    if (FAILED(hr)) {
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }
    /// 10 Creates Grabber Filter and adds it to the Filter Graph
    /// Once connected, Grabber Filter will capture still images
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGrabberFilter));
    if (FAILED(hr)) {
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    /// 11 Create Sample grabber
    hr = pGraph->AddFilter(pGrabberFilter, L"Sample Grabber");
    if (FAILED(hr)) {
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    hr = pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
    if (FAILED(hr)) {
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    /// 12 set callback
    pGrabberCB = new DirectShow_Callback(this);

    /// 13 set callback to the ISampleGrabber
    pSampleGrabber->SetCallback(pGrabberCB, 0);

    hr = pSampleGrabber->SetOneShot(FALSE);
    if (FAILED(hr)) {
        pGrabberCB->Release();
        pSampleGrabber->Release();
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }
    hr = pSampleGrabber->SetBufferSamples(TRUE); // To comment and check
    if (FAILED(hr)) {
        pGrabberCB->Release();
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    ///create NULL renderer - to disable ActiveMovie Window
    hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&pNullRenderer);
    if (FAILED(hr)) {
        pGrabberCB->Release();
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }
    hr = pGraph->AddFilter(pNullRenderer, L"NullRender");
    if (FAILED(hr)) {
        pNullRenderer->Release();
        pGrabberCB->Release();
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pVCap, pGrabberFilter, pNullRenderer);
    if (FAILED(hr)) {
        pNullRenderer->Release();
        pGrabberCB->Release();
        pGrabberFilter->Release();
        pEvent->Release();
        pControl->Release();
        pGraph->Release();
        pBuild->Release();
        pVCap->Release();
        pVideoSel->Release();
        return -99;
    }

    pControl->Run();    

    state |= CA_STATE_CAPTURING;
    return 1;      //TODO Err code
}

int DirectShow_Camera::stop()
{        
    if (!state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: Cannot stop capture because we're not capturing yet.");
        return -1;    //TODO Err code
    }

    state &= ~CA_STATE_CAPTURING;

    pControl->Stop();

    pNullRenderer->Release();
    pGrabberCB->Release();
    pGrabberFilter->Release();
    pEvent->Release();
    pControl->Release();
    pGraph->Release();
    pBuild->Release();
    pVCap->Release();
    pVideoSel->Release();

    return 1;   //TODO Err code
}

std::unique_ptr<Frame> DirectShow_Camera::captureFrame()
{
    //TODO to realise method
    return nullptr;
}

bool DirectShow_Camera::enumerateCapabilities(IBaseFilter *videoCaptureFilter, EnumEntryCallback enumEntryCallback)
{
    // destroy videoCaptureFilter only if it set (i.e. owned) by us
    CComPtr<IBaseFilter> pVideoCaptureFilter;

    if (!videoCaptureFilter) {
        CComPtr<IMoniker> pMoniker = getIMonikerByUniqueId(information.getUniqueId());
        if (!pMoniker) {
            return false;
        }

        HRESULT hr = pMoniker->BindToObject(0, 0, IID_PPV_ARGS(&pVideoCaptureFilter));
        if (FAILED(hr)) {
            return false;
        }
        videoCaptureFilter = pVideoCaptureFilter.p;
    }

    CComPtr<IEnumPins> enumPins;
    HRESULT hr = videoCaptureFilter->EnumPins(&enumPins);
    if (FAILED(hr)) {
        return false;
    }

    // enumerate pins one by one
    unsigned long pinCount = 0;

    while (true) {
        enumPins->Reset();
        hr = enumPins->Skip(pinCount);
        if (FAILED(hr)) {
            //TODO(nurupo): print error
            return false;
        } else if (hr == S_FALSE) {
            return false;
        }
        CComPtr<IPin> pin;
        hr = enumPins->Next(1, &pin, nullptr);
        if (FAILED(hr)) {
            //TODO(nurupo): print error
            return false;
        } else if (hr == S_FALSE) {
            return false;
        }
        pinCount ++;
        CComPtr<IAMStreamConfig> streamConfig;
        hr = pin->QueryInterface(IID_PPV_ARGS(&streamConfig));
        if (FAILED(hr)) {
            continue;
        }

        int capCount = 0;
        int configSize = 0;

         // get the number of different resolutions possible
         hr = streamConfig->GetNumberOfCapabilities(&capCount, &configSize);
         if (FAILED(hr)) {
             continue;
         }

         std::unique_ptr<BYTE> config(new BYTE[configSize]);

         for (int cap = 0; cap < capCount; cap++) {
             AM_MEDIA_TYPE *mediaType;

             hr = streamConfig->GetStreamCaps(cap, &mediaType, config.get());

             bool done = false;

             if (SUCCEEDED(hr)) {
 #define CALL_HELPER(VIH) enumerateCapabilitiesHelper(videoCaptureFilter, pin, streamConfig, cap, mediaType, reinterpret_cast<VIDEO_STREAM_CONFIG_CAPS*>(config.get()), reinterpret_cast<VIH*>(mediaType->pbFormat), enumEntryCallback);
                 if (mediaType->formattype == FORMAT_VideoInfo) {
                     done = CALL_HELPER(VIDEOINFOHEADER);
                 } else if (mediaType->formattype == FORMAT_VideoInfo2) {
                     done = CALL_HELPER(VIDEOINFOHEADER2);
                 } else {
                     //TODO(nurupo): improve error reporting
                     //DEBUG_PRINT("Unknown format type" << mediaType->formattype);
                 }
#undef CALL_HELPER
             }

             _DeleteMediaType(mediaType);

             if (done) {
                 goto end;
             }
         }
    }
end:
    return true;
}

// ---- Capabilities ----
std::vector<CapabilityFormat> DirectShow_Camera::getCapabilities()
{
    CapabilityTreeBuilder capabilityBuilder;

    enumerateCapabilities(nullptr, [&capabilityBuilder](IAMStreamConfig*, AM_MEDIA_TYPE*, PixelFormat pixelFormat, int width, int height, std::vector<float> &fps)
    {
        capabilityBuilder.addCapability(pixelFormat, width, height, fps);

        return false;
    });

    return capabilityBuilder.build();
}


bool DirectShow_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    long lMin, lMax, lStep, lDefault, lCaps;

    /// get IMoniker.
    IMoniker *pMoniker = getIMonikerByUniqueId(information.getUniqueId());
    if (!pMoniker) {
        return false;
    }

    /// Device binding with connection
    IBaseFilter *pBaseFilter;
    HRESULT hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pBaseFilter);
    if (FAILED(hr)) {
        pMoniker->Release();
        return false;
    }

    hr = pBaseFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.");
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }

    switch (property) {
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
            DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.");
            pProcAmp->Release();
            pBaseFilter->Release();
            pMoniker->Release();
            return false;
        }
    }

    hr = pProcAmp->GetRange(ampProperty, &lMin, &lMax, &lStep, &lDefault, &lCaps);

    if (FAILED(hr)) {
        DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.");
        pProcAmp->Release();
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }

    videoPropRange = VideoPropertyRange(lMin, lMax, lStep, lDefault);

    pProcAmp->Release();
    pBaseFilter->Release();
    pMoniker->Release();
    return true;
}



int DirectShow_Camera::getProperty(VideoProperty property)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    long value, flags;

    /// get IMoniker.
    IMoniker *pMoniker = getIMonikerByUniqueId(information.getUniqueId());
    if (!pMoniker) {
        return -1111;
    }

    /// Device binding with connection
    IBaseFilter *pBaseFilter;
    HRESULT hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pBaseFilter);
    if (FAILED(hr)) {
        pMoniker->Release();
        return -111;
    }

    hr = pBaseFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.");
        pBaseFilter->Release();
        pMoniker->Release();
        return -111;
    }

    switch (property) {
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
            DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.");
            pProcAmp->Release();
            pBaseFilter->Release();
            pMoniker->Release();
            return -111;
        }
    }


    hr = pProcAmp->Get(ampProperty, &value, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.");
        pProcAmp->Release();
        pBaseFilter->Release();
        pMoniker->Release();
        return -99999;
    }

    pProcAmp->Release();
    pBaseFilter->Release();
    pMoniker->Release();
    return value;
}



bool DirectShow_Camera::setProperty(const VideoProperty property, const int value)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    long val, flags;

    /// get IMoniker.
    IMoniker *pMoniker = getIMonikerByUniqueId(information.getUniqueId());
    if (!pMoniker) {
        return false;
    }
    /// Device binding with connection
    IBaseFilter *pBaseFilter;
    HRESULT hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pBaseFilter);
    if (FAILED(hr)) {
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }

    hr = pBaseFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);
    if (FAILED(hr)) {
        DEBUG_PRINT("Can't get IAMVideoProcAmp object. GetPropertyRange failed.");
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }

    switch (property) {
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
            DEBUG_PRINT("Unsupported VideoPropertyValue. GetPropertyRange failed.");
            pProcAmp->Release();
            pBaseFilter->Release();
            pMoniker->Release();
            return false;
        }
    }


    hr = pProcAmp->Get(ampProperty, &val, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Get. SetProperty failed.");
        pProcAmp->Release();
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }

    hr = pProcAmp->Set(ampProperty, value, flags);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during IAMVideoProcAmp->Set. SetProperty failed.");
        pProcAmp->Release();
        pBaseFilter->Release();
        pMoniker->Release();
        return false;
    }
    pProcAmp->Release();
    pBaseFilter->Release();
    pMoniker->Release();
    return true;
}


/******** SDK FUNCTIONS ******/
IMoniker* DirectShow_Camera::getIMonikerByUniqueId(const std::shared_ptr<UniqueId> &uniqueId)
{
    IEnumMoniker *pEnum;
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during CoCreateInstance.");
        return NULL;
    }

    // Create an enumerator for the category Video capture devices.
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_FALSE) {
        DEBUG_PRINT("There is no available devices.");
        hr = VFW_E_NOT_FOUND;  // The category is empty. Treat as an error.
    }
    pDevEnum->Release();

    IMoniker *pResult = NULL;
    IMoniker *pMoniker = NULL;
    while (pEnum->Next(1, &pMoniker, NULL) == S_OK) {
        IPropertyBag *pPropBag;
        HRESULT hr = pMoniker->BindToStorage(0, 0, IID_PPV_ARGS(&pPropBag));
        if (FAILED(hr)) {
            pMoniker->Release();
            continue;
        }
        VARIANT var;
        VariantInit(&var);

        hr = pPropBag->Read(L"DevicePath", &var, 0);
        if (SUCCEEDED(hr)) {
            WinapiShared_UniqueId ud(var.bstrVal, BackendImplementation::DirectShow);
            if (*uniqueId == ud) {
                pResult = pMoniker;
                VariantClear(&var);
                pPropBag->Release();
                break;
            }
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
    pEnum->Release();
    return pResult;
}

int DirectShow_Camera::setCapabilities(IBaseFilter *videoCaptureFilter, PixelFormat pixelFormat, int width, int height, float fps)
{
    int result = -1;

    enumerateCapabilities(videoCaptureFilter, [&result, &pixelFormat, &width, &height, &fps](IAMStreamConfig *streamConfig, AM_MEDIA_TYPE* mediaType, PixelFormat pixelFormatEnum, int widthEnum, int heightEnum, std::vector<float> &fpsEnum)
    {
        if (pixelFormat != pixelFormatEnum || width != widthEnum || height != heightEnum) {
            return false;
        }

        bool foundFps = false;

        for (auto &&f : fpsEnum) {
            if (FPS_EQUAL(fps, f)) {
                if (mediaType->formattype == FORMAT_VideoInfo) {
                    reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat)->AvgTimePerFrame = FPS_FROM_RATIONAL(10000000, f);
                } else if (mediaType->formattype == FORMAT_VideoInfo2) {
                    reinterpret_cast<VIDEOINFOHEADER*>(mediaType->pbFormat)->AvgTimePerFrame = FPS_FROM_RATIONAL(10000000, f);
                }
                foundFps = true;
                break;
            }
        }

        if (foundFps) {
            streamConfig->SetFormat(mediaType);
            result = 1;
            return true;
        }

        return false;
    });

    return result;
}

} // namespace webcam_capture
