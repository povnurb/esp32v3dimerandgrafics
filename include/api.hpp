//----------------------------------------------------------------------
// Zona API REST manejadores de peticion
//--------------------------------------------------------------------------
// URL:                                 METODO:                ESTADO
//------------------------------------------------------------------------
// /api/index                           GET         ----        OK
// /api/espnow                          GET         ----        OK
// /api/connection/wifi                 GET         ----        OK
// /api/connection/wifi                 POST        ----        OK
// /api/alarmas                         GET         ----        OK
// /api/relays                          GET         ----        OK
// /api/alarmas                         POST        ----        OK
// /api/relays                          POST        ----        OK
// /api/connection/wifi-scan            GET         ----        OK
// /api/connection/mqtt                 GET         ----        OK
// /api/connection/mqtt                 POST        ----        OK
// /api/device/download                 GET         ----        OK
// /api/especial/download               GET         ----        OK
// /api/device/upload                   POST        ----        OK
// /api/especial/upload                 POST        ----        OK
// /api/device/firmware                 POST        ----        OK
// /api/device/status                   GET         ----        OK
// /api/device/restart                  POST        ----        OK
// /api/device/restore                  POST        ----        OK
// /api/perfil/user                     POST        ----        OK
// /api/device/dimmer                   POST        ----        OK
// /api/connection/mqtt2                GET         ----        OK
// /api/connection/alarmasremotas       GET         ----        OK
// /api/connection/alarmasremotas       POST        ----        OK
// /api/time                            GET         ----        OK
// /api/time                            POST        ----        OK
// /api/device/buzzer                   POST        ----

// API v1 json desde String
//  en este archivo se desarrolla las funciones de la API
//  *request es un puntero de la solicitud
bool security = produccion;                // si requiere seguridad
const char *dataType = "application/json"; // para no andarlo poniendo

//---------------------------------------------------------
// Leer parámetros de configuración Index
// url: /api/index
// Método: GET
//--------------------------------------------------------
void handleApiIndex(AsyncWebServerRequest *request)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    String json = "";

    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"LUGAR\": \"" + String(LUGAR) + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"reboots\":" + String(device_restart);
    json += ",\"activeTime\": \"" + String(longTimeStr(millis() / 1000)) + "\"";
    json += ",\"wifi\":{";
    WiFi.status() == WL_CONNECTED ? json += "\"Estado_WiFi\": \"ONLINE\" " : json += "\"Estado_WiFi\": \"OFFLINE\"";
    wifi_mode ? json += ",\"SSID_WiFi\": \"" + String(wifi_ssid) + "\"" : json += ",\"SSID_WiFi\": \"" + String(ap_ssid) + "\"";
    wifi_mode ? json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.localIP()) + "\"" : json += ",\"IPv4_WiFi\": \"" + ipStr(WiFi.softAPIP()) + "\"";
    json += ",\"MAC_WiFi\": \"" + String(WiFi.macAddress()) + "\"";
    json += "},";
    json += "\"mqtt\":{";
    mqttClient.connected() ? json += "\"Estado_MQTT\": \"ONLINE\" " : json += "\"Estado_MQTT\": \"OFFLINE\"";
    mqttClient.connected() ? json += ",\"Servidor_MQTT\": \"" + String(mqtt_server) + "\"" : json += ",\"Servidor_MQTT\": \"server not connected\"";
    json += ",\"Usuario_MQTT\": \"" + String(mqtt_user) + "\"";
    json += ",\"Cliente_ID_MQTT\": \"" + String(mqtt_id) + "\"";
    json += "},";
    json += "\"info\":{";
    json += "\"Identificación\": \"" + String(device_name) + "\"";
    json += ",\"Número_de_Serie\": \"" + DeviceID() + "\"";
    json += ",\"mDNS_Url\":  \"" + String("http://" + String(device_name) + ".local/") + "\"";
    json += ",\"Dirección_IP_del_Cliente\": \"" + ipStr(request->client()->remoteIP()) + "\"";
    json += ",\"Navegador_del_Cliente\": \"" + String(request->getHeader("User-Agent")->value().c_str()) + "\"";
    json += ",\"Versión_del_Firmware\": \"" + String(device_fw_version) + "\"";
    json += ",\"Versión_del_Hardware\": \"" + String(device_hw_version) + "\"";
    json += ",\"CPU_FREQ_MHz\":" + String(getCpuFrequencyMhz());
    json += ",\"RAM_SIZE_KB\":" + String(ESP.getHeapSize() / 1024);
    json += ",\"SPIFFS_SIZE_KB\":" + String(SPIFFS.totalBytes() / 1024);
    json += ",\"FLASH_SIZE_MB\":" + String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
    json += ",\"Fabricante\": \"" + String(device_manufacture) + "\"";
    json += ",\"Tiempo_de_Actividad_del_Sistema\": \"" + String(longTimeStr(millis() / 1000)) + "\"";
    json += ",\"Cantidad_de_Reinicios\":" + String(device_restart);
    json += "}";
    json += ",\"spiffsUsed\":" + String(SPIFFS.usedBytes() / 1024);
    json += ",\"ramAvailable\":" + String(ESP.getFreeHeap() / 1024);
    json += ",\"cpuTemp\":" + String(TempCPUValue());
    isnan(Temperatura()) ? json += ",\"tC\":\"Checar_Sensor\"" : json += ",\"tC\":" + String(Temperatura());
    json += ",\"hum\":" + String(Humedad());
    json += ",\"tmin\":" + String(tempMin()); // tempMin()
    json += ",\"tmax\":" + String(tempMax());
    BUZZER_STATUS ? json += ",\"BUZZER_STATUS\": true" : json += ",\"BUZZER_STATUS\": false";
    json += ",\"relays\":[";
    json += "{";
    json += "\"R_NAME1\": \"" + String(R_NAME1) + "\"";
    R_STATUS1 ? json += ",\"R_STATUS1\": true" : json += ",\"R_STATUS1\": false";
    R_LOGIC1 ? json += ",\"R_LOGIC1\": true" : json += ",\"R_LOGIC1\": false";
    json += ",\"R_DESCRIPTION1\": \"" + String(R_DESCRIPTION1) + "\"";
    json += "},";
    json += "{";
    json += "\"R_NAME2\": \"" + String(R_NAME2) + "\"";
    R_STATUS2 ? json += ",\"R_STATUS2\": true" : json += ",\"R_STATUS2\": false";
    R_LOGIC2 ? json += ",\"R_LOGIC2\": true" : json += ",\"R_LOGIC2\": false";
    json += ",\"R_DESCRIPTION2\": \"" + String(R_DESCRIPTION2) + "\"";
    json += "}";
    json += "]";
    json += ",\"dimmer\":" + String(dim);
    json += "}";

    request->addInterestingHeader("API ESP32 Server"); // este es el header se agrega
    // send(estado,aplicacion,formato)
    request->send(200, dataType, json);
}

//---------------------------------------------------------
// leer información proporcionada por otros dispositivos atravez de protocolo espnow y mesh
// url: /api/espnow
// Método: GET
//--------------------------------------------------------
void handleApiEspNow(AsyncWebServerRequest *request)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    String json = "";

    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"MiMAC\": \"" + String(WiFi.macAddress()) + "\"";
    json += ",\"ESPNOW\":";
    json += jsonStringApi; // informacion enviada por el protocolo espnow
    json += "}";

    request->addInterestingHeader("API ESP32 Server"); // este es el header se agrega
    // send(estado,aplicacion,formato)
    request->send(200, dataType, json);
}

//---------------------------------------------------------
// Leer parámetros de configuracion wifi
// url: /api/connection/wifi
// Método: GET
//--------------------------------------------------------
void handleApiWifi(AsyncWebServerRequest *request)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    String json = "";
    // string template
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"wifi\":";
    json += "{";
    wifi_mode ? json += "\"wifi_mode\": true" : json += "\"wifi_mode\": false";
    json += ",\"wifi_ssid\": \"" + String(wifi_ssid) + "\"";
    json += ",\"wifi_password\": \"" + String(wifi_password) + "\"";
    json += ",\"wifi_ssid2\": \"" + String(wifi_ssid2) + "\"";
    json += ",\"wifi_password2\": \"" + String(wifi_password2) + "\"";
    wifi_ip_static ? json += ",\"wifi_ip_static\": true" : json += ",\"wifi_ip_static\": false";
    json += ",\"wifi_ipv4\": \"" + String(wifi_ipv4) + "\"";
    json += ",\"wifi_gateway\": \"" + String(wifi_gateway) + "\"";
    json += ",\"wifi_subnet\": \"" + String(wifi_subnet) + "\"";
    json += ",\"wifi_dns_primary\": \"" + String(wifi_dns_primary) + "\"";
    json += ",\"wifi_dns_secondary\": \"" + String(wifi_dns_secondary) + "\"";
    json += ",\"ap_ssid\": \"" + String(ap_ssid) + "\"";
    json += ",\"ap_password\": \""
            "\"";
    json += ",\"ap_chanel\":" + String(ap_chanel);
    ap_visibility ? json += ",\"ap_visibility\": false" : json += ",\"ap_visibility\": true";
    json += ",\"ap_connect\":" + String(ap_connect);
    json += "},";
    json += "\"code\": 1 ";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}

