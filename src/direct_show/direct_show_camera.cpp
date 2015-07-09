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

    IGraphBuilder *pGraph = NULL;
    IMediaControl *pControl = NULL;
    IMediaEventEx *pEvent = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
            CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pGraph));
    if (FAILED(hr)) {
        return -123;
    }

    hr = pGraph->QueryInterface(IID_PPV_ARGS(&pControl));
    if (FAILED(hr)) {
        return -3123;
    }

    hr = pGraph->QueryInterface(IID_PPV_ARGS(&pEvent));
    if (FAILED(hr)) {
        return -324;
    }

    // Create SampleGrabber
    IBaseFilter *pGrabberF = NULL;
    ISampleGrabber *pGrabber = NULL;

    hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGrabberF));
    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create CoCreateInstance.\n");
        return -3;
    }

    hr = pGraph->AddFilter(pGrabberF, L"Sample Grabber");
    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot pGrabberF->QueryInterface.\n");
        return -4;
    }

    hr = pGrabberF->QueryInterface(IID_PPV_ARGS(&pGrabber));
    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot pGrabberF->QueryInterface.\n");
        return -5;
    }

    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(mt));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_YUY2;


    hr = pGrabber->SetMediaType(&mt);
    if (FAILED(hr))  {
       return -123;
    }

////////Run the Graph
    ds_callback = new DirectShow_Callback(this);
    hr = pGrabber->SetCallback(ds_callback, 0);
    if (FAILED(hr)) {
       return -123;
    }

    hr = pGrabber->SetBufferSamples(TRUE);
    if (FAILED(hr)) {
       return -123;
    }

    hr = pControl->Run();
    if (FAILED(hr)) {
       return -123;
    }

    long evCode;
    hr = pEvent->WaitForCompletion(INFINITE, &evCode);


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
} // namespace webcam_capture
