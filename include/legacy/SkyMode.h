#pragma once

#include <LumifyMode.h>

class SkyMode : public LumifyMode
{
private:
    bool editMode{};
    int sunrisePoint{};
    int sunsetPoint{};
    float speed{};

    unsigned long sunriseStartTimeMillis{};
    unsigned long lastTimeMillis = 0UL;

    void showEditMode();
    void showSunriseLight(float);
    void showSunriseSun(float);
    void tiltColors(float);

    CRGB SUN_COLOR{};
    CRGB SKY_COLOR{};

    float skyHue{};
    float skyValue{};

public:
    void update() override;
    void updateArgs(const JsonVariant &args) override;
    void updateArg(const char *arg, const char *value) override;

    explicit SkyMode(CRGB *_leds);
};