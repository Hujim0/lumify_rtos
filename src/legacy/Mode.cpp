#include <LumifyMode.h>
#include <global.h>
#include "StaticMode.h"
#include "RainbowMode.h"
#include "WaveMode.h"
#include "SkyMode.h"

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

void LumifyMode::updateTask(void *pVoid) {

    auto *mode = static_cast<LumifyMode*>(pVoid);

    for (;;) {
        mode->update();
        FastLED.show();
    }
}

void LumifyMode::startUpdateTask() {
    xTaskCreatePinnedToCore(
            &updateTask,
            "update_mode_task",
            5120,
            this,
            1,
            &updateTaskHandle,
            MODE_HANDLER_CORE_ID);
}

LumifyMode::~LumifyMode() {
    log_i("deleting task");
    vTaskDelete(updateTaskHandle);
}


/**
 *
 * @return nullptr if mode is not found
 */
LumifyMode *LumifyMode::createAndStart(int id, const JsonVariant &args, CRGB *_leds) {

    LumifyMode *mode;

    switch (id)
    {
        case 0:
            mode = new StaticMode(_leds);
            break;
        case 1:
            mode = new RainbowMode(_leds);
            break;
        case 2:
            mode = new WaveMode(_leds);
            break;
        case 3:
            mode = new SkyMode(_leds);
            break;
        default:
            return nullptr;
    }

    mode->updateArgs(args);
    mode->startUpdateTask();

    return mode;
}

LumifyMode::LumifyMode(CRGB* _leds)
:leds(_leds)
{
}
