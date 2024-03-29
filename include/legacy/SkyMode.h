#pragma once

#include <LumifyMode.h>

class SkyMode : public LumifyMode
{
private:
    bool edit_mode;
    int sunrise_point;
    int sunset_point;
    float speed;

    unsigned long sunrise_start_time;
    unsigned long last_time = 0UL;

    float SecondsSinceSunriseStart = 0.0F;

    void ShowEditMode();
    void ShowSunriseLight(float);
    void ShowSunriseSun(float);
    void TiltColors(float);

    CRGB SUN_COLOR;
    CRGB SKY_COLOR;

    float skyHue;
    float skyValue;

public:
    void update() override;
    void updateArgs(const char *) override;
    void updateArg(const char *arg, const char *value) override;
    SkyMode(CRGB*);
    SkyMode(const char *,CRGB*);
};