#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

namespace webcam_capture {
    /**
     *  The VideoPropertyRange class representing datatype with getters and setters for video property values range, step value and default value.
     */
    class VideoPropertyRange {
        public:
            /**
            * @param min - minimum video property value (for example minimum Brightness)
            *
            * @param max - maximum video property value (for example maximum Brightness)
            *
            * @param step - delta stepping value (step size between minimum and maximum values)
            *
            * @param defaultValue - default video property value (for example default Brightness)
            */
           VideoPropertyRange (int min, int max, int step, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(step),
               defaultValue(defaultValue) {}
           ~VideoPropertyRange();
           /**
            * @return minimum video property value (for example minimum Brightness)
            */
           int getMinValue() const { return minValue; }
           /**
            * @return maximum video property value (for example minimum Brightness)
            */
           int getMaxValue() const { return maxValue; }
           /**
            * @return delta stepping value (step size between minimum and maximum values)
            */
           int getStepValue() const { return stepValue; }
           /**
            * @return default video property value (for example default Brightness)
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

