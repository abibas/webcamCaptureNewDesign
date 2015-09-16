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
    std::vector<CapabilityFormat> result;
    getVideoCapabilities(imfMediaSource, result);

    return result;
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
        const PixelFormat pixelFormat, const float fps) const
{

    IMFPresentationDescriptor *pres_desc = NULL;
    IMFStreamDescriptor *stream_desc = NULL;
    IMFMediaTypeHandler *media_handler = NULL;
    IMFMediaType *type = NULL;
    int result = 1; //TODO Err code

    HRESULT hr = source->CreatePresentationDescriptor(&pres_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("source->CreatePresentationDescriptor() failed.");
        result = -1;        //TODO Err code
        goto done;
    }

    BOOL selected;
    hr = pres_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("pres_desc->GetStreamDescriptorByIndex failed.");
        result = -2;        //TODO Err code
        goto done;
    }

    hr = stream_desc->GetMediaTypeHandler(&media_handler);

    if (FAILED(hr)) {
        DEBUG_PRINT("stream_desc->GetMediaTypehandler() failed.");
        result = -3;        //TODO Err code
        goto done;
    }

    DWORD types_count = 0;
    hr = media_handler->GetMediaTypeCount(&types_count);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.");
        result = -4;        //TODO Err code
        goto done;
    }

    PROPVARIANT var;

    bool setType = false;

    for (DWORD i = 0; i < types_count; ++i) {
        PixelFormat pixelFormatBuf = PixelFormat::UNKNOWN;
        int widthBuf = 0;
        int heightBuf = 0;
        UINT32 high = 0;
        UINT32 low =  0;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if (FAILED(hr)) {
            DEBUG_PRINT("Error: cannot get media type by index.");
            result = -5;        //TODO Err code
            goto done;
        }

        //get subtype from type
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_SUBTYPE, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        if (var.vt == VT_CLSID) {
            pixelFormatBuf = MediaFoundation_Utils::videoFormatToPixelFormat(*var.puuid);
        }
        PropVariantClear(&var);

        //get frame size from type
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
        widthBuf = (int)high;
        heightBuf = (int)low;
        PropVariantClear(&var);

        // Wait till we get the format and resolution we are looking for
        if (widthBuf != width || heightBuf != height || pixelFormatBuf != pixelFormat) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }

        // figure out fps
        // we don't read fps above because we need to set it on type, which requires PROPVARIANT of that fps
        auto trySetFps = [&type, &var, &hr, &high, &low, fps](REFGUID GUID) {
            bool setFps = false;

            PropVariantInit(&var);
            hr = type->GetItem(GUID, &var);
            if (SUCCEEDED(hr)) {
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                float fpsBuf = FPS_FROM_RATIONAL(high, low);

                if (FPS_EQUAL(fps, fpsBuf)) {
                    hr = type->SetItem(MF_MT_FRAME_RATE, var);
                    setFps = SUCCEEDED(hr);
                }
            }
            PropVariantClear(&var);

            return setFps;
        };

        // if we found the fps we are looking for and set it on the type
        if (trySetFps(MF_MT_FRAME_RATE) || trySetFps(MF_MT_FRAME_RATE_RANGE_MAX) || trySetFps(MF_MT_FRAME_RATE_RANGE_MIN)) {

            hr = media_handler->SetCurrentMediaType(type);
            if (FAILED(hr)) {
                result = -7;
                DEBUG_PRINT("Error: Failed to set the current media type for the given settings.");
            } else {
                setType = true;
            }
            MediaFoundation_Utils::safeRelease(&type);
            break;
        }

        MediaFoundation_Utils::safeRelease(&type);
    }

    if (!setType) {
        result = -8;
    }

