#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

namespace webcam_capture {
    /**
     *  Representing datatype with getters and setters for video property values range, step value and default value.
     */
    class VideoPropertyRange {
        public:
            /**
            * @param min Minimum video property value
            *
            * @param max Maximum video property value
            *
            * @param step Delta stepping value (step size between minimum and maximum values)
            *
            * @param defaultValue Default video property value
            */
           VideoPropertyRange (int min, int max, int step, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(step),
               defaultValue(defaultValue) {}
           ~VideoPropertyRange();
           /**
            * @return Minimum video property value
            */
           int getMinValue() const { return minValue; }
           /**
            * @return Maximum video property value
            */
           int getMaxValue() const { return maxValue; }
           /**
            * @return Delta stepping value (step size between minimum and maximum values)
            */
           int getStepValue() const { return stepValue; }
           /**
            * @return Default video property value
            */
           int getDefaultValue() const { return defaultValue; }
        private:
           const int minValue; 			//min Value
           const int maxValue;			//max Value
           const int stepValue;         //step Value
           const int defaultValue;      //default Value
    };

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

