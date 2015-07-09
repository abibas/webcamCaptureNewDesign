#include "direct_show_callback.h"

namespace webcam_capture {

DirectShow_Callback::DirectShow_Callback(DirectShow_Camera *cam) :
    ds_camera(cam)
{
}

STDMETHODIMP DirectShow_Callback::SampleCB(double SampleTime, IMediaSample *pSample) {

    if (!pSample)
        return E_POINTER;
    long sz = pSample->GetActualDataLength();
    BYTE *pBuf = NULL;
    pSample->GetPointer(&pBuf);
    if (sz <= 0 || pBuf==NULL) return E_UNEXPECTED;
    for(int i=0;i<sz;i+=2)
        pBuf[i] = 255 - pBuf[i];
    return S_OK;

    // you can call functions like:
//        REFERENCE_TIME   tStart, tStop;
//        BYTE            *sampleBuffer;

//        pSample->GetTime(&tStart, &tStop);
//        pSample->GetPointer(&sampleBuffer);
//        pSample->GetActualDataLength();
}

} // namespace webcam_capture
