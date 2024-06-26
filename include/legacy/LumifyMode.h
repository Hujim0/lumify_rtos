#pragma once

#include <FastLED.h>
#include <main.h>
#include <ArduinoJson.h>

#define NUMPIXELS 300

#define STATIC_DOCUMENT_MEMORY_SIZE 1024

#define MODE_HANDLER_CORE_ID 0

class LumifyMode
{
protected:
    void startUpdateTask();

public:
    const char *SPEED_ARG = "speed";
    const char *INTENSITY_ARG = "intensity";
    const char *LENGTH_ARG = "length";
    const char *REVERSED_ARG = "reversed";
    const char *COUNT_ARG = "count";
    const char *EDIT_ARG = "edit";
    const char *START_ARG = "start";
    const char *END_ARG = "end";
    const char *COLOR_ARG = "color";

    /** pure virtual method*/
    virtual void update() = 0;
    virtual void updateArgs(const JsonVariant &args) = 0;
    virtual void updateArg(const char *arg, const char *value) = 0;

    TaskHandle_t updateTaskHandle;

    CRGB *leds;

    static LumifyMode *createAndStart(int id, const JsonVariant &args, CRGB*_leds);

    static uint32_t toHex(const char *);
    static float removeNegatives(float);
    static int removeNegatives(int);
    static void printCRGB(const CRGB &);
    static void updateTask(void*);
    explicit LumifyMode(CRGB*_leds);
    virtual ~LumifyMode();
};