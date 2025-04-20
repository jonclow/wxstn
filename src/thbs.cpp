#include "thbs.h"

TempHumidityBaroSolar::TempHumidityBaroSolar(int sol_pin) {
    solar_pin = sol_pin;
}

void TempHumidityBaroSolar::init() {
    pinMode(solar_pin, INPUT);
    bme280.begin(0x76);
}

thbs_reading TempHumidityBaroSolar::getTHBSReading() {
    thbs_reading reading;
    
    reading.baro = round(bme280.readPressure()) / 100.0F; // Pa -> hPa
    reading.humid = round(bme280.readHumidity() * 10.0F) / 10.0F;
    reading.temp = round(bme280.readTemperature() * 10.0F) / 10.0F;
    reading.solar = analogRead(solar_pin);

    return reading;
}

// Converts absolute station pressure, in hPa, to sea level relative pressure for direct comparison
// to regional readings
float TempHumidityBaroSolar::convertAbsPressureToRel(float p_abs, float temp, int alt) {
    float alt_c = 0.0065 * static_cast<float>(alt);
    float p_rel = p_abs * pow((1 - alt_c / (temp + alt_c + 273.15)), -5.257);

    return round(p_rel * 100.0F) / 100.0F;
}