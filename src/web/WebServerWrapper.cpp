#include <WebServerWrapper.h>
#include <WifiManager.h>
#include <LITTLEFS.h>

WebServerWrapper::WebServerWrapper(const char *DNSHostName, int port)
{
    server = new AsyncWebServer(port);
}

WebServerWrapper::~WebServerWrapper()
{
    delete server;
}

void WebServerWrapper::beginServerTask()
{
    server->begin();

    server->serveStatic("/", LittleFS, "/public");
}
