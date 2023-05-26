//esp32a_definitions_variables.hpp
//Aqui se encuentran todas las variables globales del proyecto

//---------------------------------------------------------
//Definiciones
//---------------------------------------------------------
#define RELAY1 33     //GPIO33 para el relay 
#define RELAY2 15     //GPIO15 para el relay 
#define DIMMER 25     //GPIO25 se modificara por comando por eso no se define
#define WIFILED 12      //GPIO12 LED INDICADOR WIFI
#define MQTTLED 13      //GPIO13 LED INDICADOR MQTT
#define BUZZER  32      //GPIO32 ZUMBADOR
//--------------------------------------------------------------------------
// Calcular la capacidad del JSON
// Asistente ArduinoJson: https://arduinojson.org/v6/assistant/
// Documentacion: https://arduinojson.org/v6/api/json/deserializejson/
//--------------------------------------------------------------------------
const size_t capacitySettings = 1024*5;  //1024*5 = 5KB
StaticJsonDocument<2000> board; //verificar con mas capacidad pare espnow
String jsonStringApi = "{\"info\":\"null\"}";
//------------------------------------------------------------------
// Versión de firmware desde las variables de entorno platformio.ini
//------------------------------------------------------------------
#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)
String device_fw_version = ESCAPEQUOTE(BUILD_TAG); //lo toma de platformio
//------------------------------------------------------------------------
//Version de Hardware y Fabricante
//------------------------------------------------------------------------
#define device_hw_version   "ADMINESP32 v1 00000000"    //version hardware
#define device_manufacture  "POVNURB@HOTMAIL.COM"       //fabricante

//------------------------------------------------------------------------
//Zona configuracion Dispositivo
//------------------------------------------------------------------------
int         device_restart;         //Numero de reinicios
char        device_id[30];          //ID del dispositivo
char        device_name[30];        //Nombre del dispositivo
char        device_user[15];        //Usuario para acceso al servidor web
char        device_password[15];    //Contraseña del usuario servidor web
int         RESTART= 211179;   //= 211179;   //contraseña de 6 digitos para reiniciar el sistema (211179) Guarda informacion aun por hacer ------------
int         RESTORE= 21111979;  //= 21111979;  //Contraseña de 8 digitos para reinicar de fabrica  (21111979) borra toda la informacion ---------------
bool        REINICIAR = false;
bool        DEFABRICA = false;
//----------------------------------------------------------------------------
//Zona configuraciín WIFI modo CLiente
//----------------------------------------------------------------------------

int         wifi_app = WIFI_AP_STA;     //WIFI_AP_STA         //WIFI modo WIFI_AP& WIFI_STA
bool        wifi_mode;                  //Uso de Modo AP false, cliente true
char        wifi_ssid[63];              //Nombre de la red Wifi a conectar
char        wifi_password[63];          //Contraseña de la Red Wifi a conectar
// dos redes para conectarse
char        wifi_ssid2[63];             //Nombre de la red Wifi a conectar o red de celular
char        wifi_password2[63];         //Contraseña de la Red Wifi a conectar
//si ip estaticas se usan las siguientes variables
bool        wifi_ip_static;             //true IP estática, false DHCP
char        wifi_ipv4[16];              //Cuando ip estatica true se usa esta ip
char        wifi_gateway[16];           //direccion Gateway
char        wifi_subnet[16];            //Dir ipv4 subred
char        wifi_dns_primary[16];       //Dir Ipv4 DNS primario
char        wifi_dns_secondary[16];     //Dir ipv4 DNS secundario
int         wifi_chanel;                //mostrará el canal donde se tiene que comunicar el protocolo espnow
//----------------------------------------------------------------
//Zona configuracion WIFI modo AP
//----------------------------------------------------------------
char        ap_ssid[63];                //Nombre del SSID modo AP maximo 63
char        ap_password[63];            //Contraseña del modo AP Minimo 8 NULL red abierta
int         ap_chanel;                  //Canal AP 1-13
int         ap_visibility;              //Es visible o no el AP (0 - visible 1-oculto)
int         ap_connect;                 //Numero de conexiones al dispositivo max 8
//---------------------------------------------------------------------------------------------
//Zona configuración MQTT
//---------------------------------------------------------------------------------------------
bool    mqtt_enable;                //Habilitar MQTT Broker true / false
char    mqtt_server[39];            //Servidor del MQTT Broker
int     mqtt_port;                  //Puerto servidor MQTT Broker
bool    mqtt_retain;                //Habilitar mensajes retenidos
int     mqtt_qos;                   //Calidad del servicio
char    mqtt_id[30];                //Cliente ID MQTT Broker
char    mqtt_user[30];              //Usuario MQTT broker
char    mqtt_password[39];          //Contraseña del MQTT Broker
bool    mqtt_clean_sessions;        //Habilitar sesiones limpias
char    mqtt_willTopic[150];        //Will topico
char    mqtt_extraTopic[150];       //otro topico que dejara ver informacion de otro dispositivo
char    mqtt_willMessage[63];       //will mensaje
int     mqtt_willQoS;               //will calidad de servicio 0,1,2
bool    mqtt_willRetain;            //will mensaje retenido
bool    mqtt_time_send;             //Habilitar envio de datos
int     mqtt_time_interval;         //Tiempo de envio por MQTT en segundos
bool    mqtt_status_send;           //Habilitar envio de estados
//------------------------------------------------------------------------
//Zona EEPROM para contador de reinicios
//------------------------------------------------------------------------++++++++++++++++++++++++++++++++++++++++++++
bool    produccion = false;                             //mientras el programa este en derarrollo el valor sera false
#define Start_Address 0                                 //posicion de inicio en la EEPROM
#define Restart_Address Start_Address + sizeof(int)     //Dirección del dato en el EEPROM
//------------------------------------------------------------------------
// Zona de Temperaturas 
//-------------------------------------------------------------------------
#define DHTPIN 2    
float temp_cpu,humedad, tempC;                      // Temperatura del CPU en °C
float min2 = 100;
float max2;
/**
 * Correspondera a configuracion especial
*/
//------------------------------------------------------------------------------
//Zona RELAY
//------------------------------------------------------------------------------
char        R_NAME1[20];        //nombre de los Relays
bool        R_STATUS1;          //Estado del pin
bool        R_TIMERON1;         //indica si se activa el timer del relevador1
int         R_TIMER1;           //contador regresivo por minuto
bool        R_LOGIC1;           //por si trabaja energizado
String      R_DESCRIPTION1;     //indica que es lo que controla   
bool        TEMPORIZADOR1;      //indica si hay un control por tiempo
String      TIMEONRELAY1;       //indica a que horas se prende
String      TIMEOFFRELAY1;      //indica a que horas se apaga
bool        programado1;        //muestra si esta programado el relay1 con fecha

