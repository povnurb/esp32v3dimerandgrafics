// http://192.168.1.68/api/index
// para ver los datos en el explorador
// se modifican los datos en reset.hpp
#include <Arduino.h>
#include <EEPROM.h> //aqui se guardara el contador de reinicios
#include <SPIFFS.h> //donde se tomaran los metodos para almacenar informacion
#include <WiFi.h>   //metodos para la conexion del dispositivo a una red wifi
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h> //metodos para manejar archivos JSON
#include <esp_now.h>     //libreria de protocolo esp_now que permite la comunicacion entre dispositivos esp
#include <TimeLib.h>
#include <Ticker.h> //para los timer de los relays

//******************************************************************************************************
// Configuracion del sensor de temperatura DS18B20
// #include <OneWire.h>// descomentar para el sensor de temperatura DS18B20
// #include <DallasTemperature.h>// descomentar para el sensor de temperatura DS18B20
// #define ONE_WIRE_BUS 13 //no compatible con la tarjeta por eso hay que cambiar el pin sugerencia el 4
// OneWire oneWire(ONE_WIRE_BUS);
// DallasTemperature sensor(&oneWire)
//*******************************************************************************************************

// Libreria para el RTC del ESP32
#include <ESP32Time.h> //ayuda a calcular la fecha
// Libreria para NTP
#include <NTPClient.h> //servidor que nos da la hora local

/*Librerias agregadas para nuevas funcionalidades (LALO)*/
#include <Adafruit_Sensor.h>    //by Adafruit
#include <DHT.h>                //by Adafruit
#include <Adafruit_I2CDevice.h> //by Adafruit
#include <Adafruit_GFX.h>       // para la pantalla de lcd
#include <Adafruit_SSD1306.h>   //by Adafruit

//-------------------------------------------------------
// Archivos *.hpp -fragmentar el código
//-------------------------------------------------------
#include "globales.hpp" //antes "esp32a_definitions_variables.hpp"
#include "alarmas.hpp"
#include "functions.hpp" //antes "esp32a_functions.hpp"
#include "settingsSave.hpp"
#include "settingsReset.hpp" // no
#include "settingsRead.hpp"  //antes "esp32a_settings.hpp"
#include "wifi.hpp"
#include "espnow.hpp"
#include "mqtt.hpp"
#include "server.hpp"
#include "alarmremote.hpp"

#include "tareas.hpp"

// put function declarations here:

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  log("INFO", "main.cpp", "Iniciando el setup");
  if (produccion) // cuando ya este en funcionamiento se le pone true
  {               // en el archivo globales linea 88 aprox
    // informacion de la libreria EEPROM
    EEPROM.begin(256);                           // se define con 256 bites
    EEPROM.get(Restart_Address, device_restart); // se pone en la variable devie_restart
    device_restart++;                            // se aumenta en 1
    EEPROM.put(Restart_Address, device_restart); // se escribe ese valor en la EEprom
    EEPROM.commit();
    EEPROM.end(); // se cierra la EPROM
    log("INFO", "main.cpp", "cantidad de Reinicios: " + String(device_restart));
  }
  if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Dirección 0x3C
    Serial.println("OLED no encontrado");
    // while(true);
  }
  OLED.clearDisplay();
  dht.begin(); // su funcionalidad se encuentra en functions.hpp
  // inicializar el SPIFFS
  if (!SPIFFS.begin(true))
  { // si no se puede inicializar ahora el (true) sirve para darle formato
    log("ERROR", "main.cpp", "Falló la inicializacion del SPIFFS");
    while (true)
      ; // el dispositivo no continua hasta que logre iniciar el SPIFFS
  }
  // traer las configuraciones resientes desde json
  if (!settingsRead())
  {                 // se crea el archivo con los valores iniciales de reset
    settingsSave(); // aqui guardamos
    log("INFO", "main.cpp", "Información general Salvada.");
    // encaso que el settingsRead se a podido leer no se guarda nada para no andar tocando la memoria
  }
  // settingsSave(); //aqui mostramos la información
  // SPIFFS.remove("/settings.json"); //para pruebas ya que se guarda la configuracion y con esto borramoes el SPIFFS
  // configuraciones recientes especiales desde json
  if (!especialRead())
  {
    especialSave();
    log("INFO", "main.cpp", "Información de actuadores Salvada.");
  }
  // especialSave(); //aqui mostramos la información
  // Debuelve la lista de carpetas y archivos del SPIFFS ONLYDEBUG
  // listDir(SPIFFS, "/", 0);
  gpioDefine();   // definicion de los pines y setear (setup)
  setupAlarmas(); // definicion de las alarmas
  // Paso de los estados de los pines de los Relays
  setOnOffSingle(RELAY1, R_STATUS1);
  setOnOffSingle(RELAY2, R_STATUS2);
  // Pasar el dato del dimmer
  ledcWrite(ledChannel, dim * 2.55); // dim va de 0 a 100
  // inicial el wifi
  wifi_setup();
  // seteamos el time
  timeSetup();
  actualizaciontime.attach(1, actualizaTime); // actualizara el tiempo cada 1 segundo
  // iniciamos el servidor
  initServer();
  // crear Tarea Reconexión WIFI
  xTaskCreate(TaskWifiReconnect, "TaskWifiReconnect", 1024 * 6, NULL, 2, NULL);
  // crear Tarea de reconexión MQTT
  xTaskCreate(TaskMqttReconnect, "TaskMqttReconnect", 1024 * 6, NULL, 2, NULL);
  // LED MQTT Task
  xTaskCreate(TaskMQTTLed, "TaskMQTTLed", 1024 * 2, NULL, 1, NULL);
  // crear Tarea mostrar pantalla LCD
  xTaskCreate(TaskLCD, "TaskLCD", 1024 * 2, NULL, 1, NULL);
  // crear Tarea cambio de estado rele de manera local
  xTaskCreate(Taskrelcambio, "Taskrelcambio", 1024 * 2, NULL, 1, NULL);
  // crea tarea para ponerle horario a los relevadore
  xTaskCreate(TaskTimeRele, "TaskTimeRele", 1024 * 2, NULL, 1, NULL);
  // crear tarea estado de las alarmas
  // xTaskCreate(TaskAlarms, "TaskAlarms", 1024 * 2, NULL, 1, NULL);

  // setupEspnow();                        // hasta que este wifi configurado pero hace falta su task
  // esp_now_register_recv_cb(OnDataRecv); // no se si va o no va
  //  timer de los relays
  //-------------------------------Interrupciones-------------------------------------------------------
  /*pinMode(ACTRELE1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ACTRELE1), funcDeInterrupcion1, RISING); // Flanco de subida
  pinMode(ACTRELE2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ACTRELE2), funcDeInterrupcion2, FALLING); // Flanco de bajada*/
}

void loop()
{
  ctrlRelays();           // checa el estado de los relays para prenderlos o apagarlos
  statusAlarmVariables(); // actualiza el estado de las variables de las alarmas
  contadorAlarmas();      // cuenta la cantidad de alarmas y le pone la fecha
  // setupPinActivarAlarmas(); o crear una tarea o interrupcion
}

// put function definitions here:
