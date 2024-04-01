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

esp_err_t ModeHandler::updateMode(int id, const JsonVariant &args)
{
    if (currentModeId == id) {
        updateArgsWithJSON(args);
        return ESP_OK;
    }

    LumifyMode *mode = LumifyMode::createAndStart(id, args, leds);

    if (mode == nullptr) {
        log_e("Failed to create new mode: The id: %i is not found!", id);
        return ESP_ERR_NOT_FOUND;
    }

    FastLED.clearData();
    currentModeId = id;

    delete _currentMode;
    _currentMode = mode;

    log_i("Changed mode to id: %i", id);
    return ESP_OK;
}

void ModeHandler::updateArgsWithJSONString(const char *data)
{
    if (_currentMode != nullptr) {
        DynamicJsonDocument doc(1024);
        DeserializationError err = deserializeJson(doc, data);

        if (!err)
            _currentMode->updateArgs(doc.as<JsonVariant>());
        else
            log_e("%s", err.c_str());
    }
}
void ModeHandler::pushArg(const char *arg, void *value)
{
    if (_currentMode != nullptr)
        _currentMode->updateArg(arg, (char *)value);
}

void ModeHandler::setupFastLED() const
{
    pinMode(STRIP_PIN, OUTPUT);

    FastLED.addLeds<STRIP, STRIP_PIN, COLOR_ORDER>(leds, NUMPIXELS).setCorrection(TypicalLEDStrip);
    FastLED.clear(true);

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
//    :_changeModeQueueHandler(xQueueCreate(1, sizeof(ChangeModeEvent_t)))
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

void ModeHandler::updateArgsWithJSON(const JsonVariant &args) {
    if (_currentMode != nullptr)
        _currentMode->updateArgs(args);
}

ArgumentStreamInitializer_t::ArgumentStreamInitializer_t(ModeHandler *pModeHandler, const char *name)
{
    modeHandler = pModeHandler;
    streamParameterName = name;
}
