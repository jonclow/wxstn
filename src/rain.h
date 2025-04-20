#ifndef RAIN
#define RAIN
#include <Arduino.h>

const float VOL_PER_TIP = 0.335;
const unsigned long TIP_DEBOUNCE = 2000;

class Rain
{
    private:
        int rain_pin;
        static Rain *rainInst;
        unsigned long now;
        unsigned long previousMillis;
        volatile int rainCount;
        static void countRainISR();
        void countRainInterrupt();

    public:
        Rain(int rn_pin);
        void init();
        float getRainReading();
};

#endif