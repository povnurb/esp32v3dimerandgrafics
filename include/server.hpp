// en este archivo se manejarán las peticiones del servidor

#pragma once
#include "ESPAsyncWebServer.h"
#include <Update.h> //para actializar el fimware

AsyncWebServer server(80);          // iniciamos el servidor en el puerto 80
AsyncWebSocket ws("/ws");           // iniciamos el webSocket
AsyncEventSource events("/events"); // eventos para el webSockets

// particion del codigo
#include "api.hpp"
#include "web.hpp"

void initServer()
{
    //-----------------------------------------------------------------
    // ZONA donde Empezamos a desarrollar la API REST
    //-----------------------------------------------------------------

    //-----------------------------------------------------------------
    // solicitud de la pagina index de la Api
    // Parametros de configuración Index API
    // url: /api/index
    // Método: GET
    // para cuando soliciten en el navegador la direccion la "IP"+/api/index
    //-----------------------------------------------------------------
    server.on("/api/index", HTTP_GET, handleApiIndex); // handleApiIndex es una función que se encuentra en el archivo api.hpp

    //-----------------------------------------------------------------
    // solicitud de la pagina espnow de la Api
    // Parametros de configuración espnow API
    // url: /api/espnow
    // Método: GET
    // server.on("/api/espnow", HTTP_GET, handleApiEspNow); // handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    // Parámetros de configuración WiFi
    // url: /api/connection/wifi
    // Método: GET
    //------------------------------------------------------------------
    server.on("/api/connection/wifi", HTTP_GET, handleApiWifi);

    //------------------------------------------------------------------
    // Método POST para actualizar configuraciones Wifi
    // url: /api/connection/wifi
    // Método: POST
    //------------------------------------------------------------------
    server.on(
        "/api/connection/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostWiFi);

    //-----------------------------------------------------------------
    // solicitud de la pagina alarmas de la Api
    // Parametros de configuración alarmas API
    // url: /api/alarmas
    // Método: GET
    server.on("/api/alarmas", HTTP_GET, handleApiAlarmas); // handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    // solicitud de la pagina relays de la Api
    // Parametros de configuración relays API
    // url: /api/relays
    // Método: GET
    server.on("/api/relays", HTTP_GET, handleApiRelays); // handleApiIndex es una función que se encuentra en el archivo api.hpp

    //------------------------------------------------------------------
    // Método POST para actualizar configuraciones Wifi
    // url: /api/alarmas
    // Método: POST
    //------------------------------------------------------------------
    server.on(
        "/api/alarmas", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostAlarmas);

    //------------------------------------------------------------------
    // Método POST para actualizar configuraciones Wifi
    // url: /api/relays
    // Método: POST
    //------------------------------------------------------------------
    server.on(
        "/api/relays", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostRelays);

    // Escanear todas las redes WIFI al alcance de la señal
    // url: /api/connection/wifi-scan
    // Método: GET
    // Notas: La primera solicitud devolverá 0 resultados a menos que comience
    // a escanear desde otro lugar (ciclo / configuración).
    // No solicite más de 3-5 segundos. \ ALT + 92
    // -------------------------------------------------------------------
    // server.on("/api/connection/wifi-scan", HTTP_GET, handleApiWifiScan);

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
    server.on(
        "/api/connection/mqtt", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostMQTT);
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
    server.on(
        "/api/device/upload", HTTP_POST, [](AsyncWebServerRequest *request)
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
    server.on(
        "/api/especial/upload", HTTP_POST, [](AsyncWebServerRequest *request)
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
    server.on(
        "/api/device/firmware", HTTP_POST, [](AsyncWebServerRequest *request) {},
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
    server.on(
        "/api/perfil/user", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostUser);

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
    // server.on(
    //     "/api/device/dimmer", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostDimmer);

    // -------------------------------------------------------------------
    // logout o cerrar secion
    // url: /api/esp-logout
    // Método: POST
    // -------------------------------------------------------------------
    server.on("/api/esp-logout", HTTP_POST, [](AsyncWebServerRequest *request)
              {
        if(security){
            if (!request->authenticate(device_user, device_password))
                return request->requestAuthentication();
        } 
        request->send(401, dataType, "{ \"session\": false, \"msg\": \"Sessión cerrada correctamente\"}"); });

    //*************************************************************************************************
    // El siguiente es agregado por Eduardo Sanchez
    // la intancion es conectarse a otro broker para obtener alarmas de otro dispositivo
    // y si no se puede pues por lo menos conectarse al mismo para obtener alarmas de otro dispositivo
    //*************************************************************************************************

    // -------------------------------------------------------------------
    // Control Buzzer
    // url: /api/device/buzzer
    // Método: POST
    // -------------------------------------------------------------------
    server.on(
        "/api/device/buzzer", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostBuzzer);

    // -------------------------------------------------------------------
    // Parámetros de configuración MQTT
    // url: /api/connection/mqtt2
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/api/connection/mqtt2", HTTP_GET, handleApiMQTT2);
    // -------------------------------------------------------------------
    // Parámetros de configuración MQTT y obtener las alarmas remotas
    // url: /api/connection/alarmasremotas
    // Método: GET
    // -------------------------------------------------------------------
    // server.on("/api/connection/alarmasremotas", HTTP_GET, handleApiAlarmRemote);

    // primero configurar el post para despues obtener el get
    //  -------------------------------------------------------------------
    //  Actualizar las configuraciones del MQTT para las alarmas remotas
    //  url: /api/connection/mqtt2
    //  Método: POST
    //  -------------------------------------------------------------------
    server.on(
        "/api/connection/mqtt2", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostMQTT2);

    // ------------------------------------------------------------------
    // Parámetros de Configuración del Tiempo
    // url: /api/time
    // Método: GET
    // ------------------------------------------------------------------
    server.on("/api/time", HTTP_GET, handleApiTime);

    //--------------------------------------------------------------------------------
    // Parámetros de Configuración del Tiempo guardar cambios
    // url: /api/time
    // Método: POST
    //---------------------------------------------------------------------------------
    server.on(
        "/api/time", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, handleApiPostTime);
    //-------------------------------------------------------------------------------------------------
    // Zona servidor web
    //--------------------------------------------------------------------------------------

    //-----------------------------------------------------------------------
    // Cargar pagina index.html
    // url:/
    // Método:Get
    //--------------------------------------------------------------------------
    server.on("/", HTTP_GET, handleIndex);
    //-----------------------------------------------------------------------
    // Cargar la ruta /esp-wifi
    //--------------------------------------------------------------------------
    server.on("/esp-wifi", HTTP_GET, handleWifi);
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-mqtt
    // -------------------------------------------------------------------
    server.on("/esp-mqtt", HTTP_GET, handleMqtt);
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-restart
    // -------------------------------------------------------------------
    server.on("/esp-restart", HTTP_GET, handleRestart);
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-restore
    // -------------------------------------------------------------------
    server.on("/esp-restore", HTTP_GET, handleRestore);
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-device
    // -------------------------------------------------------------------
    server.on("/esp-device", HTTP_GET, handleFirmware);
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-admin
    // -------------------------------------------------------------------
    server.on("/esp-admin", HTTP_GET, handleUser);
    //----------------------------------------------------------------------------
    // faltan
    //----------------------------------------------------------------------------
    // #include "./webh/relays.html.gz.h"
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-relays
    // -------------------------------------------------------------------
    server.on("/esp-relays", HTTP_GET, handleRelays);
    //-----------------------------------------------------------------------
    // #include "./webh/time.html.gz.h"
    //--------------------------------------------------------------------------
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-time
    // -------------------------------------------------------------------
    server.on("/esp-time", HTTP_GET, handleTime);
    //--------------------------------------------------------------------
    // #include "./webh/alarmas.html.gz.h"
    //--------------------------------------------------------------------
    // -------------------------------------------------------------------
    // Cargar de la ruta /esp-alarmas
    // -------------------------------------------------------------------
    server.on("/esp-alarmas", HTTP_GET, handleAlarmas);
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------

    // -------------------------------------------------------------------
    // Zona archivos estáticos *******************************************
    // -------------------------------------------------------------------
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/bootstrap.css
    // -------------------------------------------------------------------
    server.on("/assets/css/bootstrap.css", HTTP_GET, handleBootstrapCss);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/icons.css
    // -------------------------------------------------------------------
    server.on("/assets/css/icons.css", HTTP_GET, handleIconsCss);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/style.css
    // -------------------------------------------------------------------
    server.on("/assets/css/style.css", HTTP_GET, handleStyleCss);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/sweetalert.css
    // -------------------------------------------------------------------
    server.on("/assets/css/sweetalert.css", HTTP_GET, handleSweetalertCss);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/fonts/icons.woff
    // -------------------------------------------------------------------
    server.on("/assets/css/fonts/icons.woff", HTTP_GET, handleIconsWoff);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/css/fonts/icons.woff2
    // -------------------------------------------------------------------
    server.on("/assets/css/fonts/icons.woff2", HTTP_GET, handleIconsWoff2);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/apexcharts.min.js
    // -------------------------------------------------------------------
    server.on("/assets/js/apexcharts.min.js", HTTP_GET, handleApexChartsJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/bundle.min.js
    // -------------------------------------------------------------------
    server.on("/assets/js/bundle.min.js", HTTP_GET, handleBundleJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/sweetalert.js
    // -------------------------------------------------------------------
    server.on("/assets/js/sweetalert.js", HTTP_GET, handleSweetAlertJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/app.js
    // -------------------------------------------------------------------
    server.on("/assets/js/app.js", HTTP_GET, handleAppJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/scripts.js
    // -------------------------------------------------------------------
    server.on("/assets/js/scripts.js", HTTP_GET, handleScriptsJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/index.js
    // -------------------------------------------------------------------
    server.on("/assets/js/index.js", HTTP_GET, handleIndexJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/wifi.js
    // -------------------------------------------------------------------
    server.on("/assets/js/wifi.js", HTTP_GET, handleWifiJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/mqtt.js
    // -------------------------------------------------------------------
    server.on("/assets/js/mqtt.js", HTTP_GET, handleMqttJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/firmware.js
    // -------------------------------------------------------------------
    server.on("/assets/js/firmware.js", HTTP_GET, handleFirmwareJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/restart.js
    // -------------------------------------------------------------------
    server.on("/assets/js/restart.js", HTTP_GET, handleRestartJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/restore.js
    // -------------------------------------------------------------------
    server.on("/assets/js/restore.js", HTTP_GET, handleRestoreJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/admin.js
    // -------------------------------------------------------------------
    server.on("/assets/js/admin.js", HTTP_GET, handleAdminJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/template.js
    // -------------------------------------------------------------------
    server.on("/assets/js/template.js", HTTP_GET, handleTemplateJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/template.js
    // -------------------------------------------------------------------
    server.on("/assets/js/alarmas.js", HTTP_GET, handleAlarmasJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/template.js
    // -------------------------------------------------------------------
    server.on("/assets/js/time.js", HTTP_GET, handleTimeJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/js/template.js
    // -------------------------------------------------------------------
    server.on("/assets/js/relays.js", HTTP_GET, handleRelaysJs);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/img/logo.png
    // -------------------------------------------------------------------
    server.on("/assets/img/logo.png", HTTP_GET, handleLogoPng);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/img/favicon.png
    // -------------------------------------------------------------------
    server.on("/assets/img/favicon.png", HTTP_GET, handleFavIconPng);
    // -------------------------------------------------------------------
    // Cargar de Archivos complementarios /assets/img/loader.png
    // -------------------------------------------------------------------
    server.on("/assets/img/loader.png", HTTP_GET, handleLoaderPng);

    // manejador de error 404
    server.onNotFound([](AsyncWebServerRequest *request)
                      {
        if (request->method() == HTTP_OPTIONS){
        request->send(200);
        }else{
             if (security){
                if (!request->authenticate(device_user, device_password))
                    return request->requestAuthentication();
            }
            AsyncWebServerResponse *response = request->beginResponse_P(200, dataTypeHTML,page404_html_gz, page404_html_gz_len);
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
        } });
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // Iniciar el servidor WEB + CORS
    //----------------------------------------------------------------------------

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Method", "POST, GET, OPTIONS, DELETE");
    server.begin();
    Update.onProgress(printProgress);
    log("INFO", "server.hpp", "Servidor HTTP iniciado");
}