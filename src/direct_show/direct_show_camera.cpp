#include "../capability_tree_builder.h"
#include "direct_show_camera.h"
#include "../winapi_shared/winapi_shared_unique_id.h"

#include <iostream>
#include <dshow.h>


//Include Qedit.h. This header file is not compatible with Microsoft® Direct3D® headers later than version 7
//That's why we creating custom define
DEFINE_GUID(CLSID_SampleGrabber,
0xC1F400A0, 0x3F08, 0x11D3, 0x9F, 0x0B, 0x00, 0x60, 0x08, 0x03, 0x9E, 0x37); //qedit.dll


namespace webcam_capture {

DirectShow_Camera::DirectShow_Camera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
    : information(information)
    , mfDeinitializer(mfDeinitializer)
    , state(CA_STATE_NONE)
    , ds_callback(NULL)
{

}

CameraInterface *DirectShow_Camera::createCamera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
{
    return new DirectShow_Camera(mfDeinitializer, information);
}

DirectShow_Camera::~DirectShow_Camera()
{
    // Stop capturing
    if (state & CA_STATE_CAPTURING) {
        stop();
    }
}

int DirectShow_Camera::start(const CapabilityFormat &capabilityFormat,
                                  const CapabilityResolution &capabilityResolution,
                                  const CapabilityFps &capabilityFps,
                                  frame_callback cb)
{
    if (!cb) {
        DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.\n");
        return -1;      //TODO Err code
    }

    if (state & CA_STATE_CAPTURING) {
        DEBUG_PRINT("Error: cannot start capture because we are already capturing.\n");
        return -2;      //TODO Err code
    }

    cb_frame = cb;

    pixel_buffer.height[0] = 480;
    pixel_buffer.width[0] = 640;
    pixel_buffer.pixel_format = Format::YUY2;

    /// 1 step get IMoniker.
    IMoniker                *pVideoSel = getIMonikerByUniqueId(information.getUniqueId());
    /// 2 step Create the Capture Graph Builder.
    ICaptureGraphBuilder2   *pBuild;
    HRESULT hr;
    hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, (void**)&pBuild );
    if (FAILED(hr)) {
        return -99;
    }

    /// 3 step Create the Filter Graph Manager.
    IGraphBuilder           *pGraph;
    hr = CoCreateInstance(CLSID_FilterGraph, 0, CLSCTX_INPROC, IID_IGraphBuilder, (void**)&pGraph);
    if (FAILED(hr)) {
        return -99;
    }

    /// 4 step Initialize the Filter Graph  for the Capture Graph Builder to use.
    pBuild->SetFiltergraph(pGraph);

    /// 5 step Attach the graph control
    IMediaControl           *pControl;
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    if (FAILED(hr)) {
        return -99;
    }

    /// 6 step  Attach the graph events
    IMediaEvent             *pEvent;
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
    if (FAILED(hr)) {
        return -99;
    }

    /// 7 step Device binding with connection
    IBaseFilter             *pVCap;
    hr = pVideoSel->BindToObject(0, 0, IID_IBaseFilter, (void**)&pVCap);
    if (FAILED(hr)) {
        return -99;
    }
    /// 8 Add the Device Filter to the Graph
    hr = pGraph->AddFilter(pVCap, L"Video Capture");
    if (FAILED(hr)) {
        return -99;
    }
    /// 9 Creates Grabber Filter and adds it to the Filter Graph
    /// Once connected, Grabber Filter will capture still images
    IBaseFilter             *pGrabberFilter;
    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGrabberFilter));
    if (FAILED(hr)) {
        return -99;
    }

    /// 10 Create Sample grabber
    hr = pGraph->AddFilter(pGrabberFilter, L"Sample Grabber");
    ISampleGrabber          *pSampleGrabber;
    hr = pGrabberFilter->QueryInterface(IID_ISampleGrabber, (void**)&pSampleGrabber);
    if (FAILED(hr)) {
        return -99;
    }

    /// 11 set callback
    ISampleGrabberCB* pGrabberCB = new DirectShow_Callback(this);

    /// 12 set callback to the ISampleGrabber
    pSampleGrabber->SetCallback(pGrabberCB, 0);

    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(mt));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_YUY2;
    hr = pSampleGrabber->SetMediaType(&mt);
    if (FAILED(hr)) {
        return -99;
    }
    hr = pSampleGrabber->SetOneShot(FALSE);
    if (FAILED(hr)) {
        return -99;
    }
    hr = pSampleGrabber->SetBufferSamples(TRUE); // To comment and check
    if (FAILED(hr)) {
        return -99;
    }

