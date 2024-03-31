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
            1024,
            this,
            1,
            &updateTaskHandle,
            MODE_HANDLER_CORE_ID);
}

LumifyMode::~LumifyMode() {
    vTaskDelete(updateTaskHandle);
}
