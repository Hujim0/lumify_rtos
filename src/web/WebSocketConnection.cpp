#include <WebSocketConnection.h>

#define TAG "WebSocket"

void WebSocketConnection::OnNewEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
                                     void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:

        log_i(TAG "Client #%i connected from %s",
              (int)client->id(),
              client->remoteIP().toString().c_str());


        if (OnNewClient != NULL)
            OnNewClient(client);

        break;
    case WS_EVT_DISCONNECT:
        log_i(TAG "Client #%i disconnected",
              (int)client->id());

        server->cleanupClients();

        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len, server);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void WebSocketConnection::handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocket *socket) const
{
    if (OnNewMessage == NULL)
        return;

    auto *info = static_cast<AwsFrameInfo *>(arg);
    if (info->opcode != WS_TEXT)
        return;

    data[len] = 0;
#ifdef SERIAL_WEBSOCKET
    log_i(TAG(char *) data);
#endif

    OnNewMessage((char *)data);
}

WebSocketConnection::WebSocketConnection(AsyncWebServer *server, const char *uri)
{
    webSocket = new AsyncWebSocket(uri);

    webSocket->onEvent(
        [this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len)
        { OnNewEvent(server, client, type, arg, data, len); });

    server->addHandler(webSocket);

    ownerServer = server;
}

WebSocketConnection::~WebSocketConnection()
{
    webSocket->closeAll();

    ownerServer->removeHandler(webSocket);

    delete webSocket;
}