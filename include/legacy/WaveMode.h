#pragma once

#include <LumifyMode.h>

class WaveMode : public LumifyMode
{
private:
    CRGB color;

    float offset;

    float speed;
    float length;
    float intensity;
    bool reversed;

public:
    void update() override;
    void updateArgs(const char *) override;
    void updateArg(const char *arg, const char *value) override;
    WaveMode(CRGB*);
    WaveMode(const char *,CRGB*);
};