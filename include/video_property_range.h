#ifndef VIDEO_PROPERTY_RANGE_H
#define VIDEO_PROPERTY_RANGE_H

namespace webcam_capture {

    class VideoPropertyRange {
        public:
           VideoPropertyRange (int min, int max, int delta, int defaultValue) :
               minValue(min),
               maxValue(max),
               stepValue(delta),
               defaultValue(defaultValue) {}
           ~VideoPropertyRange();
           int getMinValue() const { return minValue; }
           int getMaxValue() const { return maxValue; }
           int getStepValue() const { return stepValue; }
           int getDefaultValue() const { return defaultValue; }
        private:
           const int minValue; 			//min Value
           const int maxValue;			//max Value
           const int stepValue;         //step Value
           const int defaultValue;      //default Value
    };

} // namespace webcam_capture

#endif // VIDEO_PROPERTY_RANGE_H