//-----------------------------------------------------------------
// Método POST para actualizar configuraciones WiFi
// url: /api/connection/wifi
// Método: POST
//-----------------------------------------------------------------
void handleApiPostWiFi(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    // la data mete en un string
    String bodyContent = GetBodyContent(data, len);
    // y se inserta en documento JSON llamado doc
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"ERROR de JSON enviado\"}");
        log("INFO", "api.hpp", "posible falta de memoria en StaticDocument");
        return;
    };
    //---------------------------------------------------------------------
    // WIFI Client settings.json
    //---------------------------------------------------------------------
    String s = "";
    // APP Mode, jalamos la informacion para modificar los valores de las variables
    wifi_mode = doc["wifi_mode"].as<bool>();
    // SSID Client
    if (doc["wifi_ssid"] != "")
    { // si no esta vacia actualizamos la data
        s = doc["wifi_ssid"].as<String>();
        s.trim();                                         // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_ssid, s.c_str(), sizeof(wifi_ssid)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                           // limpiamos el valor de s
    }
    // Password
    if (doc["wifi_password"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_password"].as<String>();
        s.trim();                                                 // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_password, s.c_str(), sizeof(wifi_password)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                                   // limpiamos el valor de s
    }
    // SSID2 Client
    if (doc["wifi_ssid2"] != "")
    { // si no esta vacia actualizamos la data
        s = doc["wifi_ssid2"].as<String>();
        s.trim();                                           // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_ssid2, s.c_str(), sizeof(wifi_ssid2)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                             // limpiamos el valor de s
    }
    // Password2
    if (doc["wifi_password2"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_password2"].as<String>();
        s.trim();                                                   // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_password2, s.c_str(), sizeof(wifi_password2)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                                     // limpiamos el valor de s
    }
    // DHCP
    wifi_ip_static = doc["wifi_ip_static"].as<bool>();
    // IPv4
    if (doc["wifi_ipv4"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_ipv4"].as<String>();
        s.trim();                                         // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_ipv4, s.c_str(), sizeof(wifi_ipv4)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                           // limpiamos el valor de s
    }
    // Gateway
    if (doc["wifi_gateway"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_gateway"].as<String>();
        s.trim();                                               // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_gateway, s.c_str(), sizeof(wifi_gateway)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                                 // limpiamos el valor de s
    }
    // Subnet
    if (doc["wifi_subnet"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_subnet"].as<String>();
        s.trim();                                             // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_subnet, s.c_str(), sizeof(wifi_subnet)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                               // limpiamos el valor de s
    }
    // DNS Primario
    if (doc["wifi_dns_primary"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_dns_primary"].as<String>();
        s.trim();                                                       // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_dns_primary, s.c_str(), sizeof(wifi_dns_primary)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                                         // limpiamos el valor de s
    }
    // DNS Secundario
    if (doc["wifi_dns_secondary"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["wifi_dns_secondary"].as<String>();
        s.trim();                                                           // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(wifi_dns_secondary, s.c_str(), sizeof(wifi_dns_secondary)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                                             // limpiamos el valor de s
    }
    //-------------------------------------------------------------------
    // WIFI AP settings.json
    //-------------------------------------------------------------------
    // AP SSID
    if (doc["ap_ssid"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["ap_ssid"].as<String>();
        s.trim();                                     // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(ap_ssid, s.c_str(), sizeof(ap_ssid)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                       // limpiamos el valor de s
    }
    // AP Password
    if (doc["ap_password"] != "")
    { // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        s = doc["ap_password"].as<String>();
        s.trim();                                             // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(ap_password, s.c_str(), sizeof(ap_password)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                               // limpiamos el valor de s
    }
    // AP visibility 0 visible - 1 Oculto
    ap_visibility = !doc["ap_visiblility"].as<int>(); // para mi que es un bool ------------------------------------------------++++++++++++++++++++++++++++++++++++++++
    if (doc["ap_chanel"] != "")
    {
        ap_chanel = doc["ap_chanel"].as<int>();
    }
    // AP number of Connections
    if (doc["ap_connect"] != "")
    {
        ap_connect = doc["ap_connect"].as<int>();
    }
    // Save Settings.json  (salvar la data)
    if (especialSave())
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true}");
    }
    else
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false,\"descripción\": \"Erros con el SPIFFS en save verificar json e información\"}");
    }
}

//---------------------------------------------------------
// leer información proporcionada del dispositivo en alarmas
// url: /api/alarmas
// Método: GET
//--------------------------------------------------------
void handleApiAlarmas(AsyncWebServerRequest *request)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    String json = "";

    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"ALRMS\":";
    json += "{";
    json += "\"ALRM_PIN1\": \"" + String(ALRM_PIN1) + "\"";
    json += ",\"ALRM_NAME1\": \"" + String(ALRM_NAME1) + "\"";
    digitalRead(alarma1.PIN) ? (ALRM_LOGIC1 ? json += ",\"ALRM_STATUS1\": true" : json += ",\"ALRM_STATUS1\": false") : (ALRM_LOGIC1 ? json += ",\"ALRM_STATUS1\": false" : json += ",\"ALRM_STATUS1\": true");
    ALRM_LOGIC1 ? json += ",\"ALRM_LOGIC1\": true" : json += ",\"ALRM_LOGIC1\": false";
    json += ",\"ALRM_TON1\": \"" + String(ALRM_TON1) + "\"";
    json += ",\"ALRM_TOFF1\": \"" + String(ALRM_TOFF1) + "\"";
    json += ",\"ALRM_CONT1\": \"" + String(ALRM_CONT1) + "\"";
    //---------------------------------------------------
    json += ",\"ALRM_PIN2\": \"" + String(ALRM_PIN2) + "\"";
    json += ",\"ALRM_NAME2\": \"" + String(ALRM_NAME2) + "\"";
    digitalRead(alarma2.PIN) ? (ALRM_LOGIC2 ? json += ",\"ALRM_STATUS2\": true" : json += ",\"ALRM_STATUS2\": false") : (ALRM_LOGIC2 ? json += ",\"ALRM_STATUS2\": false" : json += ",\"ALRM_STATUS2\": true");
    ALRM_LOGIC2 ? json += ",\"ALRM_LOGIC2\": true" : json += ",\"ALRM_LOGIC2\": false";
    json += ",\"ALRM_TON2\": \"" + String(ALRM_TON2) + "\"";
    json += ",\"ALRM_TOFF2\": \"" + String(ALRM_TOFF2) + "\"";
    json += ",\"ALRM_CONT2\": \"" + String(ALRM_CONT2) + "\"";
    //-------------------------------------------------------------------------------------------
    json += ",\"ALRM_PIN3\": \"" + String(ALRM_PIN3) + "\"";
    json += ",\"ALRM_NAME3\": \"" + String(ALRM_NAME3) + "\"";
    digitalRead(alarma3.PIN) ? (ALRM_LOGIC3 ? json += ",\"ALRM_STATUS3\": true" : json += ",\"ALRM_STATUS3\": false") : (ALRM_LOGIC3 ? json += ",\"ALRM_STATUS3\": false" : json += ",\"ALRM_STATUS3\": true");
    ALRM_LOGIC3 ? json += ",\"ALRM_LOGIC3\": true" : json += ",\"ALRM_LOGIC3\": false";
    json += ",\"ALRM_TON3\": \"" + String(ALRM_TON3) + "\"";
    json += ",\"ALRM_TOFF3\": \"" + String(ALRM_TOFF3) + "\"";
    json += ",\"ALRM_CONT3\": \"" + String(ALRM_CONT3) + "\"";
    //-------------------------------------------------------------------------------------------
    json += ",\"ALRM_PIN4\": \"" + String(ALRM_PIN4) + "\"";
    json += ",\"ALRM_NAME4\": \"" + String(ALRM_NAME4) + "\"";
    digitalRead(alarma4.PIN) ? (ALRM_LOGIC4 ? json += ",\"ALRM_STATUS4\": true" : json += ",\"ALRM_STATUS4\": false") : (ALRM_LOGIC4 ? json += ",\"ALRM_STATUS4\": false" : json += ",\"ALRM_STATUS4\": true");
    ALRM_LOGIC4 ? json += ",\"ALRM_LOGIC4\": true" : json += ",\"ALRM_LOGIC4\": false";
    json += ",\"ALRM_TON4\": \"" + String(ALRM_TON4) + "\"";
    json += ",\"ALRM_TOFF4\": \"" + String(ALRM_TOFF4) + "\"";
    json += ",\"ALRM_CONT4\": \"" + String(ALRM_CONT4) + "\"";
    //-------------------------------------------------------------------------------------------
    json += ",\"ALRM_PIN5\": \"" + String(ALRM_PIN5) + "\"";
    json += ",\"ALRM_NAME5\": \"" + String(ALRM_NAME5) + "\"";
    digitalRead(alarma5.PIN) ? (ALRM_LOGIC5 ? json += ",\"ALRM_STATUS5\": true" : json += ",\"ALRM_STATUS5\": false") : (ALRM_LOGIC5 ? json += ",\"ALRM_STATUS5\": false" : json += ",\"ALRM_STATUS5\": true");
    ALRM_LOGIC5 ? json += ",\"ALRM_LOGIC5\": true" : json += ",\"ALRM_LOGIC5\": false";
    json += ",\"ALRM_TON5\": \"" + String(ALRM_TON5) + "\"";
    json += ",\"ALRM_TOFF5\": \"" + String(ALRM_TOFF5) + "\"";
    json += ",\"ALRM_CONT5\": \"" + String(ALRM_CONT5) + "\"";
    //-------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------

    Activar ? json += ",\"Activar\": true" : json += ",\"Activar\": false";
    json += "}";
    json += "}";

    request->addInterestingHeader("API ESP32 Server"); // este es el header se agrega
    // send(estado,aplicacion,formato)
    request->send(200, dataType, json);
}

//---------------------------------------------------------
// leer información proporcionada del dispositivo en relays
// url: /api/relays
// Método: GET
//--------------------------------------------------------
void handleApiRelays(AsyncWebServerRequest *request)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    String json = "";

    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    json += ",\"time\": \"" + getDateTime() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"RELAY1\":";
    json += "{";
    R_LOGIC1 ? json += "\"R_LOGIC1\": true" : json += "\"R_LOGIC1\": false";                  // por si trabaja energizado
    json += ",\"R_NAME1\": \"" + String(R_NAME1) + "\"";                                      // nombre del relay
    json += ",\"R_DESCRIPTION1\": \"" + String(R_DESCRIPTION1) + "\"";                        // breve descripcion
    R_TIMERON1 ? json += ",\"R_TIMERON1\": true" : json += ",\"R_TIMERON1\": false";          // si se active el timer de este relevador
    json += ",\"R_TIMER1\": \"" + String(R_TIMER1) + "\"";                                    // contador regresivo por minuto
    TEMPORIZADOR1 ? json += ",\"TEMPORIZADOR1\": true" : json += ",\"TEMPORIZADOR1\": false"; // indica si hay un control por tiempo definido por horario
    json += ",\"TIMEONRELAY1\": \"" + String(TIMEONRELAY1) + "\"";                            // indica a que hora se prende en caso de logica negativa a que hr se apaga
    json += ",\"TIMEOFFRELAY1\": \"" + String(TIMEOFFRELAY1) + "\"";                          // indica a que hora se apaga en caso de logica negativa a que hr se prende
    R_STATUS1 ? json += ",\"R_STATUS1\": true" : json += ",\"R_STATUS1\": false";             // encendido o apagado
    json += "}";
    json += ",\"RELAY2\":";
    json += "{";
    R_LOGIC2 ? json += "\"R_LOGIC2\": true" : json += "\"R_LOGIC2\": false";                  // por si trabaja energizado
    json += ",\"R_NAME2\": \"" + String(R_NAME2) + "\"";                                      // nombre del relay
    json += ",\"R_DESCRIPTION2\": \"" + String(R_DESCRIPTION2) + "\"";                        // breve descripcion
    R_TIMERON2 ? json += ",\"R_TIMERON2\": true" : json += ",\"R_TIMERON2\": false";          // si se active el timer de este relevador
    json += ",\"R_TIMER2\": \"" + String(R_TIMER2) + "\"";                                    // contador regresivo por minuto
    TEMPORIZADOR2 ? json += ",\"TEMPORIZADOR2\": true" : json += ",\"TEMPORIZADOR2\": false"; // indica si hay un control por tiempo definido por horario
    json += ",\"TIMEONRELAY2\": \"" + String(TIMEONRELAY2) + "\"";                            // indica a que hora se prende en caso de logica negativa a que hr se apaga
    json += ",\"TIMEOFFRELAY2\": \"" + String(TIMEOFFRELAY2) + "\"";                          // indica a que hora se apaga en caso de logica negativa a que hr se prende
    R_STATUS2 ? json += ",\"R_STATUS2\": true" : json += ",\"R_STATUS2\": false";             // encendido o apagado
    json += "}";
    json += "}";
    request->addInterestingHeader("API ESP32 Server"); // este es el header se agrega
    // send(estado,aplicacion,formato)
    request->send(200, dataType, json);
}

//-----------------------------------------------------------------
// Método POST para actualizar configuraciones Alarmas
// url: /api/alarmas
// Método: POST
//-----------------------------------------------------------------
void handleApiPostAlarmas(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    // la data mete en un string
    String bodyContent = GetBodyContent(data, len);
    // Serial.print(bodyContent);
    //  y se inserta en documento JSON llamado doc
    StaticJsonDocument<1024> doc; // 768
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"ERROR de JSON enviado\"}");
        log("INFO", "api.hpp", "posible falta de memoria en StaticDocument");
        return;
    };
    //---------------------------------------------------------------------
    // Alarmas en especial.json
    //---------------------------------------------------------------------

    ALRM_LOGIC1 = doc["ALRM_LOGIC1"].as<bool>();
    if (doc["ALRM_NAME1"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_NAME1 = doc["ALRM_NAME1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TON1"] != "")
    {                                              // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TON1 = doc["ALRM_TON1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TOFF1"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TOFF1 = doc["ALRM_TOFF1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_CONT1"] != "")
    {                                             // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_CONT1 = doc["ALRM_CONT1"].as<int>(); // modificamos el valor de la variable con ese valor convertido en String
    }

    ALRM_LOGIC2 = doc["ALRM_LOGIC2"].as<bool>();
    if (doc["ALRM_NAME2"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_NAME2 = doc["ALRM_NAME2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TON2"] != "")
    {                                              // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TON2 = doc["ALRM_TON2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TOFF2"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TOFF2 = doc["ALRM_TOFF2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_CONT2"] != "")
    {                                             // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_CONT2 = doc["ALRM_CONT2"].as<int>(); // modificamos el valor de la variable con ese valor convertido en String
    }

    ALRM_LOGIC3 = doc["ALRM_LOGIC3"].as<bool>();
    if (doc["ALRM_NAME3"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_NAME3 = doc["ALRM_NAME3"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TON3"] != "")
    {                                              // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TON3 = doc["ALRM_TON3"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TOFF3"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TOFF3 = doc["ALRM_TOFF3"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_CONT3"] != "")
    {                                             // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_CONT3 = doc["ALRM_CONT3"].as<int>(); // modificamos el valor de la variable con ese valor convertido en String
    }

    ALRM_LOGIC4 = doc["ALRM_LOGIC4"].as<bool>();
    if (doc["ALRM_NAME4"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_NAME4 = doc["ALRM_NAME4"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TON4"] != "")
    {                                              // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TON4 = doc["ALRM_TON4"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TOFF4"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TOFF4 = doc["ALRM_TOFF4"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_CONT4"] != "")
    {                                             // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_CONT4 = doc["ALRM_CONT4"].as<int>(); // modificamos el valor de la variable con ese valor convertido en String
    }

    ALRM_LOGIC5 = doc["ALRM_LOGIC5"].as<bool>();
    if (doc["ALRM_NAME5"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_NAME5 = doc["ALRM_NAME5"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TON5"] != "")
    {                                              // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TON5 = doc["ALRM_TON5"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_TOFF5"] != "")
    {                                                // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_TOFF5 = doc["ALRM_TOFF5"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["ALRM_CONT5"] != "")
    {                                             // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        ALRM_CONT5 = doc["ALRM_CONT5"].as<int>(); // modificamos el valor de la variable con ese valor convertido en String
    }

    if (especialSave())
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true}");
    }
    else
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false,\"descripción\": \"Erros con el SPIFFS en save verificar json e información\"}");
    }
}

//-----------------------------------------------------------------
// Método POST para actualizar configuraciones Relays
// url: /api/relays
// Método: POST
//-----------------------------------------------------------------
void handleApiPostRelays(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // agregar el usuario y contraseña
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
        {
            return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
        }
    }
    // la data mete en un string
    String bodyContent = GetBodyContent(data, len);
    // Serial.print(bodyContent);
    //  y se inserta en documento JSON llamado doc
    StaticJsonDocument<1024> doc; // 768
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"ERROR de JSON enviado\"}");
        log("INFO", "api.hpp", "posible falta de memoria en StaticDocument");
        return;
    };
    //---------------------------------------------------------------------
    // Relays especial.json
    //---------------------------------------------------------------------
    String s = "";
    // Relays1
    if (doc["R_STATUS1"] != "")
        R_STATUS1 = doc["R_STATUS1"].as<bool>();
    if (doc["R_NAME1"] != "")
    { // si no esta vacia actualizamos la data
        s = doc["R_NAME1"].as<String>();
        s.trim();                                     // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(R_NAME1, s.c_str(), sizeof(R_NAME1)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                       // limpiamos el valor de s
    }
    if (doc["R_TIMERON1"] != "")
        R_TIMERON1 = doc["R_TIMERON1"].as<bool>(); // indica si se activa el timer del relevador1
    if (doc["R_TIMER1"] != "")
    {
        R_TIMER1 = doc["R_TIMER1"].as<int>(); // contador regresivo por minuto el tiempo que permanecera encendido
    }
    if (doc["R_LOGIC1"] != "")
        R_LOGIC1 = doc["R_LOGIC1"].as<bool>(); // por si trabaja energizado logica positiva o verdera en caso contraro false
    if (doc["R_DESCRIPTION1"] != "")
    {                                                        // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        R_DESCRIPTION1 = doc["R_DESCRIPTION1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["TEMPORIZADOR1"] != "")
        TEMPORIZADOR1 = doc["TEMPORIZADOR1"].as<bool>(); // indica si hay un control por tiempo
    if (doc["TIMEONRELAY1"] != "")
    {                                                    // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        TIMEONRELAY1 = doc["TIMEONRELAY1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["TIMEOFFRELAY1"] != "")
    {                                                      // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        TIMEOFFRELAY1 = doc["TIMEOFFRELAY1"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    // Relays2
    if (doc["R_STATUS2"] != "")
        R_STATUS2 = doc["R_STATUS2"].as<bool>();
    if (doc["R_NAME2"] != "")
    { // si no esta vacia actualizamos la data
        s = doc["R_NAME2"].as<String>();
        s.trim();                                     // limpiamos de caracteres vacios (se los quitamos)
        strlcpy(R_NAME2, s.c_str(), sizeof(R_NAME2)); // modificamos el valor de la variable con ese valor convertido en char
        s = "";                                       // limpiamos el valor de s
    }
    if (doc["R_TIMERON2"] != "")
        R_TIMERON2 = doc["R_TIMERON2"].as<bool>(); // indica si se activa el timer del relevador2
    if (doc["R_TIMER2"] != "")
    {
        R_TIMER2 = doc["R_TIMER2"].as<int>(); // contador regresivo por minuto el tiempo que permanecera encendido
    }
    if (doc["R_LOGIC2"] != "")
        R_LOGIC2 = doc["R_LOGIC2"].as<bool>(); // por si trabaja energizado logica positiva o verdera en caso contraro false
    if (doc["R_DESCRIPTION2"] != "")
    {                                                        // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        R_DESCRIPTION2 = doc["R_DESCRIPTION2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["TEMPORIZADOR2"] != "")
        TEMPORIZADOR2 = doc["TEMPORIZADOR2"].as<bool>(); // indica si hay un control por tiempo
    if (doc["TIMEONRELAY2"] != "")
    {                                                    // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        TIMEONRELAY2 = doc["TIMEONRELAY2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    if (doc["TIMEOFFRELAY2"] != "")
    {                                                      // si no esta vacia actualizamos la data en caso contrario mantiene su valor
        TIMEOFFRELAY2 = doc["TIMEOFFRELAY2"].as<String>(); // modificamos el valor de la variable con ese valor convertido en String
    }
    String respuesta = "";

    respuesta = "{";
    respuesta += "\"serial\": \"" + DeviceID() + "\"";
    respuesta += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? respuesta += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : respuesta += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? respuesta += ",\"wifiStatus\": true" : respuesta += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? respuesta += ",\"rssiStatus\":" + String(WiFi.RSSI()) : respuesta += ",\"rssiStatus\": 0";
    mqttClient.connected() ? respuesta += ",\"mqttStatus\": true" : respuesta += ",\"mqttStatus\": false";
    respuesta += ",\"RELAY1\":";
    respuesta += "{";
    respuesta += "\"R_NAME1\": \"" + String(R_NAME1) + "\"";                                            // nombre del relay
    R_STATUS1 ? respuesta += ",\"R_STATUS1\": true" : respuesta += ",\"R_STATUS1\": false";             // encendido o apagado
    R_TIMERON1 ? respuesta += ",\"R_TIMERON1\": true" : respuesta += ",\"R_TIMERON1\": false";          // si se active el timer de este relevador
    respuesta += ",\"R_TIMER1\": \"" + String(R_TIMER1) + "\"";                                         // contador regresivo por minuto
    R_LOGIC1 ? respuesta += ",\"R_LOGIC1\": true" : respuesta += ",\"R_LOGIC1\": false";                // por si trabaja energizado
    respuesta += ",\"R_DESCRIPTION1\": \"" + String(R_DESCRIPTION1) + "\"";                             // breve descripcion
    TEMPORIZADOR1 ? respuesta += ",\"TEMPORIZADOR1\": true" : respuesta += ",\"TEMPORIZADOR1\": false"; // indica si hay un control por tiempo definido por horario
    respuesta += ",\"TIMEONRELAY1\": \"" + String(TIMEONRELAY1) + "\"";                                 // indica a que hora se prende en caso de logica negativa a que hr se apaga
    respuesta += ",\"TIMEOFFRELAY1\": \"" + String(TIMEOFFRELAY1) + "\"";                               // indica a que hora se apaga en caso de logica negativa a que hr se prende
    respuesta += "}";
    respuesta += ",\"RELAY2\":";
    respuesta += "{";
    respuesta += "\"R_NAME2\": \"" + String(R_NAME2) + "\"";                                            // nombre del relay
    R_STATUS2 ? respuesta += ",\"R_STATUS2\": true" : respuesta += ",\"R_STATUS2\": false";             // encendido o apagado
    R_TIMERON2 ? respuesta += ",\"R_TIMERON2\": true" : respuesta += ",\"R_TIMERON2\": false";          // si se active el timer de este relevador
    respuesta += ",\"R_TIMER2\": \"" + String(R_TIMER2) + "\"";                                         // contador regresivo por minuto
    R_LOGIC2 ? respuesta += ",\"R_LOGIC2\": true" : respuesta += ",\"R_LOGIC2\": false";                // por si trabaja energizado
    respuesta += ",\"R_DESCRIPTION2\": \"" + String(R_DESCRIPTION2) + "\"";                             // breve descripcion
    TEMPORIZADOR2 ? respuesta += ",\"TEMPORIZADOR2\": true" : respuesta += ",\"TEMPORIZADOR2\": false"; // indica si hay un control por tiempo definido por horario
    respuesta += ",\"TIMEONRELAY2\": \"" + String(TIMEONRELAY2) + "\"";                                 // indica a que hora se prende en caso de logica negativa a que hr se apaga
    respuesta += ",\"TIMEOFFRELAY2\": \"" + String(TIMEOFFRELAY2) + "\"";                               // indica a que hora se apaga en caso de logica negativa a que hr se prende
    respuesta += "},";
    if (especialSave())
    {
        respuesta += "\"save\": true}";
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, respuesta);
    }
    else
    {
        respuesta += "\"save\": false,\"descripción\": \"Erros con el SPIFFS en save verificar json e información\"}";
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, respuesta);
    }
    ctrlRelays();
}

// -------------------------------------------------------------------
// Escanear todas las redes WIFI al alcance de la señal
// url: /api/connection/wifi-scan
// Método: GET
// Notas: La primera solicitud devolverá 0 resultados a menos que comience
// a escanear desde otro lugar (ciclo / configuración).
// No solicite más de 3-5 segundos. \ ALT + 92
// -------------------------------------------------------------------
void handleApiWifiScan(AsyncWebServerRequest *request)
{
    /*negarLoopMQTT = true;
    log("INFO","api.hpp","desconectando mqtt");
    //tenemos que desactivar mqtt
    mqttClient.disconnect();*/
    /*
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    log("INFO","api.hpp","Escaneando Redes...");
    String json = "";
    int n = WiFi.scanComplete();
    if (n == -2){
        json = "{";
        json += "\"meta\": { \"serial\": \"" + DeviceID() + "\", \"count\": 0},";
        json += "\"data\": [";
        json += "],";
        json += "\"code\": 0 ";
        json += "}";
        WiFi.scanNetworks(true, true);
    } else if (n){
        json = "{";
        json += "\"meta\": { \"serial\": \"" + DeviceID() + "\", \"count\":" + String(n) + "},";
        json += "\"data\": [";

        for (int i = 0; i < n; ++i){
            if (i)
                json += ",";
            json += "{";
            json += "\"n\":" + String(i + 1);
            json += ",\"rssi\":" + String(WiFi.RSSI(i));
            json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
            json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";  //la mac
            json += ",\"channel\":" + String(WiFi.channel(i));
            json += ",\"secure\":\"" + EncryptionType(WiFi.encryptionType(i)) + "\"";
            json += "}";
        }

        json += "],";
        json += "\"code\": 1 ";
        json += "}";

        WiFi.scanDelete();
        if (WiFi.scanComplete() == -2){
            WiFi.scanNetworks(true, true);
        }
    }

    request->addInterestingHeader("API ESP32 Server");
    log("INFO","api.hpp",json);
    Serial.flush();
    request->send(200, dataType, json);
    //negarLoopMQTT = false;
    */
}

// -------------------------------------------------------------------
// Parámetros de configuración MQTT
// url: /api/connection/mqtt
// Método: GET
// -------------------------------------------------------------------
void handleApiMQTT(AsyncWebServerRequest *request)
{ // handleApiMQTT manejador de peticion
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"mqtt\":{";
    mqtt_enable ? json += "\"mqtt_enable\": true" : json += "\"mqtt_enable\": false";
    json += ",\"mqtt_server\": \"" + String(mqtt_server) + "\"";
    json += ",\"mqtt_port\":" + String(mqtt_port);
    mqtt_retain ? json += ",\"mqtt_retain\": true" : json += ",\"mqtt_retain\": false";
    json += ",\"mqtt_qos\":" + String(mqtt_qos);
    json += ",\"mqtt_id\": \"" + String(mqtt_id) + "\"";
    json += ",\"mqtt_user\": \"" + String(mqtt_user) + "\"";
    json += ",\"mqtt_password\": \""
            "\"";
    mqtt_clean_sessions ? json += ",\"mqtt_clean_sessions\": true" : json += ",\"mqtt_clean_sessions\": false";
    mqtt_time_send ? json += ",\"mqtt_time_send\": true" : json += "\"mqtt_time_send\": false";
    json += ",\"mqtt_time_interval\":" + String(mqtt_time_interval / 1000); // 60000 / 1000 = 30s
    mqtt_status_send ? json += ",\"mqtt_status_send\": true" : json += ",\"mqtt_status_send\": false";
    json += "},";
    json += "\"code\": 1 ";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}

// -------------------------------------------------------------------
// Actualizar las configuraciones del MQTT Conexiones
// url: /api/connection/mqtt
// Método: POST
// -------------------------------------------------------------------
void handleApiPostMQTT(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    //  -------------------------------------------------------------------
    //  MQTT Conexión settings.json
    //  -------------------------------------------------------------------
    String s = "";
    // MQTT Enable
    mqtt_enable = doc["mqtt_enable"].as<bool>();
    // MQTT Server
    if (doc["mqtt_server"] != "")
    {
        s = doc["mqtt_server"].as<String>();
        s.trim();
        strlcpy(mqtt_server, s.c_str(), sizeof(mqtt_server));
        s = "";
    }
    // MQTT Port
    if (doc["mqtt_port"] != "")
    {
        mqtt_port = doc["mqtt_port"].as<int>();
    }
    // MQTT Retain
    mqtt_retain = doc["mqtt_retain"].as<bool>();
    // MQTT QOS 0-1-2
    mqtt_qos = doc["mqtt_qos"].as<int>();
    // MQTT ID
    if (doc["mqtt_id"] != "")
    {
        s = doc["mqtt_id"].as<String>();
        s.trim();
        strlcpy(mqtt_id, s.c_str(), sizeof(mqtt_id));
        s = "";
    }
    // MQTT User
    if (doc["mqtt_user"] != "")
    {
        s = doc["mqtt_user"].as<String>();
        s.trim();
        strlcpy(mqtt_user, s.c_str(), sizeof(mqtt_user));
        s = "";
    }
    // MQTT Password
    if (doc["mqtt_password"] != "")
    {
        s = doc["mqtt_password"].as<String>();
        s.trim();
        strlcpy(mqtt_password, s.c_str(), sizeof(mqtt_password));
        s = "";
    }
    // mqtt_clean_sessions
    mqtt_clean_sessions = doc["mqtt_clean_sessions"].as<bool>();
    // -------------------------------------------------------------------
    // MQTT Envio de datos settings.json
    // -------------------------------------------------------------------
    // mqtt_time_send
    mqtt_time_send = doc["mqtt_time_send"].as<bool>();
    // mqtt_time_interval
    mqtt_time_interval = doc["mqtt_time_interval"].as<int>() * 1000; // 60 * 1000 = 60000 = 60s
    // mqtt_status_send
    mqtt_status_send = doc["mqtt_status_send"].as<bool>();
    // Save Settings.json
    if (especialSave())
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");
    }
    else
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}

// -------------------------------------------------------------------
// Manejo de la descarga del archivo setting.json
// url: "/api/device/download"
// Método: GET
// -------------------------------------------------------------------
void handleApiDownload(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    log("INFO", "api.hpp", "Descarga del archivo settings.json");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/settings.json", dataType, true); // el true es para descargar
    request->send(response);
}

// -------------------------------------------------------------------
// Manejo de la descarga del archivo setting.json
// url: "/api/especial/download"
// Método: GET
// -------------------------------------------------------------------
void handleApiDownloadEspecial(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    log("INFO", "api.hpp", "Descarga del archivo especial.json");
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/especial.json", dataType, true);
    request->send(response);
}

// -------------------------------------------------------------------
// Manejo de la carga del archivo settings.json
// url: "/api/device/upload"
// Método: POST
// -------------------------------------------------------------------
// Variables para la carga del archivo
File file;
bool opened = false;
void handleApiUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    if (filename != "settings.json")
    {
        log("ERROR", "api.hpp", "No es el archivo valido:" + filename + " \" - settings.json");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No es el archivo valido: " + filename + " !\"}");
        return;
    }
    if (!index)
    {
        Serial.printf("INFO", "Upload Start: %s\n", filename.c_str());
        Serial.println("");
    }
    // Validar el archivo si esta abierto settings.json
    if (opened == false)
    {
        opened = true;
        // file = SPIFFS.open(String("/") + filename, FILE_WRITE); //se modifica a especial.json
        file = SPIFFS.open(String("/especial.json"), FILE_WRITE);
        if (!file)
        {
            log("ERROR", "api.hpp", "No se pudo abrir el archivo para escribir");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo abrir el archivo para escribir!\"}");
            return;
        }
    }

    // Escribir el archivo en la memoria
    if (file.write(data, len) != len)
    {
        log("ERROR", "api.hpp", "No se pudo escribir el archivo");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo escribir el archivo: " + filename + " !\"}");
        return;
    }
    // Finalizada la carga del archivo.
    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"msg\": \"¡Carga del archivo: " + filename + " completada!\"}");
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Location", "/");
        request->send(response);
        file.close();
        opened = false;
        log("INFO", "api.hpp", "Carga del archivo " + filename + " completada");
        // Esperar la transmisión de los datos seriales
        Serial.flush();
        // Reiniciar el ESP32
        ESP.restart();
    }
}

// -------------------------------------------------------------------
// Manejo de la carga del archivo settings.json
// url: "/api/especial/upload"
// Método: POST
// -------------------------------------------------------------------
// Variables para la carga del archivo
File especialUp;
bool opened2 = false;
void handleApiEspecialUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    if (filename != "especial.json")
    {
        log("ERROR", "api.hpp", "No es el archivo valido:" + filename + " \" - especial.json");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No es el archivo valido: " + filename + " !\"}");
        return;
    }
    if (!index)
    {
        Serial.printf("INFO", "Upload Start: %s\n", filename.c_str());
        Serial.println("");
    }
    // Validar el archivo si esta abierto settings.json
    if (opened2 == false)
    {
        opened2 = true;
        especialUp = SPIFFS.open(String("/") + filename, FILE_WRITE);
        if (!especial)
        {
            log("ERROR", "api.hpp", "No se pudo abrir el archivo para escribir");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo abrir el archivo para escribir!\"}");
            return;
        }
    }

    // Escribir el archivo en la memoria
    if (especialUp.write(data, len) != len)
    {
        log("ERROR", "api.hpp", "No se pudo escribir el archivo");
        request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo escribir el archivo: " + filename + " !\"}");
        return;
    }

    // Finalizada la carga del archivo.
    if (final)
    {
        AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"msg\": \"¡Carga del archivo: " + filename + " completada!\"}");
        response->addHeader("Cache-Control", "no-cache");
        response->addHeader("Location", "/");
        request->send(response);
        especialUp.close();
        opened2 = false;
        log("INFO", "api.hpp", "Carga del archivo " + filename + " completada");
        // Esperar la transmisión de los datos seriales
        Serial.flush();
        // Reiniciar el ESP32
        ESP.restart(); // necesario para guardar las configuraciones
    }
}

