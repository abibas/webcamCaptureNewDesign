/**
    This source is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#include <atlbase.h>
#include "../utils.h"
#include "media_foundation_utils.h"
#include "media_foundation_callback.h"
#include "media_foundation_camera.h"

#include <mfidl.h>
#include <shlwapi.h>
#include <stdio.h>
#include <Mfapi.h>
#include <mferror.h>

namespace webcam_capture {

MediaFoundation_Callback::MediaFoundation_Callback(int width, int height, PixelFormat pixelFormat, FrameCallback &frameCallback, std::unique_ptr<MediaFoundation_DecompresserTransform> decompresser, std::unique_ptr<MediaFoundation_ColorConverterTransform> colorConverter) :
    referenceCount(1),
    sourceReader(nullptr),
    frameCallback(frameCallback),
    decompresser(std::move(decompresser)),
    colorConverter(std::move(colorConverter)),
    keepRunning(true),
    stoppedRunning(false)
{
    InitializeCriticalSection(&criticalSection);

    frame.width[0] = width;
    frame.height[0] = height;
    frame.pixelFormat = pixelFormat;
}

MediaFoundation_Callback::~MediaFoundation_Callback()
{
}

void MediaFoundation_Callback::setSourceReader(IMFSourceReader *sourceReader)
{
    this->sourceReader = sourceReader;
}

HRESULT MediaFoundation_Callback::QueryInterface(REFIID iid, void **v)
{
    static const QITAB qit[] = { QITABENT(MediaFoundation_Callback, IMFSourceReaderCallback), 0 };
    return QISearch(this, qit, iid, v);
}

ULONG MediaFoundation_Callback::AddRef()
{
    return InterlockedIncrement(&referenceCount);
}

ULONG MediaFoundation_Callback::Release()
{
    ULONG count = InterlockedDecrement(&referenceCount);

    if (count == 0) {
        delete this;
    }

    return count;
}

HRESULT MediaFoundation_Callback::OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp,
        IMFSample *sample)
{
    EnterCriticalSection(&criticalSection);

    if (SUCCEEDED(hr) && sample) {
        IMFSample *finalSample = sample;

        if (decompresser) {
            if (!decompresser->convert(sample, &finalSample)) {
                DEBUG_PRINT("Failed to decompress.");
            } else {
                sample = finalSample;
            }
        }

        if (colorConverter) {
            if (!colorConverter->convert(sample, &finalSample)) {
                DEBUG_PRINT("Failed to convert color.");
            }
        }

        DWORD count = 0;
        HRESULT hr = finalSample->GetBufferCount(&count);

        for (DWORD i = 0; i < count; ++i) {
            CComPtr<IMFMediaBuffer> buffer;

            hr = finalSample->GetBufferByIndex(i, &buffer);
            if (FAILED(hr)) {
                break;
            }

            DWORD length = 0;
            DWORD max_length = 0;
            BYTE *data = nullptr;
            buffer->Lock(&data, &max_length, &length);

            frame.bytes = static_cast<size_t>(length);
            frame.plane[0] = data;
            /* TODO(nurupo): figure out how to deal with planar images
            frame.plane[1] = data + frame.offset[1];
            frame.plane[2] = data + frame.offset[2];
            */
            frameCallback(frame);

            buffer->Unlock();
        }
    }

    if (SUCCEEDED(hr)) {
        if (keepRunning) {
            hr = sourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), 0, nullptr, nullptr, nullptr, nullptr);

            if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't request next sample to be read.", hr);
            }
        } else {
            stoppedRunning = true;
            stoppingCondition.notify_one();
        }
    }

    LeaveCriticalSection(&criticalSection);

    return S_OK;
}

HRESULT MediaFoundation_Callback::OnEvent(DWORD /*streamIndex*/, IMFMediaEvent * /*event*/)
{
    return S_OK;
}

HRESULT MediaFoundation_Callback::OnFlush(DWORD /*streamIndex*/)
{
    return S_OK;
}

HRESULT MediaFoundation_Callback::Wait(DWORD * /*streamFlags*/, LONGLONG * /*timestamp*/, IMFSample * /*sample*/)
{
    return S_OK;
}

HRESULT MediaFoundation_Callback::Cancel()
{
    return S_OK;
}

void MediaFoundation_Callback::stop()
{
    if (keepRunning) {
        std::unique_lock<std::mutex> lg(stoppingMutex);
        keepRunning = false;
        stoppingCondition.wait(lg, [this]{return stoppedRunning == true;});
    }
}

} // namespace webcam_capture
