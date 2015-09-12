#ifndef DIRECT_SHOW_CAMERA_H
#define DIRECT_SHOW_CAMERA_H

/* States (may be be used by implementations) */
#define CA_STATE_NONE 0x00          /* Default state */
#define CA_STATE_CAPTURING 0x01     /* The user started captureing */

#include <atlbase.h>
#include <string>
#include <vector>
#include <memory>  //std::shared_ptr include

#include "../utils.h"
#include "../include/camera_interface.h"
#include "../include/camera_information.h"
#include "../include/capability.h"
#include "../include/frame.h"
#include "../include/video_property.h"
#include "../include/video_property_range.h"
//#include "media_foundation_utils.h"
#include "direct_show_callback.h"


namespace webcam_capture {

class DirectShow_Camera : public CameraInterface
{
public:
    ~DirectShow_Camera();
    static std::unique_ptr<CameraInterface> createCamera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    int start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb, PixelFormat decodeFormat = PixelFormat::UNKNOWN, PixelFormat decompressFormat = PixelFormat::UNKNOWN);
    int stop();
    std::unique_ptr<Frame> captureFrame();
    // ---- Capabilities ----
    bool getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange);
    int getProperty(VideoProperty property);
    bool setProperty(const VideoProperty property, const int value);
    std::vector<CapabilityFormat> getCapabilities();

private:
    DirectShow_Camera(std::shared_ptr<void> mfDeinitializer, const CameraInformation &information);

    /***** SDK FUNCTIONS *****/
    IMoniker* getIMonikerByUniqueId(const std::shared_ptr<UniqueId> &uniqueId);
    int setCapabilities(IBaseFilter *videoCaptureFilter, PixelFormat pixelFormat, int width, int height, float fps);
    /**
     * Return true to stop enumerating further, otherwise return false.
     */
    typedef std::function<bool(IAMStreamConfig*, AM_MEDIA_TYPE*, PixelFormat, int, int, std::vector<float>&)> EnumEntryCallback;

    //TODO(nurupo): create IBaseFilter *videoCaptureFilter in ctor
    /**
     * Enumerates capabilities of camera and calls the provided callback for each capability set.
     * @param videoCaptureFilter Set it to the existing Video Captire Filter. Can be null.
     * @param enumEntryCallback Callback that will be called for each capability set found.
     * @return true if no errors occured, false otherwise.
     */
    bool enumerateCapabilities(IBaseFilter *videoCaptureFilter, EnumEntryCallback enumEntryCallback);

    template<typename VideoInfoHeaderN>
    bool enumerateCapabilitiesHelper(IBaseFilter *videoCaptureFilter, IPin *pin, IAMStreamConfig *streamConfig, int cap, AM_MEDIA_TYPE *mediaType, VIDEO_STREAM_CONFIG_CAPS *config, VideoInfoHeaderN *videoInfoHeader, EnumEntryCallback &enumEntryCallback)
    {
        PixelFormat pixelFormat = direct_show_video_format_to_capture_format(mediaType->subtype);

        int width = videoInfoHeader->bmiHeader.biWidth;
        int height = videoInfoHeader->bmiHeader.biHeight;

        std::vector<float> fps;

    #define ADD_RATIONAL_FPS(RATIONAL_FPS) fps.push_back(FPS_FROM_RATIONAL(10000000, RATIONAL_FPS))
        ADD_RATIONAL_FPS(config->MaxFrameInterval);
        ADD_RATIONAL_FPS(config->MinFrameInterval);
        ADD_RATIONAL_FPS(videoInfoHeader->AvgTimePerFrame);

        CComPtr<IAMVideoControl> cameraControl;
        HRESULT hr = videoCaptureFilter->QueryInterface(IID_PPV_ARGS(&cameraControl));
        if (SUCCEEDED(hr)) {
            long fpsCount;
            LONGLONG *fpsList;
            hr = cameraControl->GetFrameRateList(pin, cap, {width, height}, &fpsCount, &fpsList);
            if (SUCCEEDED(hr)) {
                for (int i = 0; i < fpsCount; i ++) {
                    ADD_RATIONAL_FPS(fpsList[i]);
                }
                CoTaskMemFree(fpsList);
            }
        }
    #undef ADD_RATIONAL_FPS

        return enumEntryCallback(streamConfig, mediaType, pixelFormat, width, height, fps);
    }

public:
    std::shared_ptr<void> mfDeinitializer;
    Frame frame;
    DirectShow_Callback *ds_callback;
    int state;
    CameraInformation information;

    IMoniker *pVideoSel;
    IBaseFilter *pVCap;
    ICaptureGraphBuilder2 *pBuild;
    IGraphBuilder *pGraph;
    IMediaControl *pControl;
    IMediaEvent *pEvent;
    IBaseFilter *pGrabberFilter;
    ISampleGrabber *pSampleGrabber;
    ISampleGrabberCB *pGrabberCB;
    IBaseFilter *pNullRenderer;

    FrameCallback cb_frame;
};

} // namespace webcam_capture
#endif