// -------------------------------------------------------------------
// Manejo de la actualización del Firmware a FileSystem
// url: /api/device/firmware
// Método: POST
// -------------------------------------------------------------------
void handleApiFirmware(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    // Si el nombre de archivo incluye ( spiffs ), actualiza la partición de spiffs que es U_PART
    int cmd = (filename.indexOf("spiffs") > -1) ? U_PART : U_FLASH; // U_FLASH es el firmware
    String updateSystem = cmd == U_PART ? "FileSystem" : "Firmware";

    if (filename != "firmware.bin")
    {
        if (filename != "spiffs.bin")
        {
            log("ERROR", "api.hpp", "No es el archivo valido:" + filename + " \" se espera firmware.bin o spiffs.bin");
            request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No es el archivo valido: " + filename + " !\"}");
            return;
        }
    }

    if (!index)
    {
        content_len = request->contentLength(); // se captura el tamaño del archivo
        log("INFO", "api.hpp", "Actualización del " + updateSystem + " iniciada...");
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
        { // si da un error
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Index: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
            log("ERROR", "api.hpp", "Update del " + updateSystem + " no ejecutado.");
        }
    }
    // si hasta aqui todo ok
    //  escribir e firmware o el filesystem
    if (Update.write(data, len) != len)
    {
        Update.printError(Serial);
        log("ERROR", "api.hpp", "Update del " + updateSystem + " no ejecutado.");
    }
    // Terminado
    if (final)
    {
        if (!Update.end(true))
        {
            AsyncWebServerResponse *response = request->beginResponse(500, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo actualizar el " + updateSystem + " Final: " + filename + " !\"}");
            request->send(response);
            Update.printError(Serial);
            log("ERROR", "api.hpp", "Update del " + updateSystem + " no ejecutado.");
        }
        else
        {
            AsyncWebServerResponse *response = request->beginResponse(201, dataType, "{ \"save\": true, \"type\": \"" + updateSystem + "\"}");
            response->addHeader("Cache-Control", "no-cache");
            response->addHeader("Location", "root@" + updateSystem + "");
            request->send(response);
            log("INFO", "api.hpp", "Update del " + updateSystem + " completado");
            // Esperar la Transmisión de los datos seriales
            Serial.flush();
            ESP.restart();
        }
    }
}
// -------------------------------------------------------------------
// Manejo de parámetros de estados Globales
// url: /api/device/status
// Método: GET
// -------------------------------------------------------------------

