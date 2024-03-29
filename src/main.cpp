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
  modeHandler->startFastLEDSetupTask();
  modeHandler->changeMode(0, GetModeArgs(0).c_str());
  modeHandler->startUpdateTask();

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