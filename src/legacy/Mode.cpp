#include <LumifyMode.h>
#include <global.h>

uint32_t LumifyMode::toHex(const char *hex)
{
    long a = strtol(&hex[1], 0, 16);
    return (uint32_t)a;
}

float LumifyMode::removeNegatives(float num)
{
    return num <= 0.0F ? 0.0F : num;
}
int LumifyMode::removeNegatives(int num)
{
    return num <= 0 ? 0 : num;
}

void LumifyMode::printCRGB(const CRGB &color)
{
    log_i("%i %i %i", (int)color.r, (int)color.g, (int)color.b);
}