void handleApiGetStatus(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"code\": 1 ";
    json += "}";
    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}
// -------------------------------------------------------------------
// Manejo del reinicio del dispositivo
// url: /api/device/restart
// Método: POST
// -------------------------------------------------------------------
void handleApiPostRestart(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    // Retornamos la respuesta
    request->send(200, dataType, "{ \"restart\": true }");
    // Reiniciar el ESP32
    String origin = "API";
    restart(origin);
}

// -------------------------------------------------------------------
// Manejo de la restauración del dispositivo
// url: /api/device/restore
// Método: POST
// -------------------------------------------------------------------
void handleApiPostRestore(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    // Retornamos la respuesta
    request->send(200, dataType, "{ \"restore\": true }");
    // Reiniciar el ESP32
    String origin = "API";
    restore(origin);
}
// -------------------------------------------------------------------
// Actualizar las configuraciones del acceso al servidor - contraseña
// url: /api/perfil/user
// Método: POST
// -------------------------------------------------------------------

void handleApiPostUser(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    // -------------------------------------------------------------------
    // Contraseña settings.json
    // -------------------------------------------------------------------
    String p, np, cp;
    // capturamos las variables enviadas en el JSON
    p = doc["device_old_password"].as<String>();    // Contraseña Actual
    np = doc["device_new_password"].as<String>();   // Nueva Contraseña
    cp = doc["device_c_new_password"].as<String>(); // Confirmación de nueva Contraseña
    // Limpiamos de espacios vacios
    p.trim();
    np.trim();
    cp.trim();

    // Validar que los datos de lacontraseña anterior no este en blanco
    if (p != "")
    { // si p que es el password es diferente de vacio entra
        // validar que la contraseña que introdusca debe coincida con la anterior
        if (p == device_password)
        {
            if (np == "" && cp == "")
            { // validar que nuevo paswword y confirmacion de password no vengan vacias
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite los datos de la nueva contraseña y confirmación vacíos!\"}");
                return;
            }
            else if (np != "" && cp != "" && np == cp)
            { // si no vienen vacios y son iguales
                if (np == device_password)
                { // si la contraseña es igual a la anterio
                    request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La contraseña nueva no puede ser igual a la anterior\"}");
                    return;
                }
                strlcpy(device_password, np.c_str(), sizeof(device_password)); // lo pasamos a char y la asignamos a device_password
                if (especialSave())
                { // salcamos todo el settings
                    request->send(200, dataType, "{ \"save\": true, \"msg\": \"¡Contraseña actualizada correctamente!\" }");
                }
                else
                {
                    request->send(500, dataType, "{ \"save\": false, \"msg\": \"¡ Internar server error !\" }");
                }
            }
            else if (np != cp)
            { // son diferentes
                request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, La nueva contraseña y confirmación de nueva contraseña no coinciden!\"}");
                return;
            }
            else
            {
            } // else vacio por convencion
        }
        else
        {
            AsyncWebServerResponse *response = request->beginResponse(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se pudo guardar, la contraseña anterior no coincide\"}");
            request->send(response);
            return;
        }
    }
    else
    {
        request->send(400, dataType, "{ \"save\": false, \"msg\": \"¡Error, No se permite contraseña anterior en blanco\"}");
        return;
    }
}
// -------------------------------------------------------------------
// Manejo de las salidas a relay
// url: /api/device/relays
// Método: POST
// -------------------------------------------------------------------
/*
void handleApiPostRelays(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if (security){
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    // capturamos la data en string
    String bodyContent = GetBodyContent(data, len);
    // Validar que sea un JSON
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error){
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    if(OnOffRelays(bodyContent)){
        // Retornamos la respuesta
        if(settingsSave())
            request->send(200, dataType, "{ \"relay\": true, \"name\":  \"" + doc["output"].as<String>() + "\", \"status\": true }");
    }else{
        if(settingsSave())
            request->send(200, dataType, "{ \"relay\": true, \"name\":  \"" + doc["output"].as<String>() + "\", \"status\": false }");
    }
}*/
// -------------------------------------------------------------------
// Manejo del dimmer
// url: /api/device/dimmer
// Método: POST
// -------------------------------------------------------------------

