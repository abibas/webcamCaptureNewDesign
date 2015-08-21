/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#include "../utils.h"
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"
#include "media_foundation_camera.h"

#include <assert.h>
#include <mfidl.h>
#include <shlwapi.h>
#include <stdio.h>

////
#include <mferror.h>
/////
namespace webcam_capture {

bool MediaFoundation_Callback::createInstance(MediaFoundation_Camera *cam, MediaFoundation_Callback **cb)
{
    if (cb == NULL) {
        DEBUG_PRINT("Error: the given MediaFoundation_Capture is invalid; cant create an instance.");
        return false;
    }

    MediaFoundation_Callback *media_cb = new MediaFoundation_Callback(cam);

    if (!media_cb) {
        DEBUG_PRINT("Error: cannot allocate a MediaFoundation_Callback object - out of memory");
        return false;
    }

    *cb = media_cb;
    (*cb)->AddRef();

    MediaFoundation_Utils::safeRelease(&media_cb);
    return true;
}

MediaFoundation_Callback::MediaFoundation_Callback(MediaFoundation_Camera *cam) : ref_count(1), cam(cam)
{
    InitializeCriticalSection(&crit_sec);
}

MediaFoundation_Callback::~MediaFoundation_Callback()
{
}

HRESULT MediaFoundation_Callback::QueryInterface(REFIID iid, void **v)
{
    static const QITAB qit[] = { QITABENT(MediaFoundation_Callback, IMFSourceReaderCallback), 0 };
    return QISearch(this, qit, iid, v);
}

ULONG MediaFoundation_Callback::AddRef()
{
    return InterlockedIncrement(&ref_count);
}

ULONG MediaFoundation_Callback::Release()
{
    ULONG ucount = InterlockedDecrement(&ref_count);

    if (ucount == 0) {
        delete this;
    }

    return ucount;
}

HRESULT MediaFoundation_Callback::OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp,
        IMFSample *sample)
{
    assert(cam);
    assert(cam->imfSourceReader);
    assert(cam->frameCallback);

    EnterCriticalSection(&crit_sec);

    if (SUCCEEDED(hr) && sample) {

        IMFMediaBuffer *buffer;
        HRESULT hr = S_OK;
        DWORD count = 0;
        HRESULT decoderHR = S_OK;
        MFT_OUTPUT_DATA_BUFFER odf;
        odf.dwStreamID = 1;
        odf.pSample = sample;
        odf.dwStatus = 0;
        odf.pEvents = NULL;

        sample->GetBufferCount(&count);

        //if pDecoder exist's (!= NULL) - need to convert samples
        if (pDecoder) {
            //Using osi and isi to get information (used in debug)
            MFT_OUTPUT_STREAM_INFO osi;
            MFT_INPUT_STREAM_INFO isi;

            decoderHR = pDecoder->GetInputStreamInfo(0, &isi);
            if (FAILED(decoderHR)) {
                DEBUG_PRINT("Error: Can't get input stream info for IMFTransform obj");
            }
            decoderHR = pDecoder->GetOutputStreamInfo(0, &osi);
            if (FAILED(decoderHR)) {
                DEBUG_PRINT("Error: Can't get output stream info for IMFTransform obj");
            }

            ///TODO pay attantion to this remark:
            /// the idea is to call IMFTransform::ProcessInput() until it returns MF_E_NOTACCEPTING.
            /// Then you should call IMFTransform::ProcessOutput() until it returns MF_E_TRANSFORM_NEED_MORE_INPUT.
            /// And you keep that actions in a loop.
            decoderHR = pDecoder->ProcessInput(0, sample, 0);
            if (FAILED(decoderHR)) {
                DEBUG_PRINT("Error: Can't Process input for IMFTransform obj");
            }

            DWORD outStatus = 0;
            decoderHR = pDecoder->ProcessOutput(0,1, &odf, &outStatus);
            if (FAILED(decoderHR)) {
                DEBUG_PRINT("Error: Can't Process output for IMFTransform obj");
            }
        }

        for (DWORD i = 0; i < count; ++i) {
            if (FAILED(decoderHR)){
                break;
            }
            hr = odf.pSample->GetBufferByIndex(i, &buffer);

            if (SUCCEEDED(hr)) {
                DWORD length = 0;
                DWORD max_length = 0;
                BYTE *data = NULL;
                buffer->Lock(&data, &max_length, &length);

                cam->frame.nbytes = (size_t)length;
                cam->frame.plane[0] = data;
                cam->frame.plane[1] = data + cam->frame.offset[1];
                cam->frame.plane[2] = data + cam->frame.offset[2];
                cam->frameCallback(cam->frame);

                buffer->Unlock();
                buffer->Release();
            }
        }
    }

    if (SUCCEEDED(hr)) {
        if (cam->imfSourceReader && cam->capturing) {
            hr = cam->imfSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

            if (FAILED(hr)) {
                DEBUG_PRINT("Error: while trying to read the next sample.");
            }
        }
    }

    LeaveCriticalSection(&crit_sec);
    return S_OK;
}

HRESULT MediaFoundation_Callback::OnEvent(DWORD, IMFMediaEvent *event)
{
    return S_OK;
}

HRESULT MediaFoundation_Callback::OnFlush(DWORD)
{
    return S_OK;
}

} // namespace webcam_capture
