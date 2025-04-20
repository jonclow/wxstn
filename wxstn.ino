#include "src/rain.h"
#include "src/wind.h"
#include "src/thbs.h"
#include <ArduinoJson.h>

#define WIND_SAMPLE_TIME 10000

Wind anemometer(3, A1, 74, WIND_SAMPLE_TIME, 2);
Rain raingauge(2);
TempHumidityBaroSolar thbs(A2);

unsigned long now;
unsigned long previousMillisWindSample;
unsigned long previousMillisWeatherReading;

void setup() {
  Serial.begin(9600);
  anemometer.init();
  raingauge.init();
  thbs.init();
  previousMillisWindSample = millis();
  previousMillisWeatherReading = millis();
}

void loop() {
  now = millis();

  // Wind sample every 10 seconds
  if (now - previousMillisWindSample >= WIND_SAMPLE_TIME) {
    previousMillisWindSample = now;
    anemometer.getWindSample();
  }

  // Weather Reading every 5 minutes, sent over serial to backend for database write
  if (now - previousMillisWeatherReading >= 300000) {
    previousMillisWeatherReading = now;
    wind_reading windReading = anemometer.getWindReading();
    thbs_reading thbsReading = thbs.getTHBSReading();

    StaticJsonDocument<128> weatherPayload;

    weatherPayload["rain"] = raingauge.getRainReading();
    weatherPayload["baro"] = thbsReading.baro;
    weatherPayload["humid"] = thbsReading.humid;
    weatherPayload["airtemp"] = thbsReading.temp;
    weatherPayload["solar"] = thbsReading.solar;

    JsonObject wslow = weatherPayload.createNestedObject("wslow");
    wslow["sp"] = windReading.lowSpeed.speed;
    wslow["dir"] = windReading.lowSpeed.direction;

    JsonObject wsmean = weatherPayload.createNestedObject("wsmean");
    wsmean["sp"] = windReading.meanSpeed.speed;
    wsmean["dir"] = windReading.meanSpeed.direction;

    JsonObject wshigh = weatherPayload.createNestedObject("wshigh");
    wshigh["sp"] = windReading.peakSpeed.speed;
    wshigh["dir"] = windReading.peakSpeed.direction;

    serializeJson(weatherPayload, Serial);
    Serial.println();
  }

}
