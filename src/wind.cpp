#include "wind.h"

// Singleton pattern for only one anemometer and static ISR
Wind *Wind::windInst = NULL;

Wind::Wind(int sp_pin, int dir_pin, int radius_mm, int sample_time_ms, float anemometer_factor) {
    windInst = this;
    this->anemometer_factor = anemometer_factor;
    speed_pin = sp_pin;
    direction_pin = dir_pin;
    wind_count = 0;
    sample_count = 0;
    distance_per_rotation_m = (2 * PI * static_cast<float>(radius_mm)) / 1000; // mm -> m
    sample_time_sec = static_cast<float>(sample_time_ms) / 1000; // ms -> s
    previousMillis = 0;
}

void Wind::init() {
    pinMode(direction_pin, INPUT);
    pinMode(speed_pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(speed_pin), countWindISR, FALLING);
}

void Wind::countWindInterrupt() {
    now = millis();
    if (now - previousMillis >= WIND_DEBOUNCE) {
        previousMillis = now;
        wind_count++;
    }
}

void Wind::countWindISR() {
    if (windInst) {
        windInst->countWindInterrupt();
    }
}

// Every x wind sample period seconds - total up the interrupt count and calc a wind speed,
// take the direction at sample time.
// This happens for y weather reading period minutes when a full wind reading is calculated and returned
// at which point the sample_count is reset.
void Wind::getWindSample() {
    float speed_ms = (wind_count * distance_per_rotation_m) / sample_time_sec * anemometer_factor;
    speed_dir[sample_count].direction = map(analogRead(direction_pin), 0, 1023, 0, 360);
    // Speed in knots
    speed_dir[sample_count].speed = round(speed_ms * (float)1.944);
    sample_count++;
    // Reset the periodic interrupt count for next sample
    wind_count = 0;
}

wind_reading Wind::getWindReading() {
    const int sampleCountAtReading = sample_count;
    int speeds[sampleCountAtReading];
    int directions[sampleCountAtReading];
    int sum = 0;
    wind min = speed_dir[0];
    wind max = speed_dir[0];
    speeds[0] = speed_dir[0].speed;
    directions[0] = speed_dir[0].direction;


    for (int i = 1; i < sampleCountAtReading; i++)
    {
        speeds[i] = speed_dir[i].speed;
        directions[i] = speed_dir[i].direction;

        if(max.speed < speed_dir[i].speed) {
            max = speed_dir[i];
        }

        if(min.speed > speed_dir[i].speed) {
            min = speed_dir[i];
        }
    }

    for (int i = 0; i < sampleCountAtReading; i++)
    {
        sum += speeds[i];
    }

    wind_reading reading;
    reading.lowSpeed = min;
    reading.peakSpeed = max;
    reading.meanSpeed.speed = round(static_cast<float>(sum) / sampleCountAtReading);
    reading.meanSpeed.direction = calculateAverageWindDirecton(directions, sampleCountAtReading);

    // Reset
    for (int i = 0; i < 40; i++)
    {
        speed_dir[i].speed = 0;
        speed_dir[i].direction = 0;
    }

    sample_count = 0;

    return reading;
}

int Wind::calculateAverageWindDirecton(int dirs[], int count) {
    double sin_sum = 0.0, cos_sum = 0.0;

    for (int i = 0; i < count; i++)
    {
        double angle_rad = radians(dirs[i]);
        sin_sum += sin(angle_rad);
        cos_sum += cos(angle_rad);
    }

    float s = sin_sum / static_cast<float>(count);
    float c = cos_sum / static_cast<float>(count);
    float arc = degrees(atan(s / c));
    float average = 0.0;

    if (s > 0 && c > 0) {
        average = arc;
    } else if (c < 0) {
        average = arc + 180;
    } else if (s < 0 && c > 0) {
        average = arc + 360;
    }

    if (average == 360) {
        return 0;
    }

    return static_cast<int>(average);
}