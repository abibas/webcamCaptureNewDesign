#include <atlbase.h>
#include "media_foundation_camera.h"

#include "../capability_tree_builder.h"
#include "../utils.h"
#include "../winapi_shared/winapi_shared_unique_id.h"
#include "media_foundation_callback.h"
#include "media_foundation_color_converter_transform.h"
#include "media_foundation_decompresser_transform.h"
#include "media_foundation_utils.h"

#include <mfapi.h>
#include <mferror.h>
#include <mfidl.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <windows.h>
#include <Wmcodecdsp.h>

namespace webcam_capture {

MediaFoundation_Camera::MediaFoundation_Camera(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information, IMFMediaSource *mediaSource) :
    information(information),
    mfDeinitializer(mfDeinitializer),
    capturing(false),
    imfMediaSource(mediaSource),
    mfCallback(nullptr),
    imfSourceReader(nullptr)
{
    // empty
}

std::unique_ptr<CameraInterface> MediaFoundation_Camera::create(std::shared_ptr<void> mfDeinitializer,
        const CameraInformation &information)
{
    IMFMediaSource *mediaSource = nullptr;
    // Create the MediaSource

    const std::wstring& symbolicLink = static_cast<WinapiShared_UniqueId *>(information.getUniqueId().get())->getId();

    if (MediaFoundation_Camera::createVideoDeviceSource(symbolicLink, &mediaSource) < 0) {
        DEBUG_PRINT("Error: Can't create the media device source.");
        return nullptr;
    }

    // TODO(nurupo): get std::make_unique to work without makeing code uglier
    return std::unique_ptr<MediaFoundation_Camera>(new MediaFoundation_Camera(mfDeinitializer, information, mediaSource));
}

MediaFoundation_Camera::~MediaFoundation_Camera()
{
    // Stop capturing
    if (capturing) {
        stop();
    }

    //Release mediaSource
    MediaFoundation_Utils::safeRelease(&imfMediaSource);
}

int MediaFoundation_Camera::start(PixelFormat pixelFormat, int width, int height, float fps, FrameCallback cb, PixelFormat decodeFormat, PixelFormat decompressFormat)
{
    if (!cb) {
        DEBUG_PRINT("Error: The callback function is empty. Capturing was not started.");
        return -1;      //TODO Err code
    }

    if (capturing) {
        DEBUG_PRINT("Error: Can't start capture because we are already capturing.");
        return -2;      //TODO Err code
    }

    frameCallback = cb;

    if (pixelFormat == PixelFormat::UNKNOWN) {
        DEBUG_PRINT("Error: Can't set a pixel format for PixelFormat::UNKNOWN.");
        return -8;      //TODO Err code
    }

    if (decodeFormat == decompressFormat && decodeFormat != PixelFormat::UNKNOWN) {
        DEBUG_PRINT("Error: decompressFormat and decodeFormat can't be the same.");
        return -5;      //TODO Err code
    }

/// TODO Check and solve this
/// The currentFpsBuf variable int setDeviceFormat and in setReaderFormat shows that
/// FPS is 3000 1 time and second time 500 - in real time - it doesn't changes
/// Looks like if you want to change FPS - we need to delete imm_media_source and create it again
    //"to test just comment this"
    MediaFoundation_Utils::safeRelease(&imfMediaSource);
    // Create the MediaSource
    const std::wstring& symbolicLink = static_cast<WinapiShared_UniqueId *>(information.getUniqueId().get())->getId();

    if (createVideoDeviceSource(symbolicLink, &imfMediaSource) < 0) {
        DEBUG_PRINT("Error: Can't create the media device source.");
        return -3;
    }

    //End of "to test comment this"

    if (setDeviceFormat(imfMediaSource, width, height, pixelFormat, fps) < 0) {
        DEBUG_PRINT("Error: Can't set the device format.");
        return -9;      //TODO Err code
    }

    std::unique_ptr<MediaFoundation_DecompresserTransform> decompresser;

    // Set Decoder formats
    if (decompressFormat != PixelFormat::UNKNOWN) {
        MediaFoundation_DecompresserTransform::RESULT res;
        decompresser = MediaFoundation_DecompresserTransform::getInstance(width, height, pixelFormat, decompressFormat, res);
        if (res != MediaFoundation_DecompresserTransform::RESULT::OK) {
            DEBUG_PRINT("Error: Can't set the decompressor formats.");
            return -6;      //TODO Err code
        }
    }

    std::unique_ptr<MediaFoundation_ColorConverterTransform> colorConvertor;

    // Set Decoder formats
    if (decodeFormat != PixelFormat::UNKNOWN) {
        MediaFoundation_ColorConverterTransform::RESULT res;
        if (decompressFormat == PixelFormat::UNKNOWN ) {
            colorConvertor = MediaFoundation_ColorConverterTransform::getInstance(width, height, pixelFormat, decodeFormat, res);
        } else {
            colorConvertor = MediaFoundation_ColorConverterTransform::getInstance(width, height, decompressFormat, decodeFormat, res);
        }
        if (res != MediaFoundation_ColorConverterTransform::RESULT::OK) {
            DEBUG_PRINT("Error: Can't set the decoder formats.");
            return -10;      //TODO Err code
        }
    }

    //Create mfCallback
    MediaFoundation_Callback::createInstance(this, std::move(decompresser), std::move(colorConvertor), &mfCallback);

    // Create the source reader.
    if (createSourceReader(imfMediaSource, mfCallback, &imfSourceReader) < 0) {
        DEBUG_PRINT("Error: Can't create the source reader.");
        MediaFoundation_Utils::safeRelease(&mfCallback);
        return -11;      //TODO Err code
    }

    // Set the source reader format
    if (setReaderFormat(imfSourceReader, width, height, pixelFormat, fps) < 0) {
        DEBUG_PRINT("Error: Can't set the reader format.");
        MediaFoundation_Utils::safeRelease(&mfCallback);
        MediaFoundation_Utils::safeRelease(&imfSourceReader);
        return -12;      //TODO Err code
    }

    frame.width[0] = width;
    frame.height[0] = height;
    frame.pixel_format = decodeFormat == PixelFormat::UNKNOWN ? (decompressFormat == PixelFormat::UNKNOWN ? pixelFormat : decompressFormat) : decodeFormat;

    // Kick off the capture stream.
    HRESULT hr = imfSourceReader->ReadSample(MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, NULL, NULL, NULL, NULL);

    if (FAILED(hr)) {
        if (hr == MF_E_INVALIDREQUEST) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDREQUEST.");
        } else if (hr == MF_E_INVALIDSTREAMNUMBER) {
            DEBUG_PRINT("ReadSample returned MF_E_INVALIDSTREAMNUMBER.");
        } else if (hr == MF_E_NOTACCEPTING) {
            DEBUG_PRINT("ReadSample returned MF_E_NOTACCEPTING.");
        } else if (hr == E_INVALIDARG) {
            DEBUG_PRINT("ReadSample returned E_INVALIDARG.");
        } else if (hr == E_POINTER) {
            DEBUG_PRINT("ReadSample returned E_POINTER.");
        } else {
            DEBUG_PRINT("ReadSample - unhandled result.");
        }

        DEBUG_PRINT_HR_ERROR("While trying to ReadSample() on the imfSourceReader.", hr);

        MediaFoundation_Utils::safeRelease(&mfCallback);
        MediaFoundation_Utils::safeRelease(&imfSourceReader);

        return -4;      //TODO Err code
    }

