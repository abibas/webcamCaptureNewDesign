#ifndef MEDIA_FOUNDATION_COLOR_CONVERTER_H
#define MEDIA_FOUNDATION_COLOR_CONVERTER_H

#include <memory>
#include <pixel_format.h>
#include <windows.h>

/**
 * A simple wrapper around Color Converter DSP
 * https://msdn.microsoft.com/en-us/library/windows/desktop/ff819079%28v=vs.85%29.aspx
 */

class IMFTransform;
class IMFMediaType;
class IMFSample;
class IMFMediaBuffer;

namespace webcam_capture {

class MediaFoundation_ColorConverter
{
public:

    enum class RESULT {
        OK,
        UNSUPPORTED_INPUT, // no conversion from the input pixel format is supported
        UNSUPPORTED_OUTPUT_FOR_INPUT, // conversion from the input pixel format is supported, but not to that particular output pixel format
        FAILURE // something else failed
    };

    static std::unique_ptr<MediaFoundation_ColorConverter> getInstance(int width, int height, PixelFormat in, PixelFormat out, RESULT &result);

    ~MediaFoundation_ColorConverter();

    /**
     * Preforms the color conversion.
     * @param inputSample Input sample to convert.
     * @param outputSample Pointer to where the converted sample will be stored. You must not release the sample.
     * @return true on success, false on failure.
     */
    bool convert(IMFSample *inputSample, IMFSample **outputSample);

private:

    enum class PRIVATE_RESULT {
        OK,
        UNSUPPORTED_PIXEL_FORMAT,
        FAILURE
    };

    static void getStreamIds(IMFTransform *transform, DWORD &inputStreamId, DWORD &outputStreamId);
    static PRIVATE_RESULT getSubtypeForPixelFormat(IMFTransform *transform, HRESULT (IMFTransform::*getAvailableType)(DWORD, DWORD, IMFMediaType**), PixelFormat pixelFormat, DWORD streamId, GUID &subtype);
    static PRIVATE_RESULT setSubtypeMediaType(IMFTransform *transform, HRESULT (IMFTransform::*setType)(DWORD, IMFMediaType*, DWORD), int width, int height, DWORD streamId, const GUID &subtype);
    MediaFoundation_ColorConverter(IMFTransform *transform, DWORD inputStreamId, DWORD outputStreamId, bool weAllocateOutputSample, IMFSample *outputSample, IMFMediaBuffer *outputSampleBuffer);
    void releaseSample(IMFSample* sample) const;

    IMFTransform *transform;
    const DWORD inputStreamId;
    const DWORD outputStreamId;
    const bool weAllocateOutputSample;
    IMFSample *outputSample;
    IMFMediaBuffer *outputSampleBuffer;
};

} // namespace webcam_capture

#endif // MEDIA_FOUNDATION_COLOR_CONVERTER_H
