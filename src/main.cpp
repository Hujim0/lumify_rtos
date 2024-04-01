#include <main.h>
#include <WebServerWrapper.h>
#include <ModeHandler.h>

#include "Debug.h"

#include <FileSystemLegacy.h>

Main mmain = Main();

ModeHandler *modeHandler;

void wifiConnectionCallback(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
  log_i("%s: %s", event_base, wl_status_tToString[event_id]);

  log_i("local-ip: %s", WiFi.localIP().toString().c_str());

  switch ((wl_status_t)event_id)
  {
  case WL_CONNECTED:
    mmain.serverInit();
    break;
  case WL_NO_SSID_AVAIL:
  case WL_CONNECT_FAILED:
    WifiManager::startAP();
    break;

  default:
    log_e("%s, Something went wrong!", event_base);
  }
}
void initializeWithPreferences() {
    auto doc = FileManager::loadPreferencesDocument();

    int id = (*doc)["mode"];

    modeHandler->updateMode(id, std::move(FileManager::getModeArgumentsJSON(id))->as<JsonVariant>());

    modeHandler->lightSwitch((*doc)["light_switch"]);
    FastLED.setBrightness((*doc)["brightness"]);
}


void setup() {
    FileManager::begin();

    esp_event_loop_create_default();

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiConnectionCallback, NULL);

    Main::wifiSetup();

    modeHandler = new ModeHandler();
    modeHandler->setupFastLED();
    initializeWithPreferences();

//    Debug::startDebugHeapTask();

    vTaskStartScheduler();
}

void loop()
{
}

void Main::serverInit()
{
    serverWrapper = new WebServerWrapper("lumify", 443);

    serverWrapper->addLegacyHandlers();

    serverWrapper->beginServerTask();
}

void Main::wifiSetup() {
    auto *creds = new WifiCredentials();

    if (FileManager::writeToWifiCredentialsFromFile(creds) == ESP_OK) {
        WifiManager::createConnectionTask(creds);
    }
    else {
        WifiManager::startAP();
    }
}

//char current_stream[64] = "";
String current_stream;

void handleWebSocket(const char *websocketMessage)
{
  log_i("websocket: %s", websocketMessage);

  if (current_stream.isEmpty())
  {
    log_i("websocket: New stream: \"%s\"", websocketMessage);

    current_stream = websocketMessage;

    modeHandler->startArgumentStreamTask(current_stream.c_str());

    return;
  }

  if (strcmp(websocketMessage, "]") == 0)
  {
    current_stream = "";
    modeHandler->deleteArgumentStreamTask();
    return;
  }

  if (current_stream.equals(BRIGHTNESS_JSON))
  {
    FastLED.setBrightness(atoi(websocketMessage));
  }
  else
  {
    char buff[streamBuffLength];
    strcpy(buff, ((char *)websocketMessage));
    modeHandler->pushArgumentThroughTask((void *)buff);
  }
}

void onPostModeArguments(AsyncWebServerRequest *request, JsonVariant &json)
{
    int id = request->arg("id").toInt();

    esp_err_t error = modeHandler->updateMode(id, json);
    if (error == ESP_OK) {
        request->send(200);
    } else {
        request->send(404);
        return;
    }

    if (request->hasArg("save"))
    {
        FileManager::saveModeArguments(id, json);

        auto preferences = FileManager::loadPreferencesDocument();
        (*preferences)["mode"] = id;
        FileManager::savePreferencesDocument(std::move(preferences));
    }
}


void getModeArguments(AsyncWebServerRequest *request)
{
    int id = request->arg("id").toInt();

    auto args = FileManager::getModeArguments(id);

    if (args == nullptr)
    {
        log_e("[ERROR] Invalid mode id: %i", id);

        request->send(404);
        return;
    }

    request->send(
            request->beginResponse(
                    HTTP_POST,
                    "text/json",
                    *args));

    if (request->hasArg("change"))
    {
        DynamicJsonDocument doc(512);
        auto err = deserializeJson(doc, *args);
        if (!err) {
            modeHandler->updateMode(id, doc.as<JsonVariant>());
        }
        else {
            log_e("json format error: %s", err.c_str());
        }
    }
    if (request->hasArg("save"))
    {
        FileManager::saveModeArguments(id, std::move(args));

        auto preferences = FileManager::loadPreferencesDocument();
        (*preferences)["mode"] = id;
        FileManager::savePreferencesDocument(std::move(preferences));
    }
}

void lightSwitch(AsyncWebServerRequest *request)
{
    bool value = request->arg("value") == "true";
    modeHandler->lightSwitch(value);
    request->send(200);

    if (request->hasArg("save"))
    {
        auto preferences = FileManager::loadPreferencesDocument();
        (*preferences)[LIGHT_SWITCH_JSON] = value;
        FileManager::savePreferencesDocument(std::move(preferences));
    }
}