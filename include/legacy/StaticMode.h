#pragma once

#include <LumifyMode.h>

class StaticMode : public LumifyMode
{
private:
    CRGB color;

public:
    void update() override;
    void updateArgs(const JsonVariant &args) override;
    void updateArg(const char *arg, const char *value) override;

    explicit StaticMode(CRGB*);
};