#include <global.h>
#include <FileSystemLegacy.h>
#include <LittleFS.h>
#include <WebServerWrapper.h>
#include <AsyncJson.h>

#include <WebSocketConnection.h>
#include <ModeHandler.h>
#include <FastLED.h>
#include <main.h>

#include <LittleFS.h>

#define currentLanguage "en"

#define AddWebPageHandler(uri, func) server->on(uri, HTTP_GET, func);

#define AddJSONBodyHandler(uri, func) server->addHandler(new AsyncCallbackJsonWebHandler(uri, func));

void getLocalization(AsyncWebServerRequest *request);
void homeRedirect(AsyncWebServerRequest *request);
void getModeArguments(AsyncWebServerRequest *request);
void getTimeEvents(AsyncWebServerRequest *request, JsonVariant &json);
void onPostModeArguments(AsyncWebServerRequest *request, JsonVariant &json);
void getElements(AsyncWebServerRequest *request);
void changeLanguage(AsyncWebServerRequest *request);
void lightSwitch(AsyncWebServerRequest *request);
void changeBrightness(AsyncWebServerRequest *request);

void WebServerWrapper::addLegacyHandlers()
{
    WebSocketConnection *connection = new WebSocketConnection(server, "/ws");
    connection->OnNewMessage = handleWebSocket;

    AsyncEventSource *events = new AsyncEventSource("/events");

    server->addHandler(events);

    // // global
    AddWebPageHandler("/", &homeRedirect);

    // AddWebPageHandler("/favicon.ico", [](AsyncWebServerRequest *request)
    //                   { request->send(
    //                         request->beginResponse(LittleFS, "/public/web/favicon.ico", "image/x-icon")); });

    AddWebPageHandler("/localization", &getLocalization);

    AddWebPageHandler("/mode", &getModeArguments);
    AddJSONBodyHandler("/time_events", &getTimeEvents);

    AddJSONBodyHandler("/mode", &onPostModeArguments);

    AddWebPageHandler("/elements", &getElements);

    AddWebPageHandler("/preferences", [](AsyncWebServerRequest *request)
                      { request->send(
                            request->beginResponse(LittleFS, "/public/preferences.json", "text/json")); });

    AddWebPageHandler("/changelang", &changeLanguage);

    AddWebPageHandler("/log", [](AsyncWebServerRequest *request)
                      { request->send(500); });

    AddWebPageHandler("/time", [](AsyncWebServerRequest *request)
                      {
    // StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
    // deserializeJson(preferences, LoadPreferences());

    // //   timeManager.Setup(
    // //     request->arg("epoch").toInt(),
    // //     request->arg("dayoftheweek").toInt(),
    // //     preferences);
    // //   _log.gotTime = true;

    // SavePreferences(&preferences);

    request->send(500); });

    AddWebPageHandler("/brightness", &changeBrightness);

    AddWebPageHandler("/light_switch", &lightSwitch);

    AddWebPageHandler("/fullreset", [](AsyncWebServerRequest *request)
                      {
    SaveWifiCredentials("", "");
    request->send(200);
    delay(100);
    ESP.restart(); });

    //====================================================

    // pages
    AddWebPageHandler("/en/home", [](AsyncWebServerRequest *request)
                      { request->send(
                            request->beginResponse(
                                LittleFS,
                                "/public/web/home/home.html",
                                "text/html")); });
    AddWebPageHandler("/ru/home", [](AsyncWebServerRequest *request)
                      { request->send(
                            request->beginResponse(
                                LittleFS,
                                "/public/web/home/home_ru.html",
                                "text/html")); });

    //------------------------------------------------------
    AddWebPageHandler("/en/schedule", [](AsyncWebServerRequest *request)
                      { request->send(
                            request->beginResponse(
                                LittleFS,
                                "/public/web/schedule/schedule.html",
                                "text/html")); });
    AddWebPageHandler("/ru/schedule", [](AsyncWebServerRequest *request)
                      { request->send(
                            request->beginResponse(
                                LittleFS,
                                "/public/web/schedule/schedule_ru.html",
                                "text/html")); });
}

void getLocalization(AsyncWebServerRequest *request)
{
    char uri[64];
    snprintf(uri,
             sizeof(uri),
             "/public/modes/elements/%s/localization.json",
             currentLanguage);

    request->send(
        request->beginResponse(
            LittleFS,
            uri,
            "text/json"));
}

