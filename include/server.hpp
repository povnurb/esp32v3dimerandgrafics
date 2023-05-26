// en este archivo se manejarán las peticiones del servidor

#pragma once
#include "ESPAsyncWebServer.h"
#include <Update.h> //para actializar el fimware

AsyncWebServer server(80);          //iniciamos el servidor en el puerto 80
AsyncWebSocket ws ("/ws");          //iniciamos el webSocket
AsyncEventSource events("events");  //eventos para el webSockets

//variables para actualizar el Firmware
size_t content_len;
#define U_PART U_SPIFFS

//particion del codigo
#include "api.hpp"



void initServer(){
    //-----------------------------------------------------------------
    //ZONA donde Empezamos a desarrollar la API REST
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    //solicitud de la pagina index de la Api
    //Parametros de configuración Index API
    //url: /api/index
    //Método: GET
    //para cuando soliciten en el navegador la direccion la "IP"+/api/index 
    //-----------------------------------------------------------------
    server.on("/api/index",HTTP_GET, handleApiIndex); //handleApiIndex es una función que se encuentra en el archivo api.hpp

    //-----------------------------------------------------------------
    //solicitud de la pagina espnow de la Api
    //Parametros de configuración espnow API
    //url: /api/espnow
    //Método: GET
    server.on("/api/espnow",HTTP_GET, handleApiEspNow); //handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    //Parámetros de configuración WiFi
    //url: /api/connection/wifi
    //Método: GET
    //------------------------------------------------------------------
    server.on("/api/connection/wifi", HTTP_GET, handleApiWifi);

    //------------------------------------------------------------------
    //Método POST para actualizar configuraciones Wifi
    //url: /api/connection/wifi
    //Método: POST
    //------------------------------------------------------------------
    server.on("/api/connection/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostWiFi);

    //-----------------------------------------------------------------
    //solicitud de la pagina alarmas de la Api
    //Parametros de configuración alarmas API
    //url: /api/alarmas
    //Método: GET
    server.on("/api/alarmas",HTTP_GET, handleApiAlarmas); //handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    //solicitud de la pagina relays de la Api
    //Parametros de configuración relays API
    //url: /api/relays
    //Método: GET
    server.on("/api/relays",HTTP_GET, handleApiRelays); //handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    //Método POST para actualizar configuraciones Wifi
    //url: /api/alarmas
    //Método: POST
    //------------------------------------------------------------------
    server.on("/api/alarmas", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostAlarmas);

    //------------------------------------------------------------------
    //Método POST para actualizar configuraciones Wifi
    //url: /api/relays
    //Método: POST
    //------------------------------------------------------------------
    server.on("/api/relays", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostRelays);

    // Escanear todas las redes WIFI al alcance de la señal
    // url: /api/connection/wifi-scan
    // Método: GET
    // Notas: La primera solicitud devolverá 0 resultados a menos que comience
    // a escanear desde otro lugar (ciclo / configuración).
    // No solicite más de 3-5 segundos. \ ALT + 92
    // -------------------------------------------------------------------
    server.on("/api/connection/wifi-scan", HTTP_GET, handleApiWifiScan);

    // -------------------------------------------------------------------
    // Parámetros de configuración MQTT
    // url: /api/connection/mqtt
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/connection/mqtt", HTTP_GET, handleApiMQTT);

    // -------------------------------------------------------------------
    // Actualizar las configuraciones del MQTT Conexiones
    // url: /api/connection/mqtt
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/connection/mqtt", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostMQTT);

    // -------------------------------------------------------------------
    // Manejo de la descarga del archivo setting.json
    // url: "/api/device/download"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/download", HTTP_GET, handleApiDownload);

    // -------------------------------------------------------------------
    // Manejo de la descarga del archivo setting.json
    // url: "/api/especial/download"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/especial/download", HTTP_GET, handleApiDownloadEspecial);

    // -------------------------------------------------------------------
    // Manejo de la carga del archivo settings.json
    // url: "/api/device/upload"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/upload", HTTP_POST, [](AsyncWebServerRequest *request)
    { opened = false; },
    [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            handleApiUpload(request, filename, index, data, len, final);
        });
    

    // -------------------------------------------------------------------
    // Manejo de la carga del archivo settings.json
    // url: "/api/especial/upload"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/especial/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { opened2 = false; },
        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            handleApiEspecialUpload(request, filename, index, data, len, final);
        });
    

    // -------------------------------------------------------------------
    // Manejo de la actualización del Firmware a FileSystem
    // url: /api/device/firmware
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/firmware", HTTP_POST, [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
        {
            handleApiFirmware(request, filename, index, data, len, final);
        });


    // -------------------------------------------------------------------
    // Manejo de parámetros de estados Globales
    // url: "/api/device/status"
    // Método: GET
    // -------------------------------------------------------------------
    server.on("/api/device/status", HTTP_GET, handleApiGetStatus);

    // -------------------------------------------------------------------
    // Manejo del reinicio
    // url: "/api/device/restart"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/restart", HTTP_POST, handleApiPostRestart);

    // -------------------------------------------------------------------
    // Manejo de la restauración
    // url: "/api/device/restore"
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/device/restore", HTTP_POST, handleApiPostRestore);

    // -------------------------------------------------------------------
    // Actualizar las configuraciones del acceso al servidor - contraseña
    // url: /api/perfil/user
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/perfil/user", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostUser);

    // -------------------------------------------------------------------
    // On/Off los relays
    // url: /api/device/relays
    // Método: POST
    // -------------------------------------------------------------------
    // server.on("/api/device/relays", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostRelays);

    // -------------------------------------------------------------------
    // Regulador Dimmer
    // url: /api/device/dimmer
    // Método: POST
    // -------------------------------------------------------------------
    //server.on("/api/device/dimmer", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostDimmer);

    server.begin();
    log("INFO","server.hpp","Servidor HTTP iniciado");
}