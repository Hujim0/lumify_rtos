#include <SkyMode.h>
#include <global.h>

const unsigned long MILLIS_BEFORE_SUNRISE_START = 0;
#define SECONDS_BEFORE_SKY_SHOWS 40.0F
#define SECONDS_BEFORE_HUE_TILT 0.0F
#define INITIAL_GLOW_RADIUS 200
#define SUNRISE_GLOW_DROPOFF 30
#define SUNRISE_STEEPNESS 1.5F

#define SECONDS_BEFORE_SUN_STARTS_TO_SHOW 20.0F
#define SUN_RADIUS 5.0F
#define SECONDS_BEFORE_SUN_FULLY_CAME_OUT 20.0F

const CRGB SUNRISE_SUN_COLOR = CRGB(255, 0, 0);

void SkyMode::update()
{
    if (editMode)
    {
        showEditMode();
        return;
    }

    if (millis() < sunriseStartTimeMillis)
        return;

    float secondsSinceSunriseStart = (float)(millis() - sunriseStartTimeMillis) / (1000.0F * (2.1F - speed));

    // initial sunrise light
    if (secondsSinceSunriseStart <= SECONDS_BEFORE_SKY_SHOWS)
    {
        showSunriseLight(secondsSinceSunriseStart);
    }

    if (secondsSinceSunriseStart >= SECONDS_BEFORE_SUN_STARTS_TO_SHOW)
    {
        const float DELAY = 0.0F;

        if (secondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW > DELAY && secondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW < DELAY + 60.0F)
        {
            tiltColors(secondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW - DELAY);
        }

        showSunriseSun(secondsSinceSunriseStart - SECONDS_BEFORE_SUN_STARTS_TO_SHOW);
    }
}

void SkyMode::updateArgs(const JsonVariant &args)
{
//    if (updateTaskHandle != nullptr)
//        vTaskSuspend(updateTaskHandle);

    FastLED.clearData();

    editMode = args[EDIT_ARG].as<bool>();
    sunriseStartTimeMillis = millis() + MILLIS_BEFORE_SUNRISE_START;
    sunrisePoint = args[START_ARG].as<int>();
    sunsetPoint = args[END_ARG].as<int>();
    speed = args[SPEED_ARG].as<float>();

    // Serial.print("sunrise start: ");
    // Serial.println(sunriseStartTimeMillis);

    SKY_COLOR = CHSV(0, 255, 60);
    SUN_COLOR = CRGB(255, 0, 0);
////
//    if (updateTaskHandle != nullptr)
//        vTaskResume(updateTaskHandle);

}
void SkyMode::updateArg(const char *arg, const char *value)
{
    if (strcmp(arg, SPEED_ARG) == 0)
    {
        speed = atof(value);
    }
}
void SkyMode::showEditMode()
{
    if (sunrisePoint != 0)
        leds[sunrisePoint - 1] = CRGB(64, 64, 0);

    leds[sunrisePoint] = CRGB(255, 255, 0);
    leds[sunrisePoint + 1] = CRGB(64, 64, 0);

    if (sunsetPoint != NUMPIXELS)
        leds[sunsetPoint + 1] = CRGB(64, 0, 0);

    leds[sunsetPoint] = CRGB(255, 0, 0);
    leds[sunsetPoint - 1] = CRGB(64, 0, 0);
}

void SkyMode::showSunriseLight(float SecondsSinceSunriseStart)
{
    float phase = (SecondsSinceSunriseStart / SECONDS_BEFORE_SKY_SHOWS);

    if (SecondsSinceSunriseStart > SECONDS_BEFORE_HUE_TILT && SecondsSinceSunriseStart < 23.0F)
    {
        skyHue = (SecondsSinceSunriseStart - SECONDS_BEFORE_HUE_TILT) * 0.8F;
        skyValue = 120.0F * phase;
        SKY_COLOR = CHSV(skyHue, 255, skyValue);
    }

    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = SKY_COLOR;
    }
}

void SkyMode::showSunriseSun(float SecondsSinceSunriseStart)
{
    float phase = ((SecondsSinceSunriseStart) / SECONDS_BEFORE_SUN_FULLY_CAME_OUT);

    if (phase >= 1.0F)
        phase = 1.0F;

    float SunOffset = SecondsSinceSunriseStart * 0.075F;

    // for (int i = sunrisePoint + floor(SunOffset); i < sunrisePoint + SUN_RADIUS + 3 + floor(SunOffset); i++)
    for (int i = 0; i < NUMPIXELS; i++)
    {
        if (i < sunrisePoint + floor(SunOffset) - (SUN_RADIUS + 1) || i > sunrisePoint + floor(SunOffset) + (SUN_RADIUS + 1))
            continue;

        float sunAlpha = cos((((float)(i - sunrisePoint) / (float)SUN_RADIUS) * HALF_PI) - SunOffset * (HALF_PI / SUN_RADIUS)); // to simulate that sun is a sphere
        // float sunAlpha = cos((((float)(i - sunrisePoint) * HALF_PI) - SunOffset) / SUN_RADIUS); // to simulate that sun is a sphere

        if (sunAlpha < 0.0F)
            sunAlpha = 0.0F;

        float multiplier = phase - (1.0F - sunAlpha);

        if (multiplier <= 0.0F)
            multiplier = 0.0F;

        CRGB new_color = SKY_COLOR + CRGB(SUN_COLOR.r * multiplier,
                                          SUN_COLOR.g * multiplier,
                                          SUN_COLOR.b * multiplier);

        leds[i] = new_color;
    }
}

void SkyMode::tiltColors(float SecondsSinceSunriseStart)
{

    float phase = (SecondsSinceSunriseStart) * 0.125F;

    const float phaseDelay = 2.5F;

    if (phase >= phaseDelay)
        SUN_COLOR = CHSV((phase - phaseDelay) * 6.0F, 255.0F - (phase - phaseDelay) * 8.0F, 255);

    // SKY_COLOR = CHSV(skyHue + phase * 3.0F, 255.0F - phase * 9.5F, skyValue - phase * 1.75F);

    // 16
    // 3
    // 0

    SKY_COLOR = CRGB(removeNegatives((int)floorf(phase * 0.25F) - 2) + removeNegatives(15 - (int)floorf(phase * 1.5F)),
                     2 + (int)floorf(phase * 1.4F),
                     removeNegatives(((int)floorf(phase * 2.0F)) - 3));

    // printCRGB(SKY_COLOR);

    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds[i] = SKY_COLOR;
    }
}

SkyMode::SkyMode(CRGB *_leds)
: LumifyMode(_leds)
{
}
