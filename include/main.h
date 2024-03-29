#pragma once

#include <WebServerWrapper.h>
#include <WifiManager.h>
#include <FileManager.h>
#include <Arduino.h>
#include <global.h>

// #include <ModeHandler.h>

class Main
{
public:
    WifiManager *wifiManager;

    TaskHandle_t *serverHandler;

    WebServerWrapper *serverWrapper;

    void serverInit();

    void wifiSetup();
};

void handleWebSocket(const char *websocketMessage);