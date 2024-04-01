#include <RainbowMode.h>

void RainbowMode::update()
{
    if (abs(hue) >= 255.0F)
        hue = 0;

    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = CHSV(hue + (i * hueConst), 255, 255);
    }

    hue += (speed * 0.1F);
}

void RainbowMode::updateArgs(const JsonVariant &args)
{
//    if (updateTaskHandle != nullptr)
//        vTaskSuspend(updateTaskHandle);

    speed = args[SPEED_ARG].as<float>();
    count = args[COUNT_ARG].as<int>();
    reversed = args[REVERSED_ARG].as<bool>();

    hueConst = (255.0F * count) / (float)(NUMPIXELS);
    if (!reversed) {
        speed *= -1.0F;
    }
//
//    if (updateTaskHandle != nullptr)
//        vTaskResume(updateTaskHandle);
}

void RainbowMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, SPEED_ARG) == 0)
        speed = atof(value);

    if (!reversed) {
        speed *= -1.0F;
    }
}

RainbowMode::RainbowMode(CRGB *_leds)
: LumifyMode(_leds)
{
}
