#pragma once

#include <LumifyMode.h>

#define LIGHT_SWITCH_JSON "light_switch"
#define MODE_SWITCH_JSON "mode_switch"
#define MODE_UPDATE_JSON "mode_update"
#define BRIGHTNESS_JSON "brightness"
#define ARGS_REQUEST_JSON "mode_args_request"
#define STREAM_OPEN_JSON "open_stream"
#define STREAM_CLOSE_JSON "close_stream"
// #define EPOCH_TIME "epoch_time"

#define PRIORITY_OF_MODE_HANDLER_EVENT_LOOP 1

#define NAME_OF_MODE_HANDLER_EVENT_LOOP "MODE_HANDLER"

#define MODE_HANDLER_CORE_ID 0

#define MODE_HANDLER_EVENT_LOOP_STACK_SIZE 512

ESP_EVENT_DECLARE_BASE(CHANGE_MODE_EVENT);
ESP_EVENT_DECLARE_BASE(UPDATE_ARGS_EVENT);
ESP_EVENT_DECLARE_BASE(LIGHT_SWITCH_EVENT);

#define MODE_HANDLER_BASE_PRIOPRITY 0

#define streamBuffLength 64

class ModeHandler
{
private:
    LumifyMode *_currentMode = nullptr;

    uint8_t _lastBrightness = 0;

    TaskHandle_t _argumentStreamTaskHandler = nullptr;
    TaskHandle_t _changeModeTaskHandler = nullptr;

    TaskHandle_t _updateTaskHandle = nullptr;

    QueueHandle_t _argumentsStreamHandler;
    QueueHandle_t _changeModeQueueHandler;

public:
    CRGB *leds = new CRGB[NUMPIXELS];
    int currentModeId;
    bool ledState = true;
    void lightSwitch(bool);
    void changeBrightness(int);
    esp_err_t changeMode(int, const char *);
    void updateArgsWithJSON(const char *);
    void pushArg(const char *arg, void *data);
    void update();

    void startUpdateTask();

    ModeHandler();
    ~ModeHandler();
    void pushArgumentThroughTask(void *);

    static void setupFastLEDTask(void*);
    static void updateTask(void *pv);
    static void streamArgumentReceiverTask(void *pv);
    void startArgumentStreamTask(const char *taskName);
    void deleteArgumentStreamTask();

    void startFastLEDSetupTask();
};

struct
{
    const char *jsonArgs;
    int modeId;
} ChangeModeEvent_t;

class ArgumentStreamInitializer_t
{
public:
    ModeHandler *modeHandler;
    const char *streamParameterName;

    ArgumentStreamInitializer_t(ModeHandler *, const char *);
};