//    hr = pSampleGrabber->GetConnectedMediaType(&mt);
//    if (FAILED(hr)) {
//        return -99;
//    }
//    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER *)mt.pbFormat;
//    int gChannels = pVih->bmiHeader.biBitCount / 8;
//    int gWidth = pVih->bmiHeader.biWidth;
//    int gHeight = pVih->bmiHeader.biHeight;

    hr = pBuild->RenderStream(&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video, pVCap, pGrabberFilter, NULL);

    pControl->Run();
/////////////////

    return 1;      //TODO Err code
}

int DirectShow_Camera::stop()
{
    return 1;   //TODO Err code
}

PixelBuffer *DirectShow_Camera::CaptureFrame()
{
    //TODO to realise method
    return NULL;
}

// ---- Capabilities ----
std::vector<CapabilityFormat> DirectShow_Camera::getCapabilities()
{
    std::vector<CapabilityFormat> result;
//    getVideoCapabilities(imf_media_source, result);

    return result;
}



bool DirectShow_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange *videoPropRange)
{
    return true;
}



int DirectShow_Camera::getProperty(VideoProperty property)
{
    return 1;
}



bool DirectShow_Camera::setProperty(const VideoProperty property, const int value)
{



    return true;
}


/******** SDK FUNCTIONS ******/
IMoniker* DirectShow_Camera::getIMonikerByUniqueId(std::shared_ptr<UniqueId> &uniqueId)
{
    IEnumMoniker *pEnum;
    ICreateDevEnum *pDevEnum;
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
                                  CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pDevEnum));

    if (FAILED(hr)) {
        DEBUG_PRINT("Error during CoCreateInstance.\n");
        return NULL;
    }

    // Create an enumerator for the category Video capture devices.
    hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pEnum, 0);
    if (hr == S_FALSE) {
        DEBUG_PRINT("There is no available devices.\n");
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
            size_t linkLen = wcslen(var.bstrVal);
            WCHAR *linkStr = new WCHAR[linkLen];
            wcsncpy(linkStr, var.bstrVal, linkLen);

            WinapiShared_UniqueId ud(linkStr, BackendImplementation::DirectShow);
            if (*uniqueId == ud) {
                pResult = pMoniker;
                VariantClear(&var);
                pPropBag->Release();
                continue;
            }
            VariantClear(&var);
        }

        pPropBag->Release();
        pMoniker->Release();
    }
    pEnum->Release();
    return pResult;
}

HRESULT DirectShow_Camera::ConnectFilters(ICaptureGraphBuilder2 *pBuild, IGraphBuilder *pGraph, IBaseFilter *pFirst, IBaseFilter *pSecond) {
    HRESULT hr = S_OK;
//    IPin *pOut = NULL;
//    IPin *pIn  = NULL;

//    pBuild->FindPin(pFirst,  PINDIR_OUTPUT, NULL, NULL, TRUE, 0, &pOut);
//    pBuild->FindPin(pSecond, PINDIR_INPUT, NULL, NULL, TRUE, 0, &pIn);

//    if(pOut && pIn) hr = pGraph->Connect(pOut, pIn);
//    if(pIn) pIn->Release();
//    if(pOut) pOut->Release();

    return hr;
}

} // namespace webcam_capture