void handleApiPostDimmer(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    // capturamos la data en string
    String bodyContent = GetBodyContent(data, len);
    // Validar que sea un JSON
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    dimmer(bodyContent);
    request->send(200, dataType, "{ \"dimmer\": true, \"value\": \"" + String(dim) + "\" }");
}

//*****************************************************************************
// Agregado por Eduardo Sanchez
// configuracion del mqtt para la obtencion de alarmas remotas
//*************************************************************************************

// -------------------------------------------------------------------
// Parámetros de configuración MQTT
// url: /api/connection/mqtt2
// Método: GET
// -------------------------------------------------------------------
void handleApiMQTT2(AsyncWebServerRequest *request)
{ // handleApiMQTT manejador de peticion
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"mqtt2\":{";
    mqtt2_status ? json += "\"mqtt2_status\": true" : json += "\"mqtt2_status\": false";
    mqtt_enable2 ? json += ",\"mqtt_enable2\": true" : json += ",\"mqtt_enable2\": false";
    json += ",\"mqtt_server2\": \"" + String(mqtt_server2) + "\"";
    json += ",\"mqtt_port2\":" + String(mqtt_port2);
    mqtt_retain2 ? json += ",\"mqtt_retain2\": true" : json += ",\"mqtt_retain2\": false";
    json += ",\"mqtt_qos2\":" + String(mqtt_qos2);
    json += ",\"mqtt_id2\": \"" + String(mqtt_id2) + "\"";
    json += ",\"mqtt_user2\": \"" + String(mqtt_user2) + "\"";
    json += ",\"mqtt_password2\": \"" + String(mqtt_password2) + "\"";
    json += ",\"mqtt_extraTopic\": \"" + String(mqtt_extraTopic) + "\"";
    mqtt_clean_sessions2 ? json += ",\"mqtt_clean_sessions2\": true" : json += ",\"mqtt_clean_sessions2\": false";
    mqtt_time_send2 ? json += ",\"mqtt_time_send2\": true" : json += "\"mqtt_time_send2\": false";
    json += ",\"mqtt_time_interval\":" + String(mqtt_time_interval2 / 1000); // 60000 / 1000 = 30s
    mqtt_status_send2 ? json += ",\"mqtt_status_send2\": true" : json += ",\"mqtt_status_send2\": false";
    json += "},";
    json += "\"code\": 1 ";
    json += "}";

    request->addInterestingHeader("API ESP32 Server");
    request->send(200, dataType, json);
}
//---------------------------------------------------------
// leer información de las alarmas proporcionada de otro dispositivo
// url: /api/connection/alarmasremotas
// Método: GET
//--------------------------------------------------------
// void handleApiAlarmRemote(AsyncWebServerRequest *request)
// { // lo que viene de el mqtt
//     // agregar el usuario y contraseña
//     if (security)
//     {
//         if (!request->authenticate(device_user, device_password))
//         {
//             return request->requestAuthentication(); // aqui se queda en circulos seria bueno agregar un contador para que salga del bucle
//         }
//     }
//     String json = "";

