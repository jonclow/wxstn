#ifndef WIND
#define WIND
#include <Arduino.h>

struct wind
{
    int speed;
    int direction;
};

struct wind_reading
{
    wind lowSpeed;
    wind meanSpeed;
    wind peakSpeed;
};

const unsigned long WIND_DEBOUNCE = 20;

class Wind
{
    private:
        static Wind *windInst;
        float anemometer_factor;
        int speed_pin;
        int direction_pin;
        float sample_time_sec;
        float distance_per_rotation_m;
        wind speed_dir[40];
        unsigned long now;
        unsigned long previousMillis;
        volatile int wind_count;
        volatile int sample_count;
        static void countWindISR();
        void countWindInterrupt();
        int calculateAverageWindDirecton(int dirs[], int count);

    public:
        Wind(int sp_pin, int dir_pin, int radius_mm, int sample_time_ms, float anemometer_factor);
        void init();
        void getWindSample();
        wind_reading getWindReading();
};

#endif