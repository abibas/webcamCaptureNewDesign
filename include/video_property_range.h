#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

namespace webcam_capture {
    /**
     *  Contains valid values of a VideoProperty
     */
    class VideoPropertyRange {
        public:
            /**
            * @param min Minimum video property value
            *
            * @param max Maximum video property value
            *
            * @param step Delta stepping value
            *
            * @param defaultValue Default video property value
            */
           VideoPropertyRange (int min, int max, int step, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(step),
               defaultValue(defaultValue) {}
           ~VideoPropertyRange() {}
           /**
            * @return Minimum video property value
            */
           int getMinValue() const { return minValue; }
           /**
            * @return Maximum video property value
            */
           int getMaxValue() const { return maxValue; }
           /**
            * @return Delta stepping value
            */
           int getStepValue() const { return stepValue; }
           /**
            * @return Default video property value
            */
           int getDefaultValue() const { return defaultValue; }
        private:
           const int minValue;
           const int maxValue;
           const int stepValue;
           const int defaultValue;
    };

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

