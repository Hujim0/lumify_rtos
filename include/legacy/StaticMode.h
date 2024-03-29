#pragma once

#include <LumifyMode.h>

class StaticMode : public LumifyMode
{
private:
    CRGB color;
    // StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;

public:
    void update() override;
    void updateArgs(const char *data) override;
    void updateArg(const char *arg, const char *value) override;
    StaticMode(CRGB *);
    StaticMode(const char *,CRGB *);
};