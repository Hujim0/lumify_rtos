#include <StaticMode.h>

void StaticMode::update()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = color;
    }
}


StaticMode::StaticMode(CRGB *_leds)
{
    leds = _leds;
    color = CRGB::White;
}

StaticMode::StaticMode(const char *data,CRGB *_leds)
{
    leds = _leds;
    updateArgs(data);
}

void StaticMode::updateArgs(const char *data)
{
    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;
    deserializeJson(args, data);

    color = toHex(args[COLOR_ARG].as<const char *>());
    // color = CRGB::White;

    args.garbageCollect();
}

void StaticMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, COLOR_ARG) == 0)
        color = toHex(value);
}
