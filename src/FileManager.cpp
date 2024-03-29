#include <FileManager.h>
#include <WifiManager.h>

#define FORMAT_LittleFS_IF_FAILED false

#define CREDENTIALS_PATH "/credentials.txt"

void FileManager::begin()
{
    LittleFS.begin(FORMAT_LittleFS_IF_FAILED, "");
}

/**
 * Creates a new instance of WifiCredentials form CREDENTIALS_PATH
 * Needs to be deleted.
 *
 * @throws IOExceptions if file doesn't exists/is empty
 */
WifiCredentials *FileManager::createWifiCredentialsFromFile()
{
    if (!LittleFS.exists(CREDENTIALS_PATH))
    {
        throw IOException();
    }

    File file = LittleFS.open(CREDENTIALS_PATH, "r", false);

    int emptyLinesCount = 0;

    String wifiSSID;
    String wifiPassword;

    while (wifiSSID.isEmpty() || wifiPassword.isEmpty())
    {
        emptyLinesCount += 1;

        String line = file.readStringUntil('\n');

        log_i("%s", line);

        if (emptyLinesCount >= 10)
        {
            log_e("Credentials file is empty!");
            throw IOException();
        }

        if (line.charAt(0) == ';' || line.isEmpty())
        {
            continue;
        }

        if (wifiSSID.isEmpty())
        {
            wifiSSID = line;
        }
        else if (wifiPassword.isEmpty())
        {
            wifiPassword = line;
        }
        else
        {
            break;
        }
    }

    wifiSSID.trim();
    wifiPassword.trim();

    return new WifiCredentials(wifiSSID, wifiPassword);
}

DynamicJsonDocument *FileManager::loadJsonFromFile(const char *path)
{
    File file = LittleFS.open(path, "r");

    DynamicJsonDocument *doc = new DynamicJsonDocument(1024);

    const char *str = file.readString().c_str();

    deserializeJson(*doc, str);

    return doc;
}