char        R_NAME2[20];        //nombre de los Relays
bool        R_STATUS2;          //Estado del pin
bool        R_TIMERON2;         //indica si se activa el timer del relevador2
int         R_TIMER2;           //contador regresivo por minuto 
bool        R_LOGIC2;           //por si trabaja energizado
String      R_DESCRIPTION2;     //breve descripcion 
bool        TEMPORIZADOR2;      //indica si hay un control por tiempo
String      TIMEONRELAY2;       //indica a que horas se prende
String      TIMEOFFRELAY2;      //indica a que horas se apaga
bool        programado2;        //muestra si esta programado el relay2 con fecha

bool        BUZZER_STATUS;      //estado del buzzer
//----------------------------------------------------------------------------
//Zona PWM
//-----------------------------------------------------------------------------

const int   freq = 1000;        //frecuancia de trabajo
const int ledChannel = 0;        //canal
const int resolution = 8;        // resolucion de 8 bits para que llegue a 255
int       dim;                  //valor del dimer que se le pasa al canal

//----------------------------------------------------
//   Zona time
//----------------------------------------------------
ESP32Time rtc;              //Clase ESP32Time

bool time_ajuste;           // 1- Manual , 0- Automático internet SRV NTP
char time_date[18];         // 2022-09-07T23:47
long time_z_horaria;        // Zona Horaria GMT 0 = 0 -GMT +1 = 3600 - GMT -1 = -3600
char time_server[39];       // Servidor NTP Elemplo: time.mist.gov

int time_hr;                // Hora 0 -23
int time_mn;                // Minutos 0 - 59
int time_sc;                // Segundos 0-59
int time_dy;                // Días 1 - 31
int time_mt;                // Meses 1- 12
int time_yr;                // Año 2023
// NTP Server
WiFiUDP ntpUDP;
NTPClient ntpClient(ntpUDP);
//-----------------------------------------------------------------
// Zona ESPNOW
//-----------------------------------------------------------------
//cambiar el nombre de las alarmas demanera locak    
String cambio11,cambio21,cambio31,cambio41,cambio51,cambio61,cambio71,cambio81;
String cambio12,cambio22,cambio32,cambio42,cambio52,cambio62,cambio72,cambio82;
String cambio13,cambio23,cambio33,cambio43,cambio53,cambio63,cambio73,cambio83;
String cambio14,cambio24,cambio34,cambio44,cambio54,cambio64,cambio74,cambio84;

//-----------------------------------------------------------------
// Zona Otras
//-----------------------------------------------------------------
bool    normalizar;                                     // ayuda al control de Normalizar alarmas
bool    prueba;                                         // envia por mptt si se esta realizando una prueba de todas las alarmas
//zona del sensor DS18B20
//float temperaturec, temperatureF;                     //para la temperatura °C y °F DS18B20
//------------------------------------------------------------------------
// OLED
// -----------------------------------------------------------------------
#define SCREEN_WIDTH 128    // Ancho   128 o 128
#define SCREEN_HEIGHT 32    // Alto     32 0 64

Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT);
// -----------------------------------------------------------------------
// Activacion de Alarmas de manera local atravez de la interupcion (boton 34)
// -----------------------------------------------------------------------
#define BOTON2 34               // Botón de interrupción de alarma
volatile bool togle = true;
#define tiempoDeRebote 250                                //sirve para omitir rebotes
volatile unsigned long tiempoDeInterrupcion= 0; //sirve para omitir rebotes en sonido
//void IRAM_ATTR activarAlarmas();


