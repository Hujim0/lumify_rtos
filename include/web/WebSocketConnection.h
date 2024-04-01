#pragma once

#include <AsyncWebSocket.h>

typedef std::function<void(const char *)> OnNewMessageHandler;
typedef std::function<void(AsyncWebSocketClient *)> OnNewClientHandler;

class WebSocketConnection
{
private:
    AsyncWebServer *ownerServer;

    void OnNewEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                    void *arg, uint8_t *data, size_t len);

    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocket *socket) const;

    // String buffer;
    // uint64_t buffer_size;

public:
    OnNewMessageHandler OnNewMessage;
    OnNewClientHandler OnNewClient;

    AsyncWebSocket *webSocket;

    WebSocketConnection(AsyncWebServer *server, const char *uri);

    ~WebSocketConnection();
};