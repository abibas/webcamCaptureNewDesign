#ifndef UNIQUE_ID_INTERFACE
#define UNIQUE_ID_INTERFACE

namespace webcam_capture {

    class UniqueIdInterface {
    public:
        UniqueIdInterface() {}
        virtual ~UniqueIdInterface() {}

        /**
         * @param other Param to compare with
         * @return Bool result
         */
        virtual bool operator==(UniqueIdInterface * other) = 0;

        /**
         * @param other Param to compare with
         * @return Bool result
         */
        virtual bool operator!=(UniqueIdInterface * other) = 0;
    };
}

#endif // UNIQUE_ID_INTERFACE
