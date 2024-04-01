#include <StaticMode.h>

void StaticMode::update()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = color;
    }
}

void StaticMode::updateArgs(const JsonVariant &args)
{
//    if (updateTaskHandle != nullptr)
//        vTaskSuspend(updateTaskHandle);

    color = toHex(args[COLOR_ARG].as<const char *>());
//
//    if (updateTaskHandle != nullptr)
//        vTaskResume(updateTaskHandle);
}

void StaticMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, COLOR_ARG) == 0)
        color = toHex(value);
}

StaticMode::StaticMode(CRGB *_leds)
: LumifyMode(_leds)
, color(CRGB::White)
{
}
