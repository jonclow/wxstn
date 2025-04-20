#ifndef THBS
#define THBS
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

struct thbs_reading
{
    int solar;
    float baro;
    float temp;
    float humid;
};


class TempHumidityBaroSolar
{
    private:
        Adafruit_BME280 bme280;
        int solar_pin;

    public:
       TempHumidityBaroSolar(int solar_pin);   
       void init();
       thbs_reading getTHBSReading();
       float convertAbsPressureToRel(float p_abs, float temp, int alt);
};

#endif