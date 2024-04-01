#pragma once

#include <WiFi.h>

#include <utility>

#define TAG "WifiManager "

#define WIFI_EVENT "wifi_event"

#define TICKS_TO_WAIT 1

typedef std::function<void()> OnConnectionLostHandler;
typedef std::function<void()> OnConnectionSuccessfulHandler;

struct WifiCredentials
{
    String ssid;
    String pw;

    WifiCredentials(String stringSSID, String stringPW)
    :ssid(std::move(stringSSID))
    , pw(std::move(stringPW))
    {
    }

    WifiCredentials() = default;
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
    static wl_status_t trySTA(WifiCredentials);

    static void createConnectionTask(WifiCredentials *creds);

    static void raiseWifiEvent(wl_status_t res);

    static void connectionTask(void *pvParameters);

    static void startAP();
};
