#include "rain.h"

// Singleton pattern for only one tipping rain gauge and static ISR
Rain *Rain::rainInst = NULL;

Rain::Rain(int rn_pin) {
    rainInst = this;
    rain_pin = rn_pin;
    rainCount = 0;
    previousMillis = 0;
}

void Rain::init() {
    pinMode(rain_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rain_pin), countRainISR, FALLING);
}

float Rain::getRainReading() {
    float volForPeriod = static_cast<float>(rainCount) * VOL_PER_TIP;
    rainCount = 0;
    return round(volForPeriod * 10.0F) / 10.0F;
}

void Rain::countRainInterrupt() {
    now = millis();
    if (now - previousMillis >= TIP_DEBOUNCE) {
        previousMillis = now;
        rainCount++;
    }
}

void Rain::countRainISR() {
    if (rainInst) {
        rainInst->countRainInterrupt();
    }
}