void homeRedirect(AsyncWebServerRequest *request)
{
    char uri[64];

    snprintf(uri, sizeof(uri), "/%s/home", currentLanguage);

    request->redirect(uri);
}

void getModeArguments(AsyncWebServerRequest *request)
{

    int id = request->arg("id").toInt();

    char args_string[MAX_ARGS_LENGTH] = "";

    {
        String args = GetModeArgs(id);

        strncpy(args_string, args.c_str(), sizeof(args_string));
        args[0] = 0;
    }

    if (strcmp(args_string, "") == 0)
    {
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "[ERROR] Invalid mode id: %i", id);
            sprintln(msg);
        }

        request->send(404);
        return;
    }

    request->send(
        request->beginResponse(
            HTTP_POST,
            "text/json",
            args_string));

    if (request->hasArg("change"))
    {
        // modeHandler.changeMode(id, args_string);
        esp_event_post(CHANGE_MODE_EVENT, id, new String(GetModeArgs(id).c_str()), sizeof(&args_string), 0);
        log_i("posted, id: %i", id);
    }
    if (request->hasArg("save"))
    {
        SaveModeArgs(id, args_string);

        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences["mode"] = id;
        SavePreferences(&preferences);
    }
}

void getTimeEvents(AsyncWebServerRequest *request, JsonVariant &json)
{
    // {
    //   String json_string;
    //   serializeJson(json, json_string);
    //   SaveTimeEvents(json_string.c_str());
    //   json_string[0] = 0;
    // }

    // {
    //   // timeManager.AddTimeEventsFromJson(json);

    //   json.clear();
    // }

    request->send(500);
}

void onPostModeArguments(AsyncWebServerRequest *request, JsonVariant &json)
{
    request->send(200);

    int id = request->arg("id").toInt();

    char args_string[MAX_ARGS_LENGTH] = "";

    {
        String _json_string((char *)0);
        serializeJson(json, _json_string);
        json.clear();
        strncpy(args_string, _json_string.c_str(), sizeof(args_string));
        _json_string[0] = 0;
    }

    if (request->hasArg("save"))
    {
        SaveModeArgs(id, args_string);
    }

    // modeHandler.changeMode(id, json_string);
//    esp_event_post(CHANGE_MODE_EVENT, id, &args_string, sizeof(&args_string), 0);

    if (request->hasArg("save"))
    {
        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences["mode"] = id;
        SavePreferences(&preferences);
    }
}
void getElements(AsyncWebServerRequest *request)
{
    char path[64] = "";
    {
        String path_string = GetElementsFilePath(currentLanguage, request->arg("id").c_str());
        strncpy(path, path_string.c_str(), sizeof(path));
        path_string[0] = 0;
    }

    if (!FileExists(path))
    {
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "[ERROR] Not found elements file! id: %s", request->arg("id").c_str());
            sprintln(msg);
        }

        request->send(404);
        return;
    }

    request->send(
        request->beginResponse(LittleFS, path, "text/json"));
}

void changeLanguage(AsyncWebServerRequest *request)
{
    char lang[10] = "";
    {
        strncpy(lang, request->arg("lang").c_str(), sizeof(lang));
        // ChangeLanguage(lang);
    }

    char uri[32] = "";

    snprintf(uri, sizeof(uri), "/%s/home", lang);

    request->redirect(uri);
}

void lightSwitch(AsyncWebServerRequest *request)
{
    bool value = request->arg("value") == "true";
    // modeHandler.lightSwitch(value);
    esp_event_post(CHANGE_MODE_EVENT, value ? 0 : 1, NULL, 0, 0);
    request->send(200);

    if (request->hasArg("save"))
    {
        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences[LIGHT_SWITCH_JSON] = value;
        SavePreferences(&preferences);
    }
}

void changeBrightness(AsyncWebServerRequest *request)
{
    int value = request->arg("value").toInt();
    FastLED.setBrightness(value);

    if (request->hasArg("save"))
    {
        StaticJsonDocument<STATIC_DOCUMENT_MEMORY_SIZE> preferences;
        deserializeJson(preferences, LoadPreferences());
        preferences[BRIGHTNESS_JSON] = value;
        SavePreferences(&preferences);
    }
    request->send(200);
}