    capturing = true;
    return 1;      //TODO Err code
}

int MediaFoundation_Camera::stop()
{
    if (!capturing) {
        DEBUG_PRINT("Error: Can't stop capture because we're not capturing yet.");
        return -1;    //TODO Err code
    }

    capturing = false;

    mfCallback->stop();

    MediaFoundation_Utils::safeRelease(&imfSourceReader);
    MediaFoundation_Utils::safeRelease(&mfCallback);

    return 1;   //TODO Err code
}

std::unique_ptr<Frame> MediaFoundation_Camera::captureFrame()
{
    //TODO to realise method
    return nullptr;
}

// ---- Capabilities ----
std::vector<CapabilityFormat> MediaFoundation_Camera::getCapabilities()
{
    CapabilityTreeBuilder capabilityBuilder;

    enumerateCapabilities(imfMediaSource, [&capabilityBuilder](IMFPresentationDescriptor*, DWORD, BOOL&, IMFMediaTypeHandler*, IMFMediaType*, PixelFormat pixelFormat, int width, int height, std::vector<std::pair<UINT32, UINT32>> fps)
    {
        std::vector<float> fpsList;
        fpsList.reserve(fps.size());

        for (auto &&f : fps) {
            fpsList.push_back(FPS_FROM_RATIONAL(f.first, f.second));
        }
        capabilityBuilder.addCapability(pixelFormat, width, height, fpsList);

        return false;
    });

    return capabilityBuilder.build();
}



bool MediaFoundation_Camera::getPropertyRange(VideoProperty property, VideoPropertyRange &videoPropRange)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    long lMin, lMax, lStep, lDefault, lCaps;

    HRESULT hr = imfMediaSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Can't get IAMVideoProcAmp object.", hr);
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
            DEBUG_PRINT("Error: Unsupported VideoProperty.");
            return false;
        }
    }

    hr = pProcAmp->GetRange(ampProperty, &lMin, &lMax, &lStep, &lDefault, &lCaps);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't get range of the VideoProperty.", hr);
        return false;
    }

    videoPropRange = VideoPropertyRange(lMin, lMax, lStep, lDefault);

    return true;
}



int MediaFoundation_Camera::getProperty(VideoProperty property)
{

    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    HRESULT hr = imfMediaSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Can't get IAMVideoProcAmp object.", hr);
        return -99999;///TODO to return error value
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
            DEBUG_PRINT("Error: Unsupported VideoProperty.");
            return -99999; ///TODO to return error value
        }
    }

    long value;
    long flags;
    hr = pProcAmp->Get(ampProperty, &value, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't get value of the VideoProperty.", hr);
        return -99999;
    }

    return value;
}



bool MediaFoundation_Camera::setProperty(const VideoProperty property, const int value)
{
    IAMVideoProcAmp *pProcAmp = NULL;
    VideoProcAmpProperty ampProperty;
    HRESULT hr = imfMediaSource->QueryInterface(IID_PPV_ARGS(&pProcAmp));

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Can't get IAMVideoProcAmp object.", hr);
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
            return 0; ///TODO to return error value
        }
    }

    long val;
    long flags;
    hr = pProcAmp->Get(ampProperty, &val, &flags);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't get value of the VideoProperty.", hr);
        return false;
    }

    hr = pProcAmp->Set(ampProperty, value, flags);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't set value of the VideoProperty.", hr);
        return false;
    }

    return true;
}

/* PLATFORM SDK SPECIFIC */
/* -------------------------------------- */

int MediaFoundation_Camera::setDeviceFormat(IMFMediaSource *source, const int width, const int height,
        const PixelFormat pixelFormat, const float fps)
{
    int result = -1;
    DWORD selectedStreamDescriptor = -1;

    enumerateCapabilities(source, [&selectedStreamDescriptor, &result, &pixelFormat, &width, &height, &fps](IMFPresentationDescriptor *presentationDescriptor, DWORD iStreamDescriptor, BOOL &isStreamSelected, IMFMediaTypeHandler *mediaTypeHandler, IMFMediaType *mediaType, PixelFormat pixelFormatEnum, int widthEnum, int heightEnum, std::vector<std::pair<UINT32, UINT32>> fpsEnum)
    {
        // whitelist the stream we have selected from being deselected
        if (iStreamDescriptor == selectedStreamDescriptor) {
            return false;
        }

        // deselect any selected stream
        if (isStreamSelected) {
            HRESULT hr = presentationDescriptor->DeselectStream(iStreamDescriptor);
            if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't deselect stream descriptor.", hr);
            } else {
                isStreamSelected = false;
            }
        }

        if (pixelFormat != pixelFormatEnum || width != widthEnum || height != heightEnum) {
            return false;
        }

        if (!setMatchingFpsOnMediaType(mediaType, fps, fpsEnum)) {
            return false;
        }

        HRESULT hr = presentationDescriptor->SelectStream(iStreamDescriptor);
        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't select stream descriptor.", hr);
            return false;
        }

        selectedStreamDescriptor = isStreamSelected;
        isStreamSelected = true;

        hr = mediaTypeHandler->SetCurrentMediaType(mediaType);
        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't set media type.", hr);
            return true;
        } else {
            result = 1;
        }

        return false;
    });

    return result;
}

int MediaFoundation_Camera::createSourceReader(IMFMediaSource *mediaSource,  IMFSourceReaderCallback *callback,
        IMFSourceReader **sourceReader) const
{

    if (mediaSource == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the IMFMediaSource passed into this function is not valid.");
        return -1;         //TODO Err code
    }

    if (callback == NULL) {
        DEBUG_PRINT("Error: Cannot create a source reader because the calls back passed into this function is not valid.");
        return -2;        //TODO Err code
    }

    HRESULT hr = S_OK;
    IMFAttributes *attrs = NULL;
    int result = 1;        //TODO Err code

    hr = MFCreateAttributes(&attrs, 2);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create attributes for the media source reader.");
        result = -3;        //TODO Err code
        goto done;
    }

    hr = attrs->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: SetUnknown() failed on the source reader");
        result = -4;        //TODO Err code
        goto done;
    }

    /// This attribute gives such result - source reader does not shut down the media source.
    hr = attrs->SetUINT32(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, TRUE);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: SetUINT32() failed on the source reader");
        result = -5;        //TODO Err code
        goto done;
    }

    // Create a source reader which sets up the pipeline for us so we get access to the pixels
    hr = MFCreateSourceReaderFromMediaSource(mediaSource, attrs, sourceReader);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: while creating a source reader.");
        result = -6;        //TODO Err code
        goto done;
    }

done:
    MediaFoundation_Utils::safeRelease(&attrs);
    return result;
}

int MediaFoundation_Camera::setReaderFormat(IMFSourceReader *reader, const int width, const int height,
        const PixelFormat pixelFormat, const float fps) const
{
    // deselect all streams
    HRESULT hr = reader->SetStreamSelection(MF_SOURCE_READER_ALL_STREAMS, false);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't deselect all streams.", hr);
        return -1;
    }

    for (DWORD iStreamIndex = 0; true; iStreamIndex ++) {

        for (DWORD iMediaType = 0; true; iMediaType ++) {

            CComPtr<IMFMediaType> mediaType;
            hr = reader->GetNativeMediaType(iStreamIndex, iMediaType, &mediaType);

            if (hr == MF_E_NO_MORE_TYPES) {
                // go to the next stream
                break;
            } else if (hr == MF_E_INVALIDSTREAMNUMBER) {
                // no more streams available
                return -1;
            } else if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't get native media type.", hr);
                return -1;
            }

            PixelFormat pixelFormatBuf;
            int widthBuf;
            int heightBuf;
            std::vector<std::pair<UINT32, UINT32>> fpsBuf;

            if (!getCapabilityFromMediaType(mediaType.p, pixelFormatBuf, widthBuf, heightBuf, fpsBuf)) {
                continue;
            }

            if (widthBuf != width || heightBuf != height || pixelFormatBuf != pixelFormat) {
                continue;
            }

            if (!setMatchingFpsOnMediaType(mediaType, fps, fpsBuf)) {
                continue;
            }

            hr = reader->SetStreamSelection(iStreamIndex, true);
            if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't select stream.", hr);
                return -1;
            }

            hr = reader->SetCurrentMediaType(iStreamIndex, nullptr, mediaType);
            if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't set media type.", hr);
                return -1;
            }

            return 1;
        }
    }

    return -1;
}