//     json = "{";
//     json += "\"serial\": \"" + String(REMOTE_serial) + "\"";
//     json += ",\"MACWiFi\": \"" + String(REMOTE_MACWiFi) + "\"";
//     json += ",\"RELAYS\":";
//     json += "{";
//     json += "\"REMOTE_R_NAME1\": \"" + String(REMOTE_R_NAME1) + "\"";
//     json += ",\"REMOTE_R_NAME2\": \"" + String(REMOTE_R_NAME2) + "\"";
//     REMOTE_R_STATUS1 ? json += ",\"REMOTE_R_STATUS1\": true" : json += ",\"REMOTE_R_STATUS1\": false";
//     REMOTE_R_STATUS2 ? json += ",\"REMOTE_R_STATUS2\": true" : json += ",\"REMOTE_R_STATUS2\": false";
//     REMOTE_R_LOGIC1 ? json += ",\"REMOTE_R_LOGIC1\": true" : json += ",\"REMOTE_R_LOGIC1\": false";
//     REMOTE_R_LOGIC2 ? json += ",\"REMOTE_R_LOGIC2\": true" : json += ",\"REMOTE_R_LOGIC2\": false";
//     json += "}";
//     json += ",\"ALRMS\":";
//     json += "{";
//     json += "\"REMOTE_ALRM_NAME1\": \"" + String(REMOTE_ALRM_NAME1) + "\"";
//     REMOTE_ALRM_STATUS1 ? json += ",\"REMOTE_ALRM_STATUS1\": true" : json += ",\"REMOTE_ALRM_STATUS1\": false";
//     REMOTE_ALRM_LOGIC1 ? json += ",\"REMOTE_ALRM_LOGIC1\": true" : json += ",\"REMOTE_ALRM_LOGIC1\": false";
//     json += ",\"REMOTE_ALRM_TON1\": \"" + String(REMOTE_ALRM_TON1) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF1\": \"" + String(REMOTE_ALRM_TOFF1) + "\"";
//     json += ",\"REMOTE_ALRM_CONT1\": \"" + String(REMOTE_ALRM_CONT1) + "\"";
//     //---------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME2\": \"" + String(REMOTE_ALRM_NAME2) + "\"";
//     REMOTE_ALRM_STATUS2 ? json += ",\"REMOTE_ALRM_STATUS2\": true" : json += ",\"REMOTE_ALRM_STATUS2\": false";
//     REMOTE_ALRM_LOGIC2 ? json += ",\"REMOTE_ALRM_LOGIC2\": true" : json += ",\"REMOTE_ALRM_LOGIC2\": false";
//     json += ",\"REMOTE_ALRM_TON2\": \"" + String(REMOTE_ALRM_TON2) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF2\": \"" + String(REMOTE_ALRM_TOFF2) + "\"";
//     json += ",\"REMOTE_ALRM_CONT2\": \"" + String(REMOTE_ALRM_CONT2) + "\"";
//     //-------------------------------------------------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME3\": \"" + String(REMOTE_ALRM_NAME3) + "\"";
//     REMOTE_ALRM_STATUS3 ? json += ",\"REMOTE_ALRM_STATUS3\": true" : json += ",\"REMOTE_ALRM_STATUS3\": false";
//     REMOTE_ALRM_LOGIC3 ? json += ",\"REMOTE_ALRM_LOGIC3\": true" : json += ",\"REMOTE_ALRM_LOGIC3\": false";
//     json += ",\"REMOTE_ALRM_TON3\": \"" + String(REMOTE_ALRM_TON3) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF3\": \"" + String(REMOTE_ALRM_TOFF3) + "\"";
//     json += ",\"REMOTE_ALRM_CONT3\": \"" + String(REMOTE_ALRM_CONT3) + "\"";
//     //-------------------------------------------------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME4\": \"" + String(REMOTE_ALRM_NAME4) + "\"";
//     REMOTE_ALRM_STATUS4 ? json += ",\"REMOTE_ALRM_STATUS4\": true" : json += ",\"REMOTE_ALRM_STATUS4\": false";
//     REMOTE_ALRM_LOGIC4 ? json += ",\"REMOTE_ALRM_LOGIC4\": true" : json += ",\"REMOTE_ALRM_LOGIC4\": false";
//     json += ",\"REMOTE_ALRM_TON4\": \"" + String(REMOTE_ALRM_TON4) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF4\": \"" + String(REMOTE_ALRM_TOFF4) + "\"";
//     json += ",\"REMOTE_ALRM_CONT4\": \"" + String(REMOTE_ALRM_CONT4) + "\"";
//     //-------------------------------------------------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME5\": \"" + String(REMOTE_ALRM_NAME5) + "\"";
//     REMOTE_ALRM_STATUS5 ? json += ",\"REMOTE_ALRM_STATUS5\": true" : json += ",\"REMOTE_ALRM_STATUS5\": false";
//     REMOTE_ALRM_LOGIC5 ? json += ",\"REMOTE_ALRM_LOGIC5\": true" : json += ",\"REMOTE_ALRM_LOGIC5\": false";
//     json += ",\"REMOTE_ALRM_TON5\": \"" + String(REMOTE_ALRM_TON5) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF5\": \"" + String(REMOTE_ALRM_TOFF5) + "\"";
//     json += ",\"REMOTE_ALRM_CONT5\": \"" + String(REMOTE_ALRM_CONT5) + "\"";
//     //-------------------------------------------------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME6\": \"" + String(REMOTE_ALRM_NAME6) + "\"";
//     REMOTE_ALRM_STATUS6 ? json += ",\"REMOTE_ALRM_STATUS6\": true" : json += ",\"REMOTE_ALRM_STATUS6\": false";
//     REMOTE_ALRM_LOGIC6 ? json += ",\"REMOTE_ALRM_LOGIC6\": true" : json += ",\"REMOTE_ALRM_LOGIC6\": false";
//     json += ",\"REMOTE_ALRM_TON6\": \"" + String(REMOTE_ALRM_TON6) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF6\": \"" + String(REMOTE_ALRM_TOFF6) + "\"";
//     json += ",\"REMOTE_ALRM_CONT6\": \"" + String(REMOTE_ALRM_CONT6) + "\"";
//     //-------------------------------------------------------------------------------------------
//     json += ",\"REMOTE_ALRM_NAME7\": \"" + String(REMOTE_ALRM_NAME7) + "\"";
//     REMOTE_ALRM_STATUS7 ? json += ",\"REMOTE_ALRM_STATUS7\": true" : json += ",\"REMOTE_ALRM_STATUS7\": false";
//     REMOTE_ALRM_LOGIC7 ? json += ",\"REMOTE_ALRM_LOGIC7\": true" : json += ",\"REMOTE_ALRM_LOGIC7\": false";
//     json += ",\"REMOTE_ALRM_TON7\": \"" + String(REMOTE_ALRM_TON7) + "\"";
//     json += ",\"REMOTE_ALRM_TOFF7\": \"" + String(REMOTE_ALRM_TOFF7) + "\"";
//     json += ",\"REMOTE_ALRM_CONT7\": \"" + String(REMOTE_ALRM_CONT7) + "\"";
//     //-------------------------------------------------------------------------------------------
//     Activar ? json += ",\"Activar\": true" : json += ",\"Activar\": false";
//     json += "}";
//     json += "}";

