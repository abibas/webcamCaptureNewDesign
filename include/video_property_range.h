#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

#include <webcam_capture_export.h>

namespace webcam_capture {
/**
 * Provides range information of a property.
 */
class WEBCAM_CAPTURE_EXPORT VideoPropertyRange
{
public:
    VideoPropertyRange():
        minValue(0),
        maxValue(0),
        stepValue(0),
        defaultValue(0) {}

    /**
     * @param min Minimum video property value.
     * @param max Maximum video property value.
     * @param step Step size.
     * @param defaultValue Default video property value.
     */
    VideoPropertyRange(int min, int max, int step, int defaultValue) :
        minValue(min),
        maxValue(max),
        stepValue(step),
        defaultValue(defaultValue) {}

    ~VideoPropertyRange() {}

    /**
     * @return Minimum video property value.
     */
    int getMinValue() const
    {
        return minValue;
    }

    /**
     * @return Maximum video property value.
     */
    int getMaxValue() const
    {
        return maxValue;
    }

    /**
     * @return Step size value.
     */
    int getStepValue() const
    {
        return stepValue;
    }

    /**
     * @return Default video property value.
     */
    int getDefaultValue() const
    {
        return defaultValue;
    }

private:
    int minValue;
    int maxValue;
    int stepValue;
    int defaultValue;
};

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