bool MediaFoundation_Camera::setMatchingFpsOnMediaType(IMFMediaType *mediaType, float fps, std::vector<std::pair<UINT32, UINT32>> &fpsList)
{
    bool setFps = false;

    for (auto &&f : fpsList) {
        if (FPS_EQUAL(fps, FPS_FROM_RATIONAL(f.first, f.second))) {
            UINT64 rationalFps = Pack2UINT32AsUINT64(f.first, f.second);
            HRESULT hr = mediaType->SetUINT64(MF_MT_FRAME_RATE, rationalFps);
            setFps = SUCCEEDED(hr);
            break;
        }
    }

    return setFps;
}

bool MediaFoundation_Camera::getCapabilityFromMediaType(IMFMediaType *mediaType, PixelFormat &pixelFormat, int &width, int &height, std::vector<std::pair<UINT32, UINT32>> &fps)
{
    HRESULT hr;

    // Pixel Format
    {
        GUID subtype;
        hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subtype);
        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get subtype of a media type.", hr);
            return false;
        }
        pixelFormat = MediaFoundation_Utils::videoFormatToPixelFormat(subtype);
        if (pixelFormat == PixelFormat::UNKNOWN) {
            return false;
        }
    }

    // width x height
    {
        UINT64 frameSize;
        UINT32 frameSizeHigh;
        UINT32 frameSizeLow;

        hr = mediaType->GetUINT64(MF_MT_FRAME_SIZE, &frameSize);
        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get frame size of a media type.", hr);
            return false;
        }
        Unpack2UINT32AsUINT64(frameSize, &frameSizeHigh, &frameSizeLow);
        width = (int)frameSizeHigh;
        height = (int)frameSizeLow;
    }

    // FPS
    auto tryGetFps = [&mediaType, &hr, &fps](REFGUID GUID) {
        UINT64 rationalFps;
        UINT32 rationalFpsHigh;
        UINT32 rationalFpsLow;
        hr = mediaType->GetUINT64(GUID, &rationalFps);
        if (FAILED(hr)) {
            return false;
        }
        Unpack2UINT32AsUINT64(rationalFps, &rationalFpsHigh, &rationalFpsLow);
        fps.push_back({rationalFpsHigh, rationalFpsLow});

        return true;
    };

    if (!tryGetFps(MF_MT_FRAME_RATE_RANGE_MIN) ||
        !tryGetFps(MF_MT_FRAME_RATE_RANGE_MAX) ||
        !tryGetFps(MF_MT_FRAME_RATE)) {
            return false;
        }

    return true;
}

