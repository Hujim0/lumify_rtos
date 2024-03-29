#include <ModeHandler.h>
#include <StaticMode.h>
#include <RainbowMode.h>
#include <WaveMode.h>
#include <SkyMode.h>

#define STRIP WS2812B
#define STRIP_PIN 4
#define COLOR_ORDER GRB

ESP_EVENT_DEFINE_BASE(CHANGE_MODE_EVENT);
ESP_EVENT_DEFINE_BASE(UPDATE_ARGS_EVENT);
ESP_EVENT_DEFINE_BASE(LIGHT_SWITCH_EVENT);

void ModeHandler::lightSwitch(bool state)
{
    if (ledState == state)
        return;

    ledState = state;

    log_i("light switch: %i", (int)state);

    if (!state)
    {
        _lastBrightness = FastLED.getBrightness();

        FastLED.clear(true);
        FastLED.setBrightness(0);
        return;
    }

    FastLED.setBrightness(_lastBrightness);
}

esp_err_t ModeHandler::changeMode(int id, const char *args)
{

    if (_updateTaskHandle != nullptr)
    {
        vTaskSuspend(_updateTaskHandle);
    }

    FastLED.clearData();

    currentModeId = id;

    delete _currentMode;

    switch (id)
    {
    case 0:
        _currentMode = new StaticMode(args, leds);
        break;
    case 1:
        _currentMode = new RainbowMode(args, leds);
        break;
    case 2:
        _currentMode = new WaveMode(args, leds);
        break;
    case 3:
        _currentMode = new SkyMode(args, leds);
        break;
    default:
        log_e("Mode by the id: %i is not found!", id);
        if (_updateTaskHandle != nullptr)
        {
            vTaskResume(_updateTaskHandle);
        }
        return ESP_ERR_NOT_FOUND;
    }

    if (_updateTaskHandle != nullptr)
    {
        vTaskResume(_updateTaskHandle);
    }

    log_i("Changed mode to id: %i", id);
    return ESP_OK;
}

void ModeHandler::update()
{
    if (_currentMode != nullptr && ledState)
    {
        _currentMode->update();

        FastLED.show();
    }
}
void ModeHandler::updateArgsWithJSON(const char *data)
{
    if (_currentMode != nullptr)
        _currentMode->updateArgs(data);
}
void ModeHandler::pushArg(const char *arg, void *value)
{
    if (_currentMode != nullptr)
        _currentMode->updateArg(arg, (char *)value);
}

void ModeHandler::changeBrightness(int value)
{
    FastLED.setBrightness(value);
}

void ModeHandler::setupFastLEDTask(void *pv)
{
    pinMode(STRIP_PIN, OUTPUT);

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(static_cast<CRGB*>(pv), NUMPIXELS).setCorrection(TypicalLEDStrip);

    FastLED.clearData();
    FastLED.clear();
    FastLED.show();
}

void ModeHandler::startUpdateTask()
{
    xTaskCreatePinnedToCore(
        &updateTask,
        "mode_handler_update_task",
        10240,
        this,
        MODE_HANDLER_BASE_PRIOPRITY,
        &_updateTaskHandle,
        MODE_HANDLER_CORE_ID);
}

void ModeHandler::startFastLEDSetupTask() {
    xTaskCreatePinnedToCore(
            &setupFastLEDTask,
            "fastLED_setup_task",
            2048,
            leds,
            10,
            nullptr,
            MODE_HANDLER_CORE_ID);
}

/**
 * @param pv ModeHandler pointer
 */
void ModeHandler::updateTask(void *pv)
{
    auto *modeHandler = static_cast<ModeHandler *>(pv);

    for (;;)
    {
        modeHandler->update();
    }
}

/**
 * @param ArgumentStreamInitializer_t pointer;
 */
void ModeHandler::streamArgumentReceiverTask(void *pv)
{
    auto *init = static_cast<ArgumentStreamInitializer_t *>(pv);

    ModeHandler *ModeHandler = init->modeHandler;
    char parameterName[streamBuffLength] = "";
    strncpy(parameterName, init->streamParameterName, sizeof(parameterName));

    delete init;

    for (;;)
    {
        char dataToPush[streamBuffLength] = "";
        xQueueReceive(ModeHandler->_argumentsStreamHandler, dataToPush, portMAX_DELAY);

        ModeHandler->pushArg(parameterName, dataToPush);
    }

    vTaskDelete(nullptr);
}

void ModeHandler::startArgumentStreamTask(const char *paramName)
{
    if (_argumentStreamTaskHandler != nullptr)
    {
        deleteArgumentStreamTask();
    }

    _argumentsStreamHandler = xQueueCreate(1, streamBuffLength);

    xTaskCreatePinnedToCore(
        &streamArgumentReceiverTask,
        "argument_stream_task",
        4096,
        new ArgumentStreamInitializer_t(this, paramName),
        MODE_HANDLER_BASE_PRIOPRITY,
        &_argumentStreamTaskHandler,
        MODE_HANDLER_CORE_ID);
}

void ModeHandler::deleteArgumentStreamTask()
{
//    assert(); // or the connection task will be deleted.
    if (_argumentsStreamHandler == nullptr) {
        return;
    }

    vTaskDelete(_argumentStreamTaskHandler);
    vQueueDelete(_argumentsStreamHandler);

    _argumentStreamTaskHandler = nullptr;
}

ModeHandler::ModeHandler()
    :_changeModeQueueHandler(xQueueCreate(1, sizeof(ChangeModeEvent_t)))
{
}

void ModeHandler::pushArgumentThroughTask(void *item)
{
    xQueueSend(_argumentsStreamHandler, item, 0);
}

ModeHandler::~ModeHandler()
{
    deleteArgumentStreamTask();

    delete _currentMode;
}

ArgumentStreamInitializer_t::ArgumentStreamInitializer_t(ModeHandler *pModeHandler, const char *name)
{
    modeHandler = pModeHandler;
    streamParameterName = name;
}
