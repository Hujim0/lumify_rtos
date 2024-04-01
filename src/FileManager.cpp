#include <FileManager.h>
#include <WifiManager.h>

void FileManager::begin()
{
    LittleFS.begin(FORMAT_LittleFS_IF_FAILED, "");
}

DynamicJsonDocument *FileManager::loadJsonFromFile(const char *path)
{
    File file = LittleFS.open(path, "r");

    DynamicJsonDocument *doc = new DynamicJsonDocument(1024);

    const char *str = file.readString().c_str();

    deserializeJson(*doc, str);

    return doc;
}

esp_err_t FileManager::writeToWifiCredentialsFromFile(WifiCredentials *credentials) {
    if (!LittleFS.exists(CREDENTIALS_PATH)) {
        return ESP_ERR_NOT_FOUND;
    }

    File file = LittleFS.open(CREDENTIALS_PATH, "r", false);

    int emptyLinesCount = 0;

    while (credentials->ssid.isEmpty() || credentials->pw.isEmpty())
    {
        emptyLinesCount += 1;

        String line = file.readStringUntil('\n');
        line.trim();

        if (emptyLinesCount >= 10) {
            log_e("Credentials file is empty!");
            return ESP_ERR_NOT_FOUND;
        }

        if (line.charAt(0) == ';' || line.isEmpty()) {
            continue;
        }

        if (credentials->ssid.isEmpty()) {
            credentials->ssid = line;
        }
        else if (credentials->pw.isEmpty()) {
            credentials->pw = line;
        }
        else {
            break;
        }
    }
    return ESP_OK;
}

std::unique_ptr<String> FileManager::getModeArguments(int id)
{
    char path[64];
    snprintf(path, sizeof(path), MODE_ARGS_PATH, id);

    if (!LittleFS.exists(path))
        return nullptr;

    File file = LittleFS.open(path, "r");

    std::unique_ptr<String> string(new String(file.readString()));
    file.close();

    return string;
}

void FileManager::saveModeArguments(int id, std::unique_ptr<String> str) {
    char path[64];
    snprintf(path, sizeof(path), MODE_ARGS_PATH, id);

    File file = LittleFS.open(path, "r");
    file.print(*str);
    file.close();
}

std::unique_ptr<String> FileManager::loadPreferencesString()
{
    if (!LittleFS.exists(PREFERENCES_PATH))
        return nullptr;

    File file = LittleFS.open(PREFERENCES_PATH, "r");

    std::unique_ptr<String> string(new String(file.readString()));
    file.close();
    return string;
}

std::unique_ptr<DynamicJsonDocument> FileManager::loadPreferencesDocument()
{
    if (!LittleFS.exists(PREFERENCES_PATH))
        return nullptr;

    File file = LittleFS.open(PREFERENCES_PATH, "r");

    std::unique_ptr<DynamicJsonDocument> jsonDocument(new DynamicJsonDocument(512));

    auto err = deserializeJson(*jsonDocument, file);

    if (err) {
        log_e("%s", err.c_str());
        return nullptr;
    }

    file.close();
    return jsonDocument;
}

std::unique_ptr<DynamicJsonDocument> FileManager::getModeArgumentsJSON(int id) {
    char path[64];
    snprintf(path, sizeof(path), MODE_ARGS_PATH, id);

    if (!LittleFS.exists(path))
        return nullptr;

    File file = LittleFS.open(path, "r");

    std::unique_ptr<DynamicJsonDocument> jsonDocument(new DynamicJsonDocument(512));
    auto err = deserializeJson(*jsonDocument, file);
    file.close();

    if (err) {
        log_e("%s", err.c_str());
        return nullptr;
    }

    return jsonDocument;

}

void FileManager::savePreferencesDocument(std::unique_ptr<DynamicJsonDocument> doc) {
    File file = LittleFS.open(PREFERENCES_PATH, "w");

    serializeJson(*doc, file);
    file.close();
}

void FileManager::saveModeArguments(int id, const JsonVariant &var) {
    char path[64];
    snprintf(path, sizeof(path), MODE_ARGS_PATH, id);
    File file = LittleFS.open(path, "r");
    serializeJson(var, file);
    file.close();
}
