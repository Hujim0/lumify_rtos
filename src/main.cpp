#include <main.h>
#include <WebServerWrapper.h>
#include <ModeHandler.h>

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

void setup()
{
  FileManager::begin();

  esp_event_loop_create_default();

  esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiConnectionCallback, NULL);

  mmain.wifiSetup();

  modeHandler = new ModeHandler();
  modeHandler->setupFastLED();
  modeHandler->changeMode(0, GetModeArgs(0).c_str());
//  modeHandler->startUpdateTask();

  std::shared_ptr<LumifyMode> test;

  vTaskStartScheduler();
}

void loop()
{
  // mmain.serverWrapper->update();
}

void Main::serverInit()
{
  serverWrapper = new WebServerWrapper("lumify", 443);

  serverWrapper->addLegacyHandlers();

  serverWrapper->beginServerTask();
}

void Main::wifiSetup()
{
  try
  {
    WifiManager::createConnectionTask(FileManager::createWifiCredentialsFromFile());
  }
  catch (const IOException &e)
  {
    WifiManager::startAP();
  }
}

char current_stream[64] = "";

void handleWebSocket(const char *websocketMessage)
{
  log_i("websocket: %s", websocketMessage);

  if (strcmp(current_stream, "") == 0)
  {

    log_i("websocket: New stream: \"%s\"", websocketMessage);

    strcpy(current_stream, websocketMessage);

    modeHandler->startArgumentStreamTask(current_stream);

    return;
  }

  if (strcmp(websocketMessage, "]") == 0)
  {
    strcpy(current_stream, "");
      modeHandler->deleteArgumentStreamTask();
    return;
  }

  if (strcmp(current_stream, BRIGHTNESS_JSON) == 0)
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
    request->send(200);

    int id = request->arg("id").toInt();

    String *jsonString = new String((char *)nullptr);
//    std::shared_ptr<String> jsonString = std::make_shared<String>();
    serializeJson(json, *jsonString);

    if (request->hasArg("save"))
    {
        SaveModeArgs(id, jsonString->c_str());
    }

     modeHandler->changeMode(id, jsonString->c_str());
//    esp_event_post(CHANGE_MODE_EVENT, id, &args_string, sizeof(&args_string), 0);

    if (request->hasArg("save"))
    {
        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences["mode"] = id;
        SavePreferences(&preferences);
    }
}


void getModeArguments(AsyncWebServerRequest *request)
{
    int id = request->arg("id").toInt();

    String args = GetModeArgs(id);

    if (args.isEmpty())
    {
        log_e("[ERROR] Invalid mode id: %i", id);

        request->send(404);
        return;
    }

    request->send(
            request->beginResponse(
                    HTTP_POST,
                    "text/json",
                    args));

    if (request->hasArg("change"))
    {
         modeHandler->changeMode(id, args.c_str());
        log_i("posted, id: %i", id);
    }
    if (request->hasArg("save"))
    {
        SaveModeArgs(id, args.c_str());

        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences["mode"] = id;
        SavePreferences(&preferences);
    }
}

void lightSwitch(AsyncWebServerRequest *request)
{
    bool value = request->arg("value") == "true";
     modeHandler->lightSwitch(value);
    request->send(200);

    if (request->hasArg("save"))
    {
        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences[LIGHT_SWITCH_JSON] = value;
        SavePreferences(&preferences);
    }
}