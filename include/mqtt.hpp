#include <PubSubClient.h>
// #define MQTT_MAX_PACKET_SIZE 1024*5 //otra manera de aumentar el buffer pero da un warning
//  inicia en video 49 y 50
WiFiClient wifiClient;               // este es el puntero a la conexión mqtt
PubSubClient mqttClient(wifiClient); // se define el cliente mqtt sobre la conexión wifi

// Variables

char topic[150];
String topic_subscribe;
String Json();
void callback(char *topic, byte *payload, unsigned int length);
char topicExpecial[150]; // se requiere un topic para recibir informacaion de manera remota
String topic_subscribeEsp;
// variable que guarda la data para ser enviadad por MQTT
String mqtt_data = "";

// los tiempos para intentar la reconexión a MQTT
long lastMqttReconnectAttempt = 0;

// tiempo para enviar el mensaje (publicar)
long lasMsg = 0;

//----------------------------------------------------------------------
// MQTT Connect
// ---------------------------------------------------------------------
bool mqtt_connect()
{
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(callback);
    log("INFO", "mqtt.hpp", "Intentando conexión al Broker MQTT ...");

    String topic_publish = PathMqttTopic("status"); // publicara
    topic_publish.toCharArray(mqtt_willTopic, 150); // will significa mensaje que ultima voluntad

    // https://pubsubclient.knolleary.net/api.html
    // boolean connect (clientID, [username, password], [willTopic, willQoS, willRetain, willMessage], [cleanSession])
    if (mqttClient.connect(mqtt_id, mqtt_user, mqtt_password, mqtt_willTopic, mqtt_willQoS, mqtt_willRetain, mqtt_willMessage, mqtt_clean_sessions))
    {
        log("INFO", "mqtt.hpp", "Conectado al Broker MQTT");

        mqttClient.setBufferSize(1024 * 5); // ampliar el buffer de 256 a 5KB
        log("INFO", "mqtt.hpp", "Buffer MQTT Size: " + String(mqttClient.getBufferSize()) + " Bytes");
        // tambien se puede modificar en la libreria en el path
        //.pio/libdeps/esp32dev/PubSubClient/src/PubSubClient.h #define MQTT_MAX_PACKET_SIZE 256 => 1024*5
        topic_subscribe = PathMqttTopic("command"); // donde llegan los comando
        topic_subscribe.toCharArray(topic, 150);
        if (mqttClient.subscribe(topic, mqtt_qos))
        {
            log("INFO", "mqtt.hpp", "Suscrito: " + String(topic));
        }
        else
        {
            log("ERROR", "mqtt.hpp", "MQTT - Falló suscripción: " + String(topic));
        }
        // se genera el path de especial y se mete a un String llamado topic_subscribeEsp
        topic_subscribeEsp = PathMqttTopic("especial"); // donde se modifica los nombres de las alarmas entre otras cosas
        // la siguiente funcion lo convierte en un char Array
        topic_subscribeEsp.toCharArray(topicExpecial, 150);
        if (mqttClient.subscribe(topicExpecial, mqtt_qos))
        {
            log("INFO", "mqtt.hpp", "Suscrito: " + String(topicExpecial));
        }
        else
        {
            log("ERROR", "mqtt.hpp", "MQTT - Falló suscripción: " + String(topicExpecial));
        }

        // si esta permitido publicar
        if (mqtt_status_send)
        {
            mqttClient.publish(mqtt_willTopic, "{\"connected\": true}", mqtt_retain);
        }
    }
    else
    {
        log("ERROR", "mqtt.hpp", "MQTT - Falló, código de error = " + String(mqttClient.state()));
        /**
         * -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
         * -3 : MQTT_CONNECTION_LOST - the network connection was broken
         * -2 : MQTT_CONNECT_FAILED - the network connection failed
         * -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
         * 0 : MQTT_CONNECTED - the client is connected
         * 1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
         * 2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
         * 3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
         * 4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
         * 5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
         */
        return (0); // esto es igual a false
    }
    return (1); // esto es igual a true
}

// -----------------------------------------------------------------------------------
// Manejo de los mensajes Entrantes
// -----------------------------------------------------------------------------------
void callback(char *topic, byte *payload, unsigned int length)
{
    String command = "";
    String str_topic(topic);
    for (int16_t i = 0; i < length; i++)
    {
        command += (char)payload[i];
        // TODO: pestañeo de led MQTT
    }
    // metodo trim() que limpia los espacion
    command.trim();
    log("INFO", "mqtt.hpp", "MQTT Tópico  --> " + str_topic);
    log("INFO", "mqtt.hpp", "MQTT Mensaje --> " + command);

    if (str_topic == topic_subscribe)
    {
        log("INFO", "mqtt.hpp", "Comando para los Actuadores");
        OnOffRelays(command);
    }
    if (str_topic == topic_subscribeEsp)
    {
        log("INFO", "mqtt.hpp", "Modificacion Especial de valores");
        especial(command);
    }
    else
    {
        log("INFO", "mqtt.hpp", "Muestra información de otro dispositivo");
        muestraInfoMqtt(command);
    }
    // TODO: responder al mqtt el estado de los relays
}

