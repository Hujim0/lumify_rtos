#include <WaveMode.h>

void WaveMode::update()
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        float multiplier = sin((i * (length)) + offset);

        if (multiplier <= 0.01F)
        {
            leds[i] = color;
            continue;
        }

        float intensity_const = 1.0F - (multiplier * intensity);

        leds[i] = CRGB(color.r * intensity_const,
                       color.g * intensity_const,
                       color.b * intensity_const);
    }
    if (abs(offset) >= TWO_PI * 10)
    {
        offset = 0;
        return;
    }
    offset += speed * (((length + 1.0F) * 2) / 255.0F); // to make it the same as in rainbow mode
}

void WaveMode::updateArgs(const JsonVariant &args)
{
//    if (updateTaskHandle != nullptr)
//        vTaskSuspend(updateTaskHandle);

    color = toHex(args[COLOR_ARG].as<const char *>());

    speed = args[SPEED_ARG].as<float>();
    intensity = args[INTENSITY_ARG].as<float>() * 0.01F;
    length = args[LENGTH_ARG].as<float>();

    if (length != 0.0F)
        length = PI / length;

    reversed = args[REVERSED_ARG].as<bool>();

    if (!reversed)
    {
        speed = abs(speed) * -1.0F;
    }
//
//    if (updateTaskHandle != nullptr)
//        vTaskResume(updateTaskHandle);
}

void WaveMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, SPEED_ARG) == 0)
    {
        speed = atof(value);
        if (!reversed)
        {
            speed *= -1.0F;
        }
    }
    else if (strcmp(arg, INTENSITY_ARG) == 0)
        intensity = (float)atof(value) * 0.01F;
    else if (strcmp(arg, COLOR_ARG) == 0)
        color = toHex(value);
}

WaveMode::WaveMode(CRGB *_leds)
: LumifyMode(_leds)
{
}
