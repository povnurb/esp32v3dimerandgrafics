long lastWSend = 0;

// declaración de funciones
void ProcessRequest(AsyncWebSocketClient *client, String request);

//-----------------------------------------------------------------------------
// funcion predefinida en la libreria de WebSockets
// Eventos del websocket
//--------------------------------------------------------------------
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    if (type == WS_EVT_CONNECT)
    {
        Serial.printf("[ INFO ] ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("{\"ClientID\": %u }", client->id());
        client->ping();
    }
    else if (type == WS_EVT_DISCONNECT)
    {
        // Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
    }
    else if (type == WS_EVT_ERROR)
    {
        log("ERROR", "websockets.hpp", "Errores en websockets -24");
        // Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    }
    else if (type == WS_EVT_PONG)
    {
        // Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
    }
    else if (type == WS_EVT_DATA)
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len)
        {
            if (info->opcode == WS_TEXT)
            {
                for (size_t i = 0; i < info->len; i++)
                {
                    msg += (char)data[i];
                }
            }
            else
            {
                char buff[3];
                for (size_t i = 0; i < info->len; i++)
                {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
            if (info->opcode == WS_TEXT)
                ProcessRequest(client, msg);
        }
        else
        {
            // message is comprised of multiple frames or the frame is split into multiple packets
            if (info->opcode == WS_TEXT)
            {
                for (size_t i = 0; i < len; i++)
                {
                    msg += (char)data[i];
                }
            }
            else
            {
                char buff[3];
                for (size_t i = 0; i < len; i++)
                {
                    sprintf(buff, "%02x ", (uint8_t)data[i]);
                    msg += buff;
                }
            }
            Serial.printf("%s\n", msg.c_str());

            if ((info->index + len) == info->len)
            {
                if (info->final)
                {
                    if (info->message_opcode == WS_TEXT)
                        ProcessRequest(client, msg);
                }
            }
        }
    }
}
// -------------------------------------------------------------------
// Manejador de ordenes que llegan por Websocket
// -------------------------------------------------------------------
void ProcessRequest(AsyncWebSocketClient *client, String request)
{

    String command = request;
    command.trim(); // limpia espacios

    if (strcmp(command.c_str(), "restore") == 0)
    {
        log("INFO", "websockets.hpp", "Commando restaurar por WS => " + command);
    }
    else if (strcmp(command.c_str(), "restart") == 0)
    {
        log("INFO", "websockets.hpp", "Commando reiniciar por WS => " + command);
    }
    else
    {
        OnOffRelays(command); // verificar si esta correcto
    }
}
// -------------------------------------------------------------------
// Inicializar el Websocket
// -------------------------------------------------------------------
void initWebsockets()
{
    ws.onEvent(onWsEvent);
    server.addHandler(&ws); // puntero de websockets
    log("INFO", "websockets.hpp", "Servidor WebSocket iniciado");
}
// -------------------------------------------------------------------
// Función enviar JSON por Websocket
// -------------------------------------------------------------------
void wsMessageSend(String msg, String icon, String Type)
{
    if (strcmp(Type.c_str(), "info") == 0)
    {
        String response;
        DynamicJsonDocument doc(300);
        doc["type"] = Type;
        doc["msg"] = msg;
        doc["icon"] = icon;
        serializeJson(doc, response);
        ws.textAll(response);
    }
    else
    {
        ws.textAll(msg);
    }
}
// -------------------------------------------------------------------
// Empaquetar el JSON para enviar por WS Datos para Index cada 1s
// -------------------------------------------------------------------
String getJsonIndex()
{
    String response = "";
    DynamicJsonDocument jsonDoc(3000);
    jsonDoc["type"] = "data";
    jsonDoc["activeTime"] = longTimeStr(millis() / 1000);
    // jsonDoc["ramAvailable"] = ESP.getFreeHeap();
    // jsonDoc["RAM_SIZE_KB"] = ESP.getHeapSize();
    jsonDoc["mqttStatus"] = mqttClient.connected() ? true : false;
    jsonDoc["mqtt_server"] = mqttClient.connected() ? F(mqtt_server) : F("server not connected");
    jsonDoc["wifiStatus"] = WiFi.status() == WL_CONNECTED ? true : false;
    jsonDoc["rssiStatus"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
    jsonDoc["wifiQuality"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;
    jsonDoc["R_STATUS1"] = R_STATUS1 ? true : false;
    jsonDoc["R_STATUS2"] = R_STATUS2 ? true : false;
    jsonDoc["dimmer"] = dim;
    // jsonDoc["cpuTemp"] = TempCPUValue();
    jsonDoc["tC"] = String(Temperatura());
    jsonDoc["hum"] = String(Humedad());
    jsonDoc["tmin"] = String(tempMin());
    jsonDoc["tmax"] = String(tempMax());
    jsonDoc["BUZZER_STATUS"] = BUZZER_STATUS ? true : false;
    jsonDoc["ALRM_NAME1"] = String(ALRM_NAME1);
    jsonDoc["ALRM_STATUS1"] = digitalRead(ALARM_PIN1) ? true : false;
    jsonDoc["ALRM_TON1"] = String(ALRM_TON1);
    jsonDoc["ALRM_TOFF1"] = String(ALRM_TOFF1);
    jsonDoc["ALRM_NAME2"] = String(ALRM_NAME2);
    jsonDoc["ALRM_STATUS2"] = digitalRead(ALARM_PIN2) ? true : false;
    jsonDoc["ALRM_TON2"] = String(ALRM_TON2);
    jsonDoc["ALRM_TOFF2"] = String(ALRM_TOFF2);
    jsonDoc["ALRM_NAME3"] = String(ALRM_NAME3);
    jsonDoc["ALRM_STATUS3"] = digitalRead(ALARM_PIN3) ? true : false;
    jsonDoc["ALRM_TON3"] = String(ALRM_TON3);
    jsonDoc["ALRM_TOFF3"] = String(ALRM_TOFF3);
    jsonDoc["ALRM_NAME4"] = String(ALRM_NAME4);
    jsonDoc["ALRM_STATUS4"] = digitalRead(CLIMAA) ? true : false;
    jsonDoc["ALRM_TON4"] = String(ALRM_TON4);
    jsonDoc["ALRM_TOFF4"] = String(ALRM_TOFF4);
    jsonDoc["ALRM_NAME5"] = String(ALRM_NAME5);
    jsonDoc["ALRM_STATUS5"] = digitalRead(CLIMAB) ? true : false;
    jsonDoc["ALRM_TON5"] = String(ALRM_TON5);
    jsonDoc["ALRM_TOFF5"] = String(ALRM_TOFF5);
    serializeJson(jsonDoc, response);
    return response;
}
