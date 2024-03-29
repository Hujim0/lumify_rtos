#pragma once

#include <WiFi.h>

#define TAG "WifiManager "

#define WIFI_EVENT "wifi_event"

#define TICKS_TO_WAIT 1

typedef std::function<void()> OnConnectionLostHandler;
typedef std::function<void()> OnConnectionSuccessfulHandler;

#define SSID_AND_PASSWORD_LENGTH 32

struct WifiCredentials
{
    char *ssid;
    char *pw;

    WifiCredentials(const String &stringSSID, const String &stringPW)
    {
        ssid = new char[32];
        pw = new char[32];

        strncpy(ssid, stringSSID.c_str(), SSID_AND_PASSWORD_LENGTH);
        strncpy(pw, stringPW.c_str(), SSID_AND_PASSWORD_LENGTH);
    }

    ~WifiCredentials()
    {
        delete ssid;
        delete pw;
    }
};

const char wl_status_tToString[7][20] =
    {
        "WL_IDLE_STATUS",
        "WL_NO_SSID_AVAIL",
        "WL_SCAN_COMPLETED",
        "WL_CONNECTED",
        "WL_CONNECT_FAILED",
        "WL_CONNECTION_LOST",
        "WL_DISCONNECTED"};

class WifiManager
{
public:
    static wl_status_t trySTA(const WifiCredentials &);

    static void createConnectionTask(WifiCredentials *creds);

    static void raiseWifiEvent(wl_status_t res);

    static void connectionTask(void *pvParameters);

    static void startAP();
};
