#pragma once

#include <WifiManager.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

#define SPIFFS LITTLEFS

class FileManager
{
public:
    static void begin();

    static WifiCredentials *createWifiCredentialsFromFile();

    static DynamicJsonDocument *loadJsonFromFile(const char *path);
};

struct IOException
{
};