//     request->addInterestingHeader("API ESP32 Server"); // este es el header se agrega
//     // send(estado,aplicacion,formato)
//     request->send(200, dataType, json);
// }

// -------------------------------------------------------------------
// Actualizar las configuraciones para el MQTT2 Conexiones
// url: /api/connection/mqtt2
// Método: POST
// -------------------------------------------------------------------
void handleApiPostMQTT2(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }

    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<768> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    //  -------------------------------------------------------------------
    //  MQTT Conexión settings.json
    //  -------------------------------------------------------------------
    String s = "";
    // MQTT Enable
    mqtt_enable2 = doc["mqtt_enable2"].as<bool>();
    // MQTT Server
    if (doc["mqtt_server2"] != "")
    {
        s = doc["mqtt_server2"].as<String>();
        s.trim();
        strlcpy(mqtt_server2, s.c_str(), sizeof(mqtt_server2));
        s = "";
    }
    // MQTT Port
    if (doc["mqtt_port2"] != "")
    {
        mqtt_port2 = doc["mqtt_port2"].as<int>();
    }
    // MQTT Retain
    mqtt_retain2 = doc["mqtt_retain2"].as<bool>();
    // MQTT QOS 0-1-2
    mqtt_qos2 = doc["mqtt_qos2"].as<int>();
    // MQTT ID
    if (doc["mqtt_id2"] != "")
    {
        s = doc["mqtt_id2"].as<String>();
        s.trim();
        strlcpy(mqtt_id2, s.c_str(), sizeof(mqtt_id2));
        s = "";
    }
    // MQTT User
    if (doc["mqtt_user2"] != "")
    {
        s = doc["mqtt_user2"].as<String>();
        s.trim();
        strlcpy(mqtt_user2, s.c_str(), sizeof(mqtt_user2));
        s = "";
    }
    // MQTT Password
    if (doc["mqtt_password2"] != "")
    {
        s = doc["mqtt_password2"].as<String>();
        s.trim();
        strlcpy(mqtt_password2, s.c_str(), sizeof(mqtt_password2));
        s = "";
    }
    if (doc["mqtt_extraTopic"] != "")
    {
        s = doc["mqtt_extraTopic"].as<String>();
        s.trim();
        strlcpy(mqtt_extraTopic, s.c_str(), sizeof(mqtt_extraTopic));
        s = "";
    }
    // mqtt_clean_sessions
    mqtt_clean_sessions2 = doc["mqtt_clean_sessions2"].as<bool>();
    // -------------------------------------------------------------------
    // MQTT Envio de datos settings.json
    // -------------------------------------------------------------------
    // mqtt_time_send
    mqtt_time_send2 = doc["mqtt_time_send2"].as<bool>();
    // mqtt_time_interval
    mqtt_time_interval2 = doc["mqtt_time_interval2"].as<int>() * 1000; // 60 * 1000 = 60000 = 60s
    // mqtt_status_send
    mqtt_status_send2 = doc["mqtt_status_send2"].as<bool>();
    // Save Settings.json
    if (especialSave())
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(200, dataType, "{ \"save\": true }");
    }
    else
    {
        request->addInterestingHeader("API ESP32 Server");
        request->send(500, dataType, "{ \"save\": false }");
    }
}

// ------------------------------------------------------------------
// Leer información de tiempo proporcionada por el servidor
// url: /api/time
// Método: GET
// ------------------------------------------------------------------
void handleApiTime(AsyncWebServerRequest *request)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    String json = "";
    json = "{";
    json += "\"serial\": \"" + DeviceID() + "\"";
    json += ",\"device\": \"" + platform() + "\"";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiQuality\":" + String(getRSSIasQuality(WiFi.RSSI())) : json += ",\"wifiQuality\": 0";
    WiFi.status() == WL_CONNECTED ? json += ",\"wifiStatus\": true" : json += ",\"wifiStatus\": false";
    WiFi.status() == WL_CONNECTED ? json += ",\"rssiStatus\":" + String(WiFi.RSSI()) : json += ",\"rssiStatus\": 0";
    mqttClient.connected() ? json += ",\"mqttStatus\": true" : json += ",\"mqttStatus\": false";
    json += ",\"time\":{";
    time_ajuste ? json += "\"time_ajuste\": true" : json += "\"time_ajuste\": false";
    json += ",\"time_date\": \"" + String(time_date) + "\""; // 2023-03-07T23:47
    json += ",\"time_z_horaria\": \"" + String(time_z_horaria) + "\"";
    json += ",\"time_server\": \"" + String(time_server) + "\"";
    json += ",\"time_now\": \"" + String(getDateTime()) + "\""; // 2023-03-07T23:47
    json += ",\"hora\": \"" + String(releTime()) + "\"";        // 2023-03-07T23:47
    json += "},";
    json += "\"code\": 1 ";
    json += "}";
    request->send(200, dataType, json);
}

//--------------------------------------------------------------------------------
// Parámetros de Configuración del Tiempo guardar cambios
// url: /api/time
// Método: POST
//---------------------------------------------------------------------------------
void handleApiPostTime(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    String bodyContent = GetBodyContent(data, len);
    StaticJsonDocument<320> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };
    // -------------------------------------------------------------------
    // Time settings.json
    // -------------------------------------------------------------------
    String s = "";
    // Manual - Internet true/false
    time_ajuste = doc["time_ajuste"].as<bool>();
    // Fecha - Hora
    if (doc["time_date"] != "")
    {
        s = doc["time_date"].as<String>();
        s.trim();
        strlcpy(time_date, s.c_str(), sizeof(time_date));
        s = "";
    }
    // Numero de la zona Horaria
    if (doc["time_z_horaria"] != "")
    {
        time_z_horaria = doc["time_z_horaria"].as<float>() * 3600;
    }
    // Servidor NTP
    if (doc["time_server"] != "")
    {
        s = doc["time_server"].as<String>();
        s.trim();
        strlcpy(time_server, s.c_str(), sizeof(time_server));
        s = "";
    }

    // Save Settings.json
    if (especialSave())
    {
        request->send(200, dataType, "{ \"save\": true }");
        log("INFO", "api.hpp", " Time Set OK");
        // Esperar la Transmisión de los datos seriales
        Serial.flush();
        delay(10);                    // como se va a reiniciar pongo un tiempo de retardo para que mando el codigo 200
        WiFi.disconnect(true, false); // para evitar el error
        ESP.restart();
    }
    else
    {
        request->send(500, dataType, "{ \"save\": false }");
    }
}

// buzzer handleApiPostBuzzer
//  -------------------------------------------------------------------
//  Manejo del buzzer
//  url: /api/device/buzzer
//  Método: POST
//  -------------------------------------------------------------------

void handleApiPostBuzzer(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (security)
    {
        if (!request->authenticate(device_user, device_password))
            return request->requestAuthentication();
    }
    // capturamos la data en string
    String bodyContent = GetBodyContent(data, len);
    // Validar que sea un JSON
    StaticJsonDocument<250> doc;
    DeserializationError error = deserializeJson(doc, bodyContent);
    if (error)
    {
        request->send(400, dataType, "{ \"status\": \"Error de JSON enviado\" }");
        return;
    };

    buzzer(bodyContent);
    request->send(200, dataType, "{ \"buzzer\": true, \"value\": \"" + String(BUZZER_STATUS) + "\" }");
    if (BUZZER_STATUS)
    {
        digitalWrite(BUZZER, HIGH);
    }
    else
    {
        digitalWrite(BUZZER, LOW);
    }
}