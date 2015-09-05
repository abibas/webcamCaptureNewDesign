#include "media_foundation_color_converter.h"

#include "media_foundation_utils.h"

#include <mftransform.h>
#include <mferror.h>
#include <mfapi.h>
#include <wmcodecdsp.h>

namespace webcam_capture {

std::unique_ptr<MediaFoundation_ColorConverter> MediaFoundation_ColorConverter::getInstance(int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result)
{
    IMFTransform *transform = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_CColorConvertDMO, nullptr, CLSCTX_INPROC_SERVER, IID_IMFTransform, reinterpret_cast<void**>(&transform));
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to create transform.", hr);
        result = RESULT::FAILURE;
        return nullptr;
    }

    DWORD inputStreamId;
    DWORD outputStreamId;

    getStreamIds(transform, inputStreamId, outputStreamId);

    GUID inputSubtype;
    GUID outputSubtype;

    PRIVATE_RESULT res = getSubtypeForPixelFormat(transform, &IMFTransform::GetInputAvailableType, inputPixelFormat, inputStreamId, inputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        MediaFoundation_Utils::safeRelease(&transform);
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = setSubtypeMediaType(transform, &IMFTransform::SetInputType, width, height, inputStreamId, inputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        MediaFoundation_Utils::safeRelease(&transform);
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = getSubtypeForPixelFormat(transform, &IMFTransform::GetOutputAvailableType, outputPixelFormat, outputStreamId, outputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        MediaFoundation_Utils::safeRelease(&transform);
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_OUTPUT_FOR_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = setSubtypeMediaType(transform, &IMFTransform::SetOutputType, width, height, outputStreamId, outputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        MediaFoundation_Utils::safeRelease(&transform);
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_OUTPUT_FOR_INPUT : RESULT::FAILURE;
        return nullptr;
    }

    MFT_OUTPUT_STREAM_INFO osi;

    hr = transform->GetOutputStreamInfo(outputStreamId, &osi);
    if (FAILED(hr)) {
        MediaFoundation_Utils::safeRelease(&transform);
        result = RESULT::FAILURE;
        return nullptr;
    }

    bool weAllocateOutputSample = !((osi.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) || (osi.dwFlags & MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES));

    IMFSample *outputSample = nullptr;
    IMFMediaBuffer *outputSampleBuffer = nullptr;

    if (weAllocateOutputSample) {
        hr = MFCreateSample(&outputSample);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&transform);
            result = RESULT::FAILURE;
            return nullptr;
        }
        hr = MFCreateMemoryBuffer(osi.cbSize, &outputSampleBuffer);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&transform);
            MediaFoundation_Utils::safeRelease(&outputSample);
            result = RESULT::FAILURE;
            return nullptr;
        }
        hr = outputSample->AddBuffer(outputSampleBuffer);
        if (FAILED(hr)) {
            MediaFoundation_Utils::safeRelease(&transform);
            MediaFoundation_Utils::safeRelease(&outputSample);
            MediaFoundation_Utils::safeRelease(&outputSampleBuffer);
            result = RESULT::FAILURE;
            return nullptr;
        }
    }

    result = RESULT::OK;

    return std::unique_ptr<MediaFoundation_ColorConverter>(new MediaFoundation_ColorConverter(transform, inputStreamId, outputStreamId, weAllocateOutputSample, outputSample, outputSampleBuffer));
}

void MediaFoundation_ColorConverter::getStreamIds(IMFTransform *transform, DWORD &inputStreamId, DWORD &outputStreamId)
{
    // first we try to get stream ids, since some implementations of transforms use them.
    // if we fail to get them, we assume they are cosecutive and 0-index based, just like
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms693988%28v=vs.85%29.aspx
    // describes, i.e. we just use 0 values in that case.
    inputStreamId = 0;
    outputStreamId = 0;

    DWORD inputStreamIdCount;
    DWORD outputStreamIdCount;

    std::unique_ptr<DWORD[]> inputStreamIds;
    std::unique_ptr<DWORD[]> outputStreamIds;

    HRESULT hr = transform->GetStreamCount(&inputStreamIdCount, &outputStreamIdCount);
    if (SUCCEEDED(hr) && inputStreamIdCount > 0 && outputStreamIdCount > 0) {
        inputStreamIds.reset(new DWORD[inputStreamIdCount]);
        outputStreamIds.reset(new DWORD[outputStreamIdCount]);

        hr = transform->GetStreamIDs(inputStreamIdCount, inputStreamIds.get(), outputStreamIdCount, outputStreamIds.get());
        if (SUCCEEDED(hr)) {
            inputStreamId = inputStreamIds[0];
            outputStreamId = outputStreamIds[0];
        }
    }
}