bool MediaFoundation_Camera::enumerateCapabilities(IMFMediaSource *source, EnumEntryCallback enumEntryCallback)
{
    CComPtr<IMFPresentationDescriptor> presentationDescriptor;
    HRESULT hr = source->CreatePresentationDescriptor(&presentationDescriptor);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't get presentation descriptor.", hr);
        return false;
    }

    DWORD streamDescriptorCount;
    hr = presentationDescriptor->GetStreamDescriptorCount(&streamDescriptorCount);

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Couldn't get stream descriptor count.", hr);
        return false;
    }

    for (DWORD iStreamDescriptor = 0; iStreamDescriptor < streamDescriptorCount; iStreamDescriptor++) {

        BOOL isStreamSelected;
        CComPtr<IMFStreamDescriptor> streamDescriptor;
        hr = presentationDescriptor->GetStreamDescriptorByIndex(iStreamDescriptor, &isStreamSelected, &streamDescriptor);

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get stream descriptor.", hr);
            continue;
        }

        CComPtr<IMFMediaTypeHandler> mediaTypeHandler;
        hr = streamDescriptor->GetMediaTypeHandler(&mediaTypeHandler);

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get media type handler.", hr);
            continue;
        }

        // check if it's stream for video
        // TODO(nurupo): handle still images MFMediaType_Image
        // https://msdn.microsoft.com/en-us/library/windows/desktop/aa367377(v=vs.85).aspx
        /*GUID majorType;
        hr = mediaTypeHandler->GetMajorType(&majorType);

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get major type of the stream.", hr);
            continue;
        } else if (!IsEqualGUID(majorType, MFMediaType_Video)) {
            continue;
        }*/


        DWORD mediaTypeCount;
        hr = mediaTypeHandler->GetMediaTypeCount(&mediaTypeCount);

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Couldn't get media type count.", hr);
            continue;
        }

        // Loop over all the types
        for (DWORD iMediaType = 0; iMediaType < mediaTypeCount; iMediaType++) {

            CComPtr<IMFMediaType> mediaType;
            hr = mediaTypeHandler->GetMediaTypeByIndex(iMediaType, &mediaType);

            if (FAILED(hr)) {
                DEBUG_PRINT_HR_ERROR("Couldn't get media type.", hr);
                continue;
            }

            PixelFormat pixelFormat;
            int width;
            int height;
            std::vector<std::pair<UINT32, UINT32>> fps;

            if (!getCapabilityFromMediaType(mediaType.p, pixelFormat, width, height, fps)) {
                continue;
            }

            if (enumEntryCallback(presentationDescriptor.p, iStreamDescriptor, isStreamSelected, mediaTypeHandler, mediaType, pixelFormat, width, height, fps)) {
                return true;
            }
        }
    }

    return true;
}

/**
 * Create and active the given `device`.
 *
 * @param WCHAR *pszSymbolicLink[in] The device symbolic Link, which is unique identifier of camera device
 *                                   for which you want to get an activated IMFMediaSource object. This function
 *                                   allocates this object and increases the reference
 *                                   count. When you're ready with this object, make sure
 *                                   to call `safeReleaseMediaFoundation(&source)`
 *
 * @param IMFMediaSource** [out]     We allocate and activate the device for the
 *                                   given `device` parameter. When ready, call
 *                                   `safeReleaseMediaFoundation(&source)` to free memory.
 */
int MediaFoundation_Camera::createVideoDeviceSource(const std::wstring &pszSymbolicLink, IMFMediaSource **ppSource)
{
    *ppSource = NULL;
    IMFAttributes *pAttributes = NULL;

    HRESULT hr = MFCreateAttributes(&pAttributes, 2);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot create MFCreateAttributes.");
        MediaFoundation_Utils::safeRelease(&pAttributes);
        return -1;        //TODO Err code
    }

    // Set the device type to video.
    hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
                              MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the device type to video.");
        MediaFoundation_Utils::safeRelease(&pAttributes);
        return -2;        //TODO Err code
    }

    // Set the symbolic link.
    hr = pAttributes->SetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK,
                                pszSymbolicLink.c_str());

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot set the symbolic link.");
        MediaFoundation_Utils::safeRelease(&pAttributes);
        return -3;        //TODO Err code
    }

    hr = MFCreateDeviceSource(pAttributes, ppSource);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot crete MF Device Source.");
        MediaFoundation_Utils::safeRelease(&pAttributes);
        return -4;        //TODO Err code
    }

    MediaFoundation_Utils::safeRelease(&pAttributes);
    return 1; //TODO Err code
}
} // namespace webcam_capture
