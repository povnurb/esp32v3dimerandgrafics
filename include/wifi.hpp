#include <WiFiMulti.h>

WiFiMulti wifiMulti;

const byte DNSSERVER_PORT = 53;
DNSServer dnsServer;

IPAddress ap_IPv4(192, 168, 4, 1); // ip del dispositivo en modo ap
IPAddress ap_subnet(255, 255, 255, 0);

// WiFi.mode(WIFI_STA)      -station mode: el esp32 se conecta a un access point
// WiFi.mode(WIFI_AP)       -access point mode: las estaciones pueden conectarse al esp32 (los clientes)
// WiFi.mode(WIFI_AP_STA)   -access point and station connected to another access point

bool wifi_change = false; // para detectar cambios en la conexión esta en false por defecto

unsigned long previousMillisWIFI = 0; // temposizacion en modo WIFI
unsigned long previousMillisAP = 0;   // temporizacion en modo AP
unsigned long intervalWIFI = 30000;   // 30 segundos

// variable para usar hostname ejemplo http://adminesp32.local
const char *esp_hostname = device_name;
// el modo ap es provisional para configurar la red
void startAP()
{
    log("INFO", "wifi.hpp", "Iniciando Modo AP ...");
    WiFi.mode(WIFI_AP);
    WiFi.disconnect(true, true); // apagamos el wifi y borramos la memoria
    vTaskDelay(20);              // es como un delay
    WiFi.softAPConfig(ap_IPv4, ap_IPv4, ap_subnet);
    WiFi.hostname(esp_hostname);
    WiFi.softAP(ap_ssid, ap_password, ap_chanel, ap_visibility, ap_connect);
    log("INFO", "wifi.hpp", "WiFi en modo AP" + String(ap_ssid) + " - IP " + ipStr(WiFi.softAPIP()));
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure); // escucha las conexiones por DNS
    dnsServer.start(DNSSERVER_PORT, "*", ap_IPv4);
    wifi_app = WIFI_AP;
}
void startClient()
{
    log("INFO", "wifi.hpp", "Iniciando en Modo Estacion...");
    WiFi.mode(WIFI_AP_STA);
    if (wifi_ip_static)
    {
        if (!WiFi.config(CharToIP(wifi_ipv4), CharToIP(wifi_gateway), CharToIP(wifi_subnet), CharToIP(wifi_dns_primary), CharToIP(wifi_dns_secondary)))
        {
            log("ERROR", "wifi.hpp", "Falló la configuración en Modo Estación.");
        }
    }
    WiFi.hostname(esp_hostname);
    // WiFi.begin(wifi_ssid, wifi_password);
    wifiMulti.addAP(wifi_ssid, wifi_password);
    wifiMulti.addAP(wifi_ssid2, wifi_password2);
    log("INFO", "wifi.hpp", "Conectando a red WiFi...");
    byte b = 0;
    while (wifiMulti.run() != WL_CONNECTED && b < 60)
    {
        b++;
        log("WARNING", "wifi.hpp", "Intentando conexión a WiFi...");
        vTaskDelay(250);
        digitalWrite(WIFILED, HIGH);
        vTaskDelay(250);
        digitalWrite(WIFILED, LOW);
        blinkSingle(100, WIFILED);
    }
    if (wifiMulti.run() == WL_CONNECTED)
    {
        log("INFO", "wifi.hpp", "Conectado a la Red Wifi: " + String(WiFi.SSID()));
        log("INFO", "wifi.hpp", "WiFi potencia (" + String(WiFi.RSSI()) + ") dBm, IPv4 " + ipStr(WiFi.localIP()));
        blinkRandomSingle(10, 100, WIFILED);
        wifi_app = WIFI_AP_STA;
        wifi_change = true;
    }
    else
    {
        log("ERROR", "wifi.hpp", "WiFi no conectado : (");
        blinkRandomSingle(10, 100, WIFILED);
        wifi_change = true;
        startAP();
    }
}

//-----------------------------------------------------
// Setup
//-----------------------------------------------------
void wifi_setup()
{
    WiFi.disconnect(true); // nos deconectamos de cualquier conexion existente
    // 1) Si esta activo el Modo Cliente
    if (wifi_mode)
    {
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0, 0);
        OLED.println("CONECTANDO A RED WIFI");
        OLED.display();
        startClient();
        if (WiFi.status() == WL_CONNECTED)
        {
            log("INFO", "wifi.hpp", "WiFi Modo Estación");
        }
    }
    else
    {
        // 2) Caso contrario en Modo APOLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0, 0);
        OLED.println("INICIANDO MODO AP");
        OLED.display();
        // startClient();
        startAP();
        log("INFO", "wifi.hpp", "WiFi en Modo AP");
    }
    // iniciamos el hostname broadcast
    if (wifi_app == WIFI_AP_STA || wifi_app == WIFI_AP)
    {
        if (MDNS.begin(esp_hostname))
        {
            MDNS.addService("http", "tcp", 80);
        }
    }
}
//------------------------------------------------------------------------
// Loop Modo Estacion
//------------------------------------------------------------------------
byte w = 0;
void wifiLoop()
{
    
    unsigned long currentMillis = millis();
    if (WiFi.status() != WL_CONNECTED && (currentMillis - previousMillisWIFI >= intervalWIFI))
    {
        w++;
        blinkSingle(100, WIFILED);
        WiFi.disconnect(true);
        WiFi.reconnect(); //----------------------------------------ver si jala
        previousMillisWIFI = currentMillis;
        // 2= 1 minuto
        if (w == 2)
        {
            log("INFO", "wifi.hpp", "Cambiando a Modo AP");
            wifi_change = true;
            w = 0;
            startAP();
        }
        else
        {
            log("WARNING", "wifi.hpp", "Redes registradas anteriormente estan fuera de alcance");
        }
    }
    else
    {
        blinkSingleAsy(10, 500, WIFILED);
    }
}
//----------------------------------------------------------------------
// Loop Modo AP
//----------------------------------------------------------------------
byte a = 0;
void wifiAPLoop()
{
    blinkSingleAsy(50, 1000, WIFILED); // pestañeo mucho mas lento
    dnsServer.processNextRequest();    // Portal captivo DNS para conectarse al dispositivo
    unsigned long currentMillis = millis();
    if ((currentMillis - previousMillisAP >= intervalWIFI) && wifi_change)
    {
        a++;
        previousMillisAP = currentMillis;
        // 20 es igual a 10 minutos
        if (produccion)
        { // en el archivo globales linea 88 aprox
            if (a == 20)
            { // paso 10 minutos
                log("INFO", "wifi.hpp", "Cambiando a Modo Estacion.");
                wifi_change = false;
                a = 0;
                startClient();
            }
        }
        else
        {
            if (a == 2)
            { // paso 1 minuto
                log("INFO", "wifi.hpp", "Cambiando a Modo Estacion");
                wifi_change = false;
                a = 0;
                startClient();
            }
        }
    }
}

// la variable wifi_change es cuando se esta intentando conectar en modo cliente
//  https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
//  https://randomnerdtutorials.com/esp32-wifimulti/