// -----------------------------------------------------------------------------------
// Manejo de los mensajes Salientes
// -----------------------------------------------------------------------------------
void mqtt_publish()
{
    String topic = PathMqttTopic("device");
    log("INFO", "mqtt.hpp", "publicando en: " + topic);
    Serial.flush();
    mqtt_data = Json();
    // c_str() convierte un string en char
    mqttClient.publish(topic.c_str(), mqtt_data.c_str(), mqtt_retain);
    mqtt_data = ""; // limpiamos los valores de mqtt
}
// ----------------------------------------------------------------------------------
// JSON con información del dispositivo para envio por MQTT
// ----------------------------------------------------------------------------------
String Json()
{
    String response;
    DynamicJsonDocument jsonDoc(1024 * 4);
    // Capturar el valor de temperatura
    // readSensor(); //toma lecturas del sensor DS18B20
    // Crear JSON
    jsonDoc["lugar"] = String(LUGAR);
    jsonDoc["serial"] = DeviceID();
    jsonDoc["MACWiFi"] = String(WiFi.macAddress());
    jsonDoc["ip"] = ipStr(WiFi.localIP());
    // jsonDoc["deviceManufacturer"] = String(device_manufacturer);
    // jsonDoc["deviceFwVersion"] = device_fw_version;
    // jsonDoc["deviceHwVersion"] = String(device_hw_version);
    // jsonDoc["deviceSdk"] = String(ESP.getSdkVersion());

    JsonObject dataObj = jsonDoc.createNestedObject("data");
    // dataObj["RAM_Disponible"] = ESP.getFreeHeap();
    // dataObj["RAM_TOTAL"] = ESP.getHeapSize();
    // dataObj["Flas_disponible"] = ???; //aun no se
    dataObj["R_STATUS1"] = R_STATUS1 ? true : false;
    dataObj["R_STATUS2"] = R_STATUS2 ? true : false;
    dataObj["BUZZER_STATUS"] = BUZZER_STATUS ? true : false;
    dataObj["dimmer"] = dim;
    dataObj["cpuTemp"] = String(TempCPUValue());
    // dataObj["deviceDS18B20TempC"] = temperatureC; //se requiere libreria adicional comentada en platformio.ini
    // dataObj["deviceDS18B20TempF"] = temperatureF; //se requiere libreria adicional comentada en platformio.ini
    dataObj["tC"] = String(Temperatura());
    dataObj["hum"] = String(Humedad());
    dataObj["tmin"] = String(tempMin());
    dataObj["tmax"] = String(tempMax());
    // dataObj["activeTime"] = longTimeStr(millis() / 1000);
    // dataObj["deviceSpiffsUsed"] = String(SPIFFS.usedBytes());
    // dataObj["deviceCpuClock"] = String(getCpuFrequencyMhz());
    // dataObj["deviceFlashSize"] = String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
    // dataObj["deviceRestartS"] = String(device_restart);
    dataObj["mqttStatus"] = mqttClient.connected() ? true : false;
    dataObj["mqttServer"] = mqttClient.connected() ? F(mqtt_server) : F("server not connected");
    dataObj["wifiStatus"] = WiFi.status() == WL_CONNECTED ? true : false;
    dataObj["wifiRssiStatus"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
    // dataObj["wifiQuality"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;
    //  lo que se puede modificar de manera remota por mqtt
    JsonObject espObj = jsonDoc.createNestedObject("especial");
    espObj["R_NAME1"] = String(R_NAME1);
    espObj["R_NAME2"] = String(R_NAME2);
    espObj["R_LOGIC1"] = R_LOGIC1 ? true : false;
    espObj["R_LOGIC2"] = R_LOGIC2 ? true : false;
    espObj["R_DESCRIPTION1"] = String(R_DESCRIPTION1);
    espObj["R_DESCRIPTION2"] = String(R_DESCRIPTION2);
    espObj["ALRM_LOGIC1"] = ALRM_LOGIC1 ? true : false;
    espObj["ALRM_LOGIC2"] = ALRM_LOGIC2 ? true : false;
    espObj["ALRM_LOGIC3"] = ALRM_LOGIC3 ? true : false;
    espObj["ALRM_LOGIC4"] = ALRM_LOGIC4 ? true : false;
    espObj["ALRM_LOGIC5"] = ALRM_LOGIC5 ? true : false;
    espObj["ALRM_NAME1"] = String(ALRM_NAME1);
    espObj["ALRM_NAME2"] = String(ALRM_NAME2);
    espObj["ALRM_NAME3"] = String(ALRM_NAME3);
    espObj["ALRM_NAME4"] = String(ALRM_NAME4);
    espObj["ALRM_NAME5"] = String(ALRM_NAME5);
    espObj["ALRM_STATUS1"] = ALRM_LOGIC1 ? (digitalRead(ALRM_PIN1) ? true : false) : (digitalRead(ALRM_PIN1) ? false : true);
    espObj["ALRM_STATUS2"] = ALRM_LOGIC2 ? (digitalRead(ALRM_PIN2) ? true : false) : (digitalRead(ALRM_PIN2) ? false : true);
    espObj["ALRM_STATUS3"] = ALRM_LOGIC3 ? (digitalRead(ALRM_PIN3) ? true : false) : (digitalRead(ALRM_PIN3) ? false : true);
    espObj["ALRM_STATUS4"] = ALRM_LOGIC4 ? (digitalRead(ALRM_PIN4) ? true : false) : (digitalRead(ALRM_PIN4) ? false : true);
    espObj["ALRM_STATUS5"] = ALRM_LOGIC5 ? (digitalRead(ALRM_PIN5) ? true : false) : (digitalRead(ALRM_PIN5) ? false : true);
    // falta por enviar cosas para mi que este no se manda con remote si no se cacha
    espObj["ALRM_TON1"] = ALRM_TON1;
    espObj["ALRM_TON2"] = ALRM_TON2;
    espObj["ALRM_TON3"] = ALRM_TON3;
    espObj["ALRM_TON4"] = ALRM_TON4;
    espObj["ALRM_TON5"] = ALRM_TON5;

    espObj["ALRM_TOFF1"] = ALRM_TOFF1;
    espObj["ALRM_TOFF2"] = ALRM_TOFF2;
    espObj["ALRM_TOFF3"] = ALRM_TOFF3;
    espObj["ALRM_TOFF4"] = ALRM_TOFF4;
    espObj["ALRM_TOFF5"] = ALRM_TOFF5;

    espObj["ALRM_CONT1"] = ALRM_CONT1;
    espObj["ALRM_CONT2"] = ALRM_CONT2;
    espObj["ALRM_CONT3"] = ALRM_CONT3;
    espObj["ALRM_CONT4"] = ALRM_CONT4;
    espObj["ALRM_CONT5"] = ALRM_CONT5;

    JsonObject r1Obj = jsonDoc.createNestedObject("timerR1");
    r1Obj["R_TIMERON1"] = R_TIMERON1 ? true : false;
    r1Obj["R_TIMER1"] = String(R_TIMER1);
    r1Obj["TEMPORIZADOR1"] = TEMPORIZADOR1 ? true : false;
    r1Obj["TIMEONRELAY1"] = String(TIMEONRELAY1);
    r1Obj["TIMEOFFRELAY1"] = String(TIMEOFFRELAY1);
    JsonObject r2Obj = jsonDoc.createNestedObject("timerR2");
    r2Obj["R_TIMERON2"] = R_TIMERON2 ? true : false;
    r2Obj["R_TIMER2"] = String(R_TIMER2);
    r2Obj["TEMPORIZADOR2"] = TEMPORIZADOR2 ? true : false;
    r2Obj["TIMEONRELAY2"] = String(TIMEONRELAY2);
    r2Obj["TIMEOFFRELAY2"] = String(TIMEOFFRELAY2);
    serializeJson(jsonDoc, response);
    return response;
}

//----------------------------------------------------------------------
// MQTT Loop Principal se encarga de mantener viva conexión MQTT
// ---------------------------------------------------------------------

void mqttloop()
{
    if (mqtt_enable)
    {
        if (!mqttClient.connected())
        {
            long now = millis();
            if (now < 60000 || ((now - lastMqttReconnectAttempt) > 120000))
            {
                lastMqttReconnectAttempt = now;
                if (mqtt_connect())
                {
                    lastMqttReconnectAttempt = 0;
                }
            }
        }
        else
        {
            mqttClient.loop(); // este metodo permita que siga escuchando
            // la informacion callback por ejemplo
            // escuchando las conexiones
        }
    }
}
//----------------------------------------------------------------------------------------------------------
// parametros de configuraccion Index
// Método: GET
// para la api y MQTT
//  url: /api/index
// Mejor manera de armar un Json y que la otra manera consume muchos recursos
String apiGetIndex(String IPv4, String Browser)
{
    String response = "";
    DynamicJsonDocument jsonDoc(2048);
    // general
    jsonDoc["serial"] = DeviceID();
    jsonDoc["LUGAR"] = LUGAR;
    jsonDoc["device"] = platform();
    jsonDoc["wifiQuality"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;
    jsonDoc["wifiStatus"] = WiFi.status() == WL_CONNECTED ? true : false;
    jsonDoc["rssiStatus"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
    jsonDoc["mqttStatus"] = mqttClient.connected() ? true : false;
    jsonDoc["reboots"] = device_restart;
    jsonDoc["activeTime"] = longTimeStr(millis() / 1000);
    // WIFI
    JsonObject wifiObj = jsonDoc.createNestedObject("wifi");
    wifiObj["Estado_WiFi"] = WiFi.status() == WL_CONNECTED ? "ONLINE" : "OFFLINE";
    wifiObj["SSID_WiFi"] = wifi_mode == WIFI_STA ? wifi_ssid : ap_ssid;
    wifiObj["IPv4_WiFi"] = wifi_mode == WIFI_STA ? ipStr(WiFi.localIP()) : ipStr(WiFi.softAPIP());
    wifiObj["MAC_WiFi"] = WiFi.macAddress();
    // MQTT
    JsonObject mqttObj = jsonDoc.createNestedObject("mqtt");
    mqttObj["Estado_MQTT"] = mqttClient.connected() ? "ONLINE" : "OFFLINE";
    mqttObj["Servidor_MQTT"] = mqttClient.connected() ? mqtt_server : "server not connected";
    mqttObj["Usuario_MQTT"] = mqtt_user;
    mqttObj["Cliente_ID_MQTT"] = mqtt_id;
    // INFO
    JsonObject infoObj = jsonDoc.createNestedObject("info");
    infoObj["Identificación"] = device_name;
    infoObj["Número_de_Serie"] = DeviceID();
    infoObj["mDNS_Url"] = String("http://" + String(device_name) + ".local/");
    infoObj["Dirección_IP_del_Cliente"] = IPv4; // IPv4;
    infoObj["Navegador_del_Cliente"] = Browser; // Browser;
    infoObj["Versión_del_Firmware"] = device_fw_version;
    infoObj["Versión_del_Hardware"] = device_hw_version;
    infoObj["CPU_FREQ_MHz"] = getCpuFrequencyMhz();
    infoObj["RAM_SIZE_KB"] = ESP.getHeapSize() / 1024;
    infoObj["SPIFFS_SIZE_KB"] = SPIFFS.totalBytes() / 1024;
    infoObj["FLASH_SIZE_MB"] = ESP.getFlashChipSize() / (1024.0 * 1024);
    infoObj["Fabricante"] = device_manufacture;
    infoObj["Tiempo_de_Actividad_del_Sistema"] = longTimeStr(millis() / 1000);
    infoObj["Cantidad_de_Reinicios"] = device_restart;
    // GENERAL
    jsonDoc["spiffsUsed"] = SPIFFS.usedBytes() / 1024;
    jsonDoc["ramAvailable"] = ESP.getFreeHeap() / 1024;
    jsonDoc["cpuTemp"] = String(TempCPUValue());
    jsonDoc["tC"] = String(Temperatura());
    jsonDoc["hum"] = String(Humedad());
    jsonDoc["tmin"] = String(tempMin()); // tempMin()
    jsonDoc["tmax"] = String(tempMax());
    jsonDoc["BUZZER_STATUS"] = BUZZER_STATUS;
    // Relays arrays de objetos
    StaticJsonDocument<512> obj1; // Buffer
    StaticJsonDocument<512> obj2; // Buffer
    JsonObject relay1 = obj1.to<JsonObject>();
    relay1["R_NAME1"] = R_NAME1;
    relay1["R_STATUS1"] = R_STATUS1 ? true : false;
    relay1["R_LOGIC1"] = R_LOGIC1;
    relay1["R_DESCRIPTION1"] = R_DESCRIPTION1;
    JsonObject relay2 = obj2.to<JsonObject>();
    relay2["R_NAME2"] = R_NAME2;
    relay2["R_STATUS2"] = R_STATUS2 ? true : false;
    relay2["R_LOGIC2"] = R_LOGIC2;
    relay2["R_DESCRIPTION2"] = R_DESCRIPTION2;

    JsonArray relays = jsonDoc.createNestedArray("relays");
    relays.add(relay1); // se agrega objeto 1 dentro del array
    relays.add(relay2); // se agrega objeto 1 dentro del array
    jsonDoc["dimmer"] = dim;
    serializeJson(jsonDoc, response);
    return response;
}