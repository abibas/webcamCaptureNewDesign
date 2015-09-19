#include "media_foundation_pixel_format_transform.h"

#include "media_foundation_utils.h"

#include <mftransform.h>
#include <mferror.h>
#include <mfapi.h>

namespace webcam_capture {

std::unique_ptr<MediaFoundation_PixelFormatTransform> MediaFoundation_PixelFormatTransform::getInstance(CComPtr<IMFTransform> &transform, int width, int height, PixelFormat inputPixelFormat, PixelFormat outputPixelFormat, RESULT &result)
{
    DWORD inputStreamId;
    DWORD outputStreamId;

    getStreamIds(transform, inputStreamId, outputStreamId);

    GUID inputSubtype;
    GUID outputSubtype;

    PRIVATE_RESULT res = getSubtypeForPixelFormat(transform, &IMFTransform::GetInputAvailableType, inputPixelFormat, inputStreamId, inputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = setSubtypeMediaType(transform, &IMFTransform::SetInputType, width, height, inputStreamId, inputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = getSubtypeForPixelFormat(transform, &IMFTransform::GetOutputAvailableType, outputPixelFormat, outputStreamId, outputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_OUTPUT_FOR_INPUT : RESULT::FAILURE;
        return nullptr;
    }
    res = setSubtypeMediaType(transform, &IMFTransform::SetOutputType, width, height, outputStreamId, outputSubtype);
    if (res != PRIVATE_RESULT::OK) {
        result = res == PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT ? RESULT::UNSUPPORTED_OUTPUT_FOR_INPUT : RESULT::FAILURE;
        return nullptr;
    }

    MFT_OUTPUT_STREAM_INFO osi;

    HRESULT hr = transform->GetOutputStreamInfo(outputStreamId, &osi);
    if (FAILED(hr)) {
        result = RESULT::FAILURE;
        return nullptr;
    }

    bool weAllocateOutputSample = !((osi.dwFlags & MFT_OUTPUT_STREAM_PROVIDES_SAMPLES) || (osi.dwFlags & MFT_OUTPUT_STREAM_CAN_PROVIDE_SAMPLES));

    CComPtr<IMFSample> outputSample;
    CComPtr<IMFMediaBuffer> outputSampleBuffer;

    if (weAllocateOutputSample) {
        hr = MFCreateSample(&outputSample);
        if (FAILED(hr)) {
            result = RESULT::FAILURE;
            return nullptr;
        }
        hr = MFCreateMemoryBuffer(osi.cbSize, &outputSampleBuffer);
        if (FAILED(hr)) {
            result = RESULT::FAILURE;
            return nullptr;
        }
        hr = outputSample->AddBuffer(outputSampleBuffer);
        if (FAILED(hr)) {
            result = RESULT::FAILURE;
            return nullptr;
        }
    }

    result = RESULT::OK;

    return std::unique_ptr<MediaFoundation_PixelFormatTransform>(new MediaFoundation_PixelFormatTransform(transform, inputStreamId, outputStreamId, weAllocateOutputSample, outputSample, outputSampleBuffer));
}

MediaFoundation_PixelFormatTransform::MediaFoundation_PixelFormatTransform(MediaFoundation_PixelFormatTransform &&other)
{
    if (this != &other) {
        // copy all data
        this->transform.Attach(other.transform.Detach());
        this->inputStreamId = other.inputStreamId;
        this->outputStreamId = other.outputStreamId;
        this->weAllocateOutputSample = other.weAllocateOutputSample;
        this->outputSample.Attach(other.outputSample.Detach());
        this->outputSampleBuffer.Attach(other.outputSampleBuffer.Detach());
    }
}

void MediaFoundation_PixelFormatTransform::getStreamIds(IMFTransform *transform, DWORD &inputStreamId, DWORD &outputStreamId)
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

MediaFoundation_PixelFormatTransform::PRIVATE_RESULT MediaFoundation_PixelFormatTransform::getSubtypeForPixelFormat(IMFTransform *transform, HRESULT (IMFTransform::*getAvailableType)(DWORD, DWORD, IMFMediaType **), PixelFormat pixelFormat, DWORD streamId, GUID &subtype)
{
    GUID desiredSubtype;
    bool ok = MediaFoundation_Utils::pixelFormatToVideoFormat(pixelFormat, desiredSubtype);

    if (!ok) {
        return PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT;
    }

    for (int i = 0; true; i++) {
        CComPtr<IMFMediaType> mediaType;

        HRESULT hr = (transform->*getAvailableType)(streamId, i, &mediaType);

        if (hr == MF_E_NO_MORE_TYPES) {
            DEBUG_PRINT("No conversion for the pixel format is available.");
            return PRIVATE_RESULT::UNSUPPORTED_PIXEL_FORMAT;
        }

        if (FAILED(hr)) {
            DEBUG_PRINT_HR_ERROR("Failed to get a list of available pixel formats for the conversion.", hr);
            return PRIVATE_RESULT::FAILURE;
        }

        GUID testSubtype;
        hr = mediaType->GetGUID(MF_MT_SUBTYPE, &testSubtype);
        if (SUCCEEDED(hr)) {
            if (IsEqualGUID(desiredSubtype, testSubtype)) {
                subtype = testSubtype;
                return PRIVATE_RESULT::OK;
            }
        }
    }
}

MediaFoundation_PixelFormatTransform::PRIVATE_RESULT MediaFoundation_PixelFormatTransform::setSubtypeMediaType(IMFTransform *transform, HRESULT (IMFTransform::*setType)(DWORD, IMFMediaType *, DWORD), int width, int height, DWORD streamId, const GUID &subtype)
{
    CComPtr<IMFMediaType> mediaType;
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

MediaFoundation_PixelFormatTransform::MediaFoundation_PixelFormatTransform(CComPtr<IMFTransform> &transform, DWORD inputStreamId, DWORD outputStreamId, bool weManageAllocation, CComPtr<IMFSample> &outputSample, CComPtr<IMFMediaBuffer> &outputSampleBuffer) :
    transform(transform), inputStreamId(inputStreamId), outputStreamId(outputStreamId), weAllocateOutputSample(weManageAllocation), outputSample(outputSample), outputSampleBuffer(outputSampleBuffer)
{
    // empty
}

void MediaFoundation_PixelFormatTransform::releaseSampleBuffers(IMFSample *sample)
{
    DWORD count;
    sample->GetBufferCount(&count);

    for (int i = 0; i < count; i ++) {
        IMFMediaBuffer *buffer;
        sample->GetBufferByIndex(i, &buffer);
        MediaFoundation_Utils::safeRelease(&buffer);
    }
}


MediaFoundation_PixelFormatTransform::~MediaFoundation_PixelFormatTransform()
{
    if (!weAllocateOutputSample && this->outputSample) {
        releaseSampleBuffers(this->outputSample);
    }
}

bool MediaFoundation_PixelFormatTransform::convert(IMFSample *inputSample, IMFSample **outputSample)
{
    HRESULT hr = transform->ProcessInput(inputStreamId, inputSample, 0);
    if (FAILED(hr)) {
        DEBUG_PRINT_HR_ERROR("Failed to process input.", hr);
        return false;
    }

    // we have to release sample Media Foundation allocates every time we convert.
    if (!weAllocateOutputSample && this->outputSample) {
        releaseSampleBuffers(this->outputSample);
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
        // store Media Foundation's allocated sample to release next time we are called (or when we are destructed)
        this->outputSample.Attach(odf.pSample);
    }

    *outputSample = odf.pSample;

    return true;
}

} // namespace webcam_capture