MediaFoundation_ColorConverter::PRIVATE_RESULT MediaFoundation_ColorConverter::getSubtypeForPixelFormat(IMFTransform *transform, HRESULT (IMFTransform::*getAvailableType)(DWORD, DWORD, IMFMediaType **), PixelFormat pixelFormat, DWORD streamId, GUID &subtype)
{
    for (int i = 0; true; i++) {
        IMFMediaType *mediaType;

        HRESULT hr = (transform->*getAvailableType)(streamId, i, &mediaType);

        if (hr == MF_E_NO_MORE_TYPES) {
            DEBUG_PRINT("No conversion for the pixel format is available.");
            return PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT;
        }

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Failed to get a list of available pixel formats for the conversion.", hr);
            return PRIVATE_RESULT::FAILURE;
        }

        PROPVARIANT var;
        PropVariantInit(&var);
        hr = mediaType->GetItem(MF_MT_SUBTYPE, &var);
        if (SUCCEEDED(hr)) {
            PixelFormat pixelFormatBuf = MediaFoundation_Utils::videoFormatToPixelFormat(*var.puuid);
            if (pixelFormatBuf == pixelFormat && pixelFormatBuf != PixelFormat::UNKNOWN) {
                subtype = *var.puuid;
                PropVariantClear(&var);
                MediaFoundation_Utils::safeRelease(&mediaType);
                return PRIVATE_RESULT::OK;
            }
        }
        PropVariantClear(&var);
        MediaFoundation_Utils::safeRelease(&mediaType);
    }
}

MediaFoundation_ColorConverter::PRIVATE_RESULT MediaFoundation_ColorConverter::setSubtypeMediaType(IMFTransform *transform, HRESULT (IMFTransform::*setType)(DWORD, IMFMediaType *, DWORD), int width, int height, DWORD streamId, const GUID &subtype)
{
    IMFMediaType *mediaType;
    HRESULT hr = MFCreateMediaType(&mediaType);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to create MediaType.", hr);
        return PRIVATE_RESULT::FAILURE;
    }

#define CHECK_ATTRIBUTE \
if (FAILED(hr)) { \
    DEBUG_PRINT_HR_ERROR("Failed to set attribute on MediaType.", hr); \
    return PRIVATE_RESULT::FAILURE; \
}

    hr = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    CHECK_ATTRIBUTE
    hr = mediaType->SetGUID(MF_MT_SUBTYPE, subtype);
    CHECK_ATTRIBUTE
    hr = mediaType->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);
    CHECK_ATTRIBUTE
    hr = MFSetAttributeSize(mediaType, MF_MT_FRAME_SIZE, width, height);
    CHECK_ATTRIBUTE
    hr = MFSetAttributeRatio(mediaType, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);
    CHECK_ATTRIBUTE

#undef CHECK_ATTRIBUTE

    hr = (transform->*setType)(streamId, mediaType, 0);
    if (hr == MF_E_INVALIDMEDIATYPE) {
        DEBUG_PRINT("Conversion with such pixel format is unsupported.");
        return PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT;
    }
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to set MediaType on transform.", hr);
        return PRIVATE_RESULT::FAILURE;
    }

    return PRIVATE_RESULT::OK;
}

MediaFoundation_ColorConverter::MediaFoundation_ColorConverter(IMFTransform *transform, DWORD inputStreamId, DWORD outputStreamId, bool weManageAllocation, IMFSample *outputSample, IMFMediaBuffer *outputSampleBuffer) :
    transform(transform), inputStreamId(inputStreamId), outputStreamId(outputStreamId), weAllocateOutputSample(weManageAllocation), outputSample(outputSample), outputSampleBuffer(outputSampleBuffer)
{

}

void MediaFoundation_ColorConverter::releaseSample(IMFSample *sample) const
{
    DWORD count;
    sample->GetBufferCount(&count);

    for (int i = 0; i < count; i ++) {
        IMFMediaBuffer *buffer;
        sample->GetBufferByIndex(i, &buffer);
        MediaFoundation_Utils::safeRelease(&buffer);
    }

    MediaFoundation_Utils::safeRelease(&sample);
}


MediaFoundation_ColorConverter::~MediaFoundation_ColorConverter()
{
    MediaFoundation_Utils::safeRelease(&transform);

    if (weAllocateOutputSample) {
        MediaFoundation_Utils::safeRelease(&outputSample);
        MediaFoundation_Utils::safeRelease(&outputSampleBuffer);
    } else if (outputSample != nullptr) {
        // release all possible buffers the sample might have
        releaseSample(outputSample);
    }
}

bool MediaFoundation_ColorConverter::convert(IMFSample *inputSample, IMFSample **outputSample)
{
    HRESULT hr = transform->ProcessInput(inputStreamId, inputSample, 0);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to process input.", hr);
        return false;
    }

    // we have to release sample Media Foundation allocates every time we convert.
    if (!weAllocateOutputSample && this->outputSample != nullptr) {
        releaseSample(this->outputSample);
        this->outputSample = nullptr;
    }

    MFT_OUTPUT_DATA_BUFFER odf;
    odf.dwStreamID = outputStreamId;
    odf.pSample = weAllocateOutputSample ? this->outputSample : nullptr;
    odf.dwStatus = 0;
    odf.pEvents = nullptr;

    DWORD outStatus = 0;

    hr = transform->ProcessOutput(0, 1, &odf, &outStatus);

    if (odf.pEvents) {
        MediaFoundation_Utils::safeRelease(&odf.pEvents);
    }

    // TODO(nurupo): should we handle the case when MFT_PROCESS_OUTPUT_STATUS_NEW_STREAMS in outStatus is set?

    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to process output.", hr);
        return false;
    }

    if (!weAllocateOutputSample) {
        // store Media Foundation's allocated sample to release next time we are called (or in dtor)
        this->outputSample = odf.pSample;
    }

    *outputSample = odf.pSample;

    return true;
}

} // namespace webcam_capture
