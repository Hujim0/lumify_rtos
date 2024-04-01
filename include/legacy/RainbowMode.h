#pragma once

#include <LumifyMode.h>

class RainbowMode : public LumifyMode
{
private:
    float hue{};
    bool reversed{};
    float hueConst{};

    float speed{};
    int count{};

public:
    void update() override;
    void updateArgs(const JsonVariant &args) override;
    void updateArg(const char *arg, const char *value) override;

    explicit RainbowMode(CRGB*_leds);
};