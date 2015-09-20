/**
    This header is using code from the https://github.com/roxlu/video_capture
    distributed under the Apache 2.0 license
    http://www.apache.org/licenses/LICENSE-2.0
  */

#ifndef MEDIA_FOUNDATION_CALLBACK_H
#define MEDIA_FOUNDATION_CALLBACK_H

#include "media_foundation_color_converter_transform.h"
#include "media_foundation_decompresser_transform.h"

#include <camera_interface.h>

#include <frame.h>

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include <windows.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <shlwapi.h>

namespace webcam_capture {

class MediaFoundation_Camera;

class MediaFoundation_Callback : public IMFSourceReaderCallback
{
public:
    MediaFoundation_Callback(int width, int height, PixelFormat pixelFormat, FrameCallback &frameCallback, std::unique_ptr<MediaFoundation_DecompresserTransform> decompresser, std::unique_ptr<MediaFoundation_ColorConverterTransform> colorConverter);
    void setSourceReader(IMFSourceReader *sourceReader);

    STDMETHODIMP QueryInterface(REFIID iid, void **v);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    STDMETHODIMP OnReadSample(HRESULT hr, DWORD streamIndex, DWORD streamFlags, LONGLONG timestamp, IMFSample *sample);
    STDMETHODIMP OnEvent(DWORD streamIndex, IMFMediaEvent *event);
    STDMETHODIMP OnFlush(DWORD streamIndex);

    HRESULT Wait(DWORD *streamFlags, LONGLONG *timestamp, IMFSample *sample);

    HRESULT Cancel();

    void stop();

private:
    virtual ~MediaFoundation_Callback();

    IMFSourceReader *sourceReader;
    Frame frame;
    FrameCallback frameCallback;
    std::unique_ptr<MediaFoundation_DecompresserTransform> decompresser;
    std::unique_ptr<MediaFoundation_ColorConverterTransform> colorConverter;
    long referenceCount;
    CRITICAL_SECTION criticalSection;
    std::atomic<bool> keepRunning;
    std::atomic<bool> stoppedRunning;
    std::mutex stoppingMutex;
    std::condition_variable stoppingCondition;
};
} // namespace webcam_capture


#endif
