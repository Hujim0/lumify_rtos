#pragma once

#include <WifiManager.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define SPIFFS LITTLEFS
#define FORMAT_LittleFS_IF_FAILED false

#define PREFERENCES_PATH "/public/preferences.json"
#define CREDENTIALS_PATH "/credentials.txt"
#define MODE_ARGS_PATH "/public/modes/mode%i.json"


class FileManager
{
public:
    static void begin();

    static esp_err_t writeToWifiCredentialsFromFile(WifiCredentials *);

    static std::unique_ptr<String> getModeArguments(int id);
    static std::unique_ptr<DynamicJsonDocument> getModeArgumentsJSON(int id);
    static void saveModeArguments(int id, std::unique_ptr<String> str);
    static void saveModeArguments(int id, const JsonVariant &var);

    static std::unique_ptr<DynamicJsonDocument> loadPreferencesDocument();
    static void savePreferencesDocument(std::unique_ptr<DynamicJsonDocument> doc);

    static std::unique_ptr<String> loadPreferencesString();
    static DynamicJsonDocument *loadJsonFromFile(const char *path);

};
