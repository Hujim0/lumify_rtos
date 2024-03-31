#include <WifiManager.h>

#define MAX_ATTEMPT_COUNT 5

#define DEBUG_WIFI_SETTINGS

/**
 * Tries 5 times to connect to wifi.
 *
 * @return WL_CONNECTED - connected; WL_NO_SSID_AVAIL - returns after first attempt, wifi doesn't exists.
 */
wl_status_t WifiManager::trySTA(const WifiCredentials &creds)
{
    log_i(TAG "Wifi credentials: %s %s, Connecting to Wifi... ", creds.ssid, creds.pw);

    WiFi.mode(WiFiMode_t::WIFI_STA);

    wl_status_t status = wl_status_t::WL_IDLE_STATUS;

    int attempt = 0;

    WiFi.begin(creds.ssid, creds.pw);

    while (attempt <= MAX_ATTEMPT_COUNT)
    {
        status = (wl_status_t)WiFi.waitForConnectResult();

        log_v(TAG "Attempt %i: %s", attempt, wl_status_tToString[status]);

        if (attempt == MAX_ATTEMPT_COUNT)
        {
            return status;
        }

        switch (status)
        {
        case WL_CONNECTED:
        {
            log_i(TAG "success!");

            return status;
        }
        case WL_NO_SSID_AVAIL:
        {
            log_i(TAG "Wrong SSID!!!");
            return status;
        }
        default:
        {
            attempt += 1;

            WiFi.reconnect();
        }
        }
    }

    return status;
}

void WifiManager::connectionTask(void *pvParameters)
{
    WifiCredentials *creds = static_cast<WifiCredentials *>(pvParameters);

    wl_status_t res = WifiManager::trySTA(*creds);

    delete creds;

    WifiManager::raiseWifiEvent(res);

    vTaskDelete(nullptr);
}

void WifiManager::startAP()
{
    log_i("Not implemented...");
}

void WifiManager::createConnectionTask(WifiCredentials *creds)
{
    xTaskCreatePinnedToCore(
        &connectionTask,
        "wifi_connection",
        10000,
        creds,
        1,
        nullptr,
        NETWORK_RUNNING_CORE);
}

void WifiManager::raiseWifiEvent(wl_status_t res)
{
    esp_event_post(WIFI_EVENT, res, NULL, 0, TICKS_TO_WAIT);
}