done:
    MediaFoundation_Utils::safeRelease(&pres_desc);
    MediaFoundation_Utils::safeRelease(&stream_desc);
    MediaFoundation_Utils::safeRelease(&media_handler);
    MediaFoundation_Utils::safeRelease(&type);

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

    int result = -1;        //TODO Err code
    HRESULT hr = S_OK;

    for (DWORD media_type_index = 0; true; ++media_type_index) {
        PixelFormat pixelFormatBuf = PixelFormat::UNKNOWN;
        int widthBuf = 0;
        int heightBuf = 0;
        UINT32 high = 0;
        UINT32 low =  0;
        IMFMediaType *type = NULL;

        hr = reader->GetNativeMediaType(0, media_type_index, &type);

        if (FAILED(hr)) {
            break;
        }

        // PIXELFORMAT
        PROPVARIANT var;
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_SUBTYPE, &var);

        if (SUCCEEDED(hr)) {
            pixelFormatBuf = MediaFoundation_Utils::videoFormatToPixelFormat(*var.puuid);
        }
        PropVariantClear(&var);

        // SIZE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
        if (SUCCEEDED(hr)) {
            Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
            widthBuf = high;
            heightBuf = low;
        }
        PropVariantClear(&var);

        // figure out fps
        // we don't read fps above because we need to set it on type, which requires PROPVARIANT of that fps
        auto trySetFps = [&type, &var, &hr, &high, &low, fps](REFGUID GUID) {
            bool setFps = false;

            PropVariantInit(&var);
            hr = type->GetItem(GUID, &var);
            if (SUCCEEDED(hr)) {
                Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
                float fpsBuf = FPS_FROM_RATIONAL(high, low);

                if (FPS_EQUAL(fps, fpsBuf)) {
                    hr = type->SetItem(MF_MT_FRAME_RATE, var);
                    setFps = SUCCEEDED(hr);
                }
            }
            PropVariantClear(&var);

            return setFps;
        };

        // When the output media type of the source reader matches our specs, set it!
        if (widthBuf == width &&
                heightBuf == height &&
                pixelFormatBuf == pixelFormat &&
                (trySetFps(MF_MT_FRAME_RATE) ||
                 trySetFps(MF_MT_FRAME_RATE_RANGE_MAX) ||
                 trySetFps(MF_MT_FRAME_RATE_RANGE_MIN))) {

            hr = reader->SetCurrentMediaType(0, NULL, type);

            if (FAILED(hr)) {
                DEBUG_PRINT("Error: Failed to set the current media type for the given settings.");
                result = -2;
            } else {
                hr = S_OK;
                result = 1;        //TODO Err code
            }
            MediaFoundation_Utils::safeRelease(&type);
            break;
        }

        MediaFoundation_Utils::safeRelease(&type);
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
int MediaFoundation_Camera::getVideoCapabilities(IMFMediaSource *source,
        std::vector<CapabilityFormat> &capFormatVector) const
{

    IMFPresentationDescriptor *presentation_desc = NULL;
    IMFStreamDescriptor *stream_desc = NULL;
    IMFMediaTypeHandler *media_handler = NULL;
    IMFMediaType *type = NULL;
    int result = 1;        //TODO Err code
    CapabilityTreeBuilder capabilityBuilder;

    HRESULT hr = source->CreatePresentationDescriptor(&presentation_desc);

    if (hr == MF_E_SHUTDOWN) {
        DEBUG_PRINT("Error: The media source's Shutdown method has been called.");
        goto done;
    }

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get presentation descriptor.");
        result = -1;        //TODO Err code
        goto done;
    }

    BOOL selected;
    hr = presentation_desc->GetStreamDescriptorByIndex(0, &selected, &stream_desc);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get stream descriptor.");
        result = -2;        //TODO Err code
        goto done;
    }

    hr = stream_desc->GetMediaTypeHandler(&media_handler);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type handler.");
        result = -3;        //TODO Err code
        goto done;
    }

    DWORD types_count = 0;
    hr = media_handler->GetMediaTypeCount(&types_count);

    if (FAILED(hr)) {
        DEBUG_PRINT("Error: cannot get media type count.");
        result = -4;        //TODO Err code
        goto done;
    }

    // Loop over all the types
    PROPVARIANT var;

    for (DWORD i = 0; i < types_count; ++i) {

        PixelFormat pixelFormat = PixelFormat::UNKNOWN;
        int width = 0;
        int height = 0;
        float minFps = 0;
        float maxFps = 0;
        float currentFps = 0;
        UINT32 high = 0;
        UINT32 low =  0;

        hr = media_handler->GetMediaTypeByIndex(i, &type);

        if (FAILED(hr)) {
            DEBUG_PRINT("Error: cannot get media type by index.");
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }

        //TYPE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_SUBTYPE, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        if (var.vt == VT_CLSID) {
            pixelFormat = MediaFoundation_Utils::videoFormatToPixelFormat(*var.puuid);
        }
        PropVariantClear(&var);

        //FRAME SIZE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_SIZE, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
        width = (int)high;
        height = (int)low;
        PropVariantClear(&var);

        //MIN FRAME RATE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MIN, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
        minFps = FPS_FROM_RATIONAL(high, low);
        PropVariantClear(&var);

        //MAX FRAME RATE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_RATE_RANGE_MAX, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
        maxFps = FPS_FROM_RATIONAL(high, low);
        PropVariantClear(&var);

        //CURRENT FRAME RATE
        PropVariantInit(&var);
        hr = type->GetItem(MF_MT_FRAME_RATE, &var);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }
        Unpack2UINT32AsUINT64(var.uhVal.QuadPart, &high, &low);
        currentFps = FPS_FROM_RATIONAL(high, low);
        PropVariantClear(&var);

        // check that all required fields were set
        if (pixelFormat == PixelFormat::UNKNOWN || !width || !height || !minFps || !maxFps || !currentFps) {
            MediaFoundation_Utils::safeRelease(&type);
            continue;
        }

        capabilityBuilder.addCapability(pixelFormat, width, height, {minFps, maxFps, currentFps});

        MediaFoundation_Utils::safeRelease(&type);
    }

    capFormatVector = capabilityBuilder.build();

done:
    MediaFoundation_Utils::safeRelease(&presentation_desc);
    MediaFoundation_Utils::safeRelease(&stream_desc);
    MediaFoundation_Utils::safeRelease(&media_handler);

    return result;
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
