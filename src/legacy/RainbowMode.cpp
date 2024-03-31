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

RainbowMode::RainbowMode(CRGB *leds)
:hue(0.0F),
speed(10.0F),
count(1),
reversed(false)
{
    hueConst = (255.0F * count) / (float)(NUMPIXELS);
    if (!reversed) // should be the other way around but i like more when its reversed, so i will keep it a default
    {
        speed *= -1;
    }
}

void RainbowMode::updateArgs(const char *data)
{
    if (updateTaskHandle != nullptr)
        vTaskSuspend(updateTaskHandle);

    StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> args;
    deserializeJson(args, data);

    speed = args[SPEED_ARG].as<float>();
    count = args[COUNT_ARG].as<int>();
    reversed = args[REVERSED_ARG].as<bool>();

    hueConst = (255.0F * count) / (float)(NUMPIXELS);
    if (!reversed)
    {
        speed *= -1.0F;
    }

    args.garbageCollect();
    if (updateTaskHandle != nullptr)
        vTaskResume(updateTaskHandle);
}

void RainbowMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, SPEED_ARG) == 0)
        speed = atof(value);

    if (!reversed)
    {
        speed *= -1.0F;
    }
}

RainbowMode::RainbowMode(const char *data, CRGB*_leds)
{
    leds = _leds;
    updateArgs(data);

    startUpdateTask();
}