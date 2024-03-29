#pragma once

#include <ESPAsyncWebServer.h>

class WebServerWrapper
{
public:
    AsyncWebServer *server;

    WebServerWrapper(const char *DNSHostName, int port);
    ~WebServerWrapper();

    void beginServerTask();

    void addLegacyHandlers();
};