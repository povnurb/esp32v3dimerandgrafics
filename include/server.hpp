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