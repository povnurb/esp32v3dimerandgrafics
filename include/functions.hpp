#include "blinkleds.hpp"
// archivo de funciones que se ocuparán en el programa
// especialSave() funcion que usaremos para guardar la configuracion cuando reiniciemos por mqtt

Ticker timerRelay1;       // para el manejo del relay1
Ticker timerRelay2;       // para el manejo del relay2
Ticker actualizaciontime; // actualizara el valor de la variable time cada 10 segundos

// falta R_TIMER1 y R_TIMER2 para contabilizar el tiempo que lleva encendido

String releTime(); // es el comparador para encender o apagar los relevadores
void settingsReset();
void especialReset();
bool especialSave();
bool settingsSave();
void offRelay1();
void offRelay2();
void setDyMsYr();
void wsMessageSend(String msg, String icon, String Type);

/**
 * void log Genera mensajes personalizados en el puerto Serial
 */
void log(String type, String archivo, String msg)
{ //[INFO][ARCHIVO] msg
    Serial.println("[" + type + "]" + "[" + archivo + "] " + msg);
}
//-------------------------------------------------------------
// Definir la plataforma
// Optiene la Plataforma de hardware
//-------------------------------------------------------------
String platform()
{
#ifdef ARDUINO_ESP32_DEV
    return "ESP32";
#endif
}
//----------------------------------------------------------------
// funcion de HEX a String osea de hexadecimal a String
//----------------------------------------------------------------
String HexToStr(const unsigned long &h, const byte &l)
{ // vairable de tipo byte - l es un puntero
    String s;
    s = String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    // Serial.println("s: "+ s);
    return s;
}
//-----------------------------------------------------------------------
// Crea un ID unico desde la direccion MAC
// Retorna los ultimos 4 Bytes del MAC pero rotados
//----------------------------------------------------------------------
String UniqueID()
{
    char uniqueid[15];
    uint64_t chipid = ESP.getEfuseMac();
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(uniqueid, 15, "%04X", chip); // formato de entrega
    // log("INFO","functions.hpp","uniqueid: "+ String(uniqueid));
    return uniqueid;
}

String DeviceID()
{
    String d = String(platform()) + HexToStr(ESP.getEfuseMac(), 8) + String(UniqueID());
    // log("INFO","functions.hpp","DeviceID: "+ d); //muestra en consola
    return String(platform()) + HexToStr(ESP.getEfuseMac(), 8) + String(UniqueID());
}
//------------------------------------------------------------
// para el mDNS es la siguiente funcion extractNumbers
// quita los numeros de una cadena
//------------------------------------------------------------
String extractNumbers(String e)
{
    String inputString = e;
    inputString.toLowerCase(); // todo a minuscula
    String outString = "";     // variable que estrega esta funcion
    int len = inputString.length();
    for (size_t i = 0; i < len; i++)
    {
        if (!isDigit((char)inputString[i]))
        {
            outString += (char)inputString[i]; // cuando sea un caracter lo agrega a la salida
        }
    }
    return outString;
}

//-----------------------------------------------------------------
// el path para el mqtt (el topic) ejemplo:  emqx/ESP32XXXXXXXX/status = true si esta conectado y false desconectado
//-----------------------------------------------------------------
String PathMqttTopic(String topic)
{
    return String(String(mqtt_user) + "/" + String(mqtt_id) + "/" + topic);
}

// Definicion de los pines GPIO
/**
 * #define RELAY1 33     //GPIO33 para el relay pero ya no se va a utilizar ya que lo modificare por comandos MQTT
#define RELAY2 15     //GPIO15 para el relay pero ya no se va a utilizar ya que lo modificare por comandos MQTT
#define DIMMER 25     //GPIO25 se modificara por comando por eso no se define
#define WIFILED 12      //GPIO12 LED INDICADOR WIFI
#define MQTTLED 13      //GPIO13 LED INDICADOR MQTT
#define BUZZER  32      //GPIO32 ZUMBADOR
*/
void gpioDefine()
{
    // PINES
    pinMode(WIFILED, OUTPUT);
    pinMode(MQTTLED, OUTPUT);
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(ALARM_PIN1, INPUT_PULLUP);
    pinMode(ALARM_PIN2, INPUT_PULLUP);
    pinMode(ALARM_PIN3, INPUT_PULLUP);
    pinMode(CLIMAA, INPUT_PULLUP);
    pinMode(CLIMAB, INPUT_PULLUP);
    pinMode(ACTRELE1, INPUT_PULLUP); // GPIO 35
    pinMode(ACTRELE2, INPUT_PULLUP); // GPIO 32
    // PWM (DIMMER) dimerizar el led o foco o motor
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(DIMMER, ledChannel);
    // BUZZER
    pinMode(BUZZER, OUTPUT);
    // poner todos lo configurado en 0 o apagado
    setOffSingle(WIFILED);
    setOffSingle(MQTTLED);
    setOffSingle(RELAY1);
    setOffSingle(RELAY2);
    setOffSingle(BUZZER);
    ledcWrite(ledChannel, 0); // puede ir de 0 a 255 ya que la resolucion es de 8 bits
}
// -------------------------------------------------------------------
// Convierte un char a IP
// -------------------------------------------------------------------
uint8_t ip[4]; // Variable función convertir string a IP
IPAddress CharToIP(const char *str)
{ // convierte de char a IP
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// -------------------------------------------------------------------
// Retorna IPAddress en formato "n.n.n.n" osea de IP a String
// -------------------------------------------------------------------
String ipStr(const IPAddress &ip)
{
    String sFn = "";
    for (byte bFn = 0; bFn < 3; bFn++)
    {
        sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
    }
    sFn += String(((ip >> 8 * 3)) & 0xFF);
    return sFn;
}

//----------------------------------------------------------------------------
// maneja los relay, y el buzzer (solo modifica y guarda valores falta la activacion constante o loop)
// OnOffRelays(command)     mensajes desde el broker
// Funcion para operar los Relays de forma por MQTT.
// ejemplo tópico: lalo/ESP3263A7DBCC2CC1/command
// {"protocol":"MQTT", "output": "RELAY1", "value":true }
//----------------------------------------------------------------------------
bool OnOffRelays(String command)
{
    DynamicJsonDocument JsonCommand(320);
    deserializeJson(JsonCommand, command);
    log("INFO", "functions.hpp", "Comando enviado desde: " + JsonCommand["protocol"].as<String>() + " <=> " + JsonCommand["output"].as<String>() + " <=> " + JsonCommand["value"].as<String>());

    if (JsonCommand["value"] == true)
    {
        if (JsonCommand["output"] == R_NAME1)
        {
            if (R_LOGIC1)
            {
                digitalWrite(RELAY1, HIGH);
                R_STATUS1 = true;
            }
            else
            {
                digitalWrite(RELAY1, LOW);
                R_STATUS1 = false;
            }
            return true;
        }
        else if (JsonCommand["output"] == R_NAME2)
        {
            if (R_LOGIC2)
            {
                digitalWrite(RELAY2, HIGH);
                R_STATUS2 = true;
            }
            else
            {
                digitalWrite(RELAY2, LOW);
                R_STATUS2 = false;
            }
            return true;
        }
        else if (JsonCommand["output"] == "BUZZER")
        {
            digitalWrite(BUZZER, HIGH);
            BUZZER_STATUS = true;
            return true;
        }
        else
        {
            log("ERROR", "functions.hpp", "Actuador No Reconocido " + JsonCommand["output"].as<String>());
            return false;
        }
    }
    else if (JsonCommand["value"] == false)
    {
        if (JsonCommand["output"] == R_NAME1)
        {
            if (R_LOGIC1)
            {
                digitalWrite(RELAY1, LOW);
                R_STATUS1 = false;
            }
            else
            {
                digitalWrite(RELAY1, HIGH);
                R_STATUS1 = true;
            }
            return true;
        }
        else if (JsonCommand["output"] == R_NAME2)
        {
            if (R_LOGIC2)
            {
                digitalWrite(RELAY2, LOW);
                R_STATUS2 = false;
            }
            else
            {
                digitalWrite(RELAY2, HIGH);
                R_STATUS2 = true;
            }
            return true;
        }
        else if (JsonCommand["output"] == "BUZZER")
        {
            digitalWrite(BUZZER, LOW);
            BUZZER_STATUS = false;
            return true;
        }
        else
        {
            log("ERROR", "functions.hpp", "Actuador No Reconocido" + JsonCommand["output"].as<String>());
            return false;
        }
    }
    else
    {
        log("WARNING", "functions.hpp", "Comando NO reconocido" + JsonCommand["value"].as<String>());
        return false;
    }
}
//----------------------------------------------------------------------------
// mensajes desde el broker
// funcion que modifica los valores de las variables del dispositivo
// ejemplo topico: lalo/ESP3263A7DBCC2CC1/especial
// {"protocol":"MQTT", "varible": "RELAY1", "value":"CERRADURA" }
// {"protocol":"MQTT", "varible": "RELAY2", "value":"PUERTA"}
//----------------------------------------------------------------------------
bool especial(String command)
{
    DynamicJsonDocument JsonCommand(320);
    deserializeJson(JsonCommand, command);
    log("INFO", "functions.hpp", "Comando enviado desde: " + JsonCommand["protocol"].as<String>() + " <=> " + JsonCommand["varible"].as<String>() + " <=> " + JsonCommand["value"].as<String>());

    if (JsonCommand["varible"] == R_NAME1)
    {
        strlcpy(R_NAME1, JsonCommand["value"], sizeof(R_NAME1));
        return true;
    }
    if (JsonCommand["varible"] == R_NAME2)
    {
        strlcpy(R_NAME2, JsonCommand["value"], sizeof(R_NAME2));
        return true;
    }
    if (JsonCommand["varible"] == R_LOGIC1)
    {
        R_LOGIC1 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == R_LOGIC2)
    {
        R_LOGIC2 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == R_DESCRIPTION1)
    {
        R_DESCRIPTION1 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == R_DESCRIPTION2)
    {
        R_DESCRIPTION2 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC1)
    {
        ALRM_LOGIC1 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC2)
    {
        ALRM_LOGIC2 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC3)
    {
        ALRM_LOGIC3 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC4)
    {
        ALRM_LOGIC4 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC5)
    {
        ALRM_LOGIC5 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME1)
    {
        ALRM_NAME1 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME2)
    {
        ALRM_NAME2 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME3)
    {
        ALRM_NAME3 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME4)
    {
        ALRM_NAME4 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME5)
    {
        ALRM_NAME5 = JsonCommand["value"].as<String>();
        return true;
    }
    /*
    if (JsonCommand["varible"] == "cambio11")
    {
        cambio11 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio21")
    {
        cambio21 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio31")
    {
        cambio31 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio41")
    {
        cambio41 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio51")
    {
        cambio51 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio61")
    {
        cambio61 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio71")
    {
        cambio71 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio81")
    {
        cambio81 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio12")
    {
        cambio12 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio22")
    {
        cambio22 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio32")
    {
        cambio32 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio42")
    {
        cambio42 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio52")
    {
        cambio52 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio62")
    {
        cambio62 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio72")
    {
        cambio72 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio82")
    {
        cambio82 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio13")
    {
        cambio13 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio23")
    {
        cambio23 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio33")
    {
        cambio33 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio43")
    {
        cambio43 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio53")
    {
        cambio53 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio63")
    {
        cambio63 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio73")
    {
        cambio73 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio83")
    {
        cambio83 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio14")
    {
        cambio14 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio24")
    {
        cambio24 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio34")
    {
        cambio34 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio44")
    {
        cambio44 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio54")
    {
        cambio54 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio64")
    {
        cambio64 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio74")
    {
        cambio74 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "cambio84")
    {
        cambio84 = JsonCommand["value"].as<String>();
        return true;
    }
    */
    if (JsonCommand["varible"] == "CONTRASEÑA")
    {
        // {"protocol":"MQTT", "varible": "CONTRASEÑA", "value":"211179" }
        if (JsonCommand["value"] == RESTART)
        {
            log("INFO", "functions.hpp", "Salvando archivo especial");
            REINICIAR = especialSave(); // falta aplicar si no puede salvar no reiniciará

            if (REINICIAR)
            {
                log("INFO", "functions.hpp", "Se va a reinicial el dispositivo por comandos en MQTT");
                Serial.println("[RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART]");
                Serial.println("[RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART]");
                // Esperar la transmisión de los datos seriales
                Serial.flush();
                // Reiniciar el ESP32
                ESP.restart();
            }
            else
            {
                log("ERROR", "functions.hpp", "Error en la funcion especialSave()");
                return false;
            }
        }
        else if (JsonCommand["value"] == RESTORE)
        {
            // {"protocol":"MQTT", "varible": "CONTRASEÑA", "value":"21111979" }
            log("INFO", "functions.hpp", "Se va a restaurar archivo de configuración especial a valores de fabrica");
            especialReset();
            log("INFO", "functions.hpp", "Se guarda archivo de configuración especial con valores de fabrica");
            DEFABRICA = especialSave(); // falta aplicar
            if (DEFABRICA)
            {
                log("INFO", "functions.hpp", "Se va a reiniciar el dispositivo");
                Serial.println("[RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE]");
                Serial.println("[RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE]");
                // Esperar la transmisión de los datos seriales
                Serial.flush();
                // Reiniciar el ESP32
                ESP.restart();
            }
            else
            {
                log("ERROR", "functions.hpp", "Error en la funcion especialSave()");
                return false;
            }
        }
        else
        {
            log("ERROR", "functions.hpp", "Contraseña Incorrecta");
            return false;
        }
        return true;
    }
    else
    {
        log("ERROR", "functions.hpp", "variable no reconocida " + JsonCommand["varible"].as<String>());
        return false;
    }
}
void muestraInfoMqtt(String command)
{
    DynamicJsonDocument JsonCommand(capacitySettings);
    deserializeJson(JsonCommand, command);
    log("INFO", "functions", "pendiente de mostrar informacion");
}
// void muestraInfoMqtt2(String command)
// { // muestra informacion del dispositivo remoto y lo almacena en las variables
//     DynamicJsonDocument JsonCommand(capacitySettings);
//     deserializeJson(JsonCommand, command); // lo que llega a command se manda al JsonCommand
//     log("INFO", "functions.hpp", "Informacion remota recibida");
//     // serializeJsonPretty(JsonCommand, Serial);
//     log("INFO", "functions.hpp", "Asignando valores a las variables REMOTAS ");
//     REMOTE_serial = JsonCommand["serial"].as<String>();
//     REMOTE_MACWiFi = JsonCommand["MACWiFi"].as<String>();
//     REMOTE_ALRM_STATUS1 = JsonCommand["especial"]["ALRM_STATUS1"].as<bool>();
//     REMOTE_ALRM_STATUS2 = JsonCommand["especial"]["ALRM_STATUS2"].as<bool>();
//     REMOTE_ALRM_STATUS3 = JsonCommand["especial"]["ALRM_STATUS3"].as<bool>();
//     REMOTE_ALRM_STATUS4 = JsonCommand["especial"]["ALRM_STATUS4"].as<bool>();
//     REMOTE_ALRM_STATUS5 = JsonCommand["especial"]["ALRM_STATUS5"].as<bool>();
//     REMOTE_ALRM_STATUS6 = JsonCommand["especial"]["ALRM_STATUS6"].as<bool>();
//     REMOTE_ALRM_STATUS7 = JsonCommand["especial"]["ALRM_STATUS7"].as<bool>();
//     REMOTE_ALRM_NAME1 = JsonCommand["especial"]["ALRM_NAME1"].as<String>();
//     REMOTE_ALRM_NAME2 = JsonCommand["especial"]["ALRM_NAME2"].as<String>();
//     REMOTE_ALRM_NAME3 = JsonCommand["especial"]["ALRM_NAME3"].as<String>();
//     REMOTE_ALRM_NAME4 = JsonCommand["especial"]["ALRM_NAME4"].as<String>();
//     REMOTE_ALRM_NAME5 = JsonCommand["especial"]["ALRM_NAME5"].as<String>();
//     REMOTE_ALRM_NAME7 = JsonCommand["especial"]["ALRM_NAME7"].as<String>();
//     REMOTE_ALRM_NAME7 = JsonCommand["especial"]["ALRM_NAME7"].as<String>();
//     REMOTE_ALRM_LOGIC1 = JsonCommand["especial"]["ALRM_LOGIC1"].as<bool>();
//     REMOTE_ALRM_LOGIC2 = JsonCommand["especial"]["ALRM_LOGIC2"].as<bool>();
//     REMOTE_ALRM_LOGIC3 = JsonCommand["especial"]["ALRM_LOGIC3"].as<bool>();
//     REMOTE_ALRM_LOGIC4 = JsonCommand["especial"]["ALRM_LOGIC4"].as<bool>();
//     REMOTE_ALRM_LOGIC5 = JsonCommand["especial"]["ALRM_LOGIC5"].as<bool>();
//     REMOTE_ALRM_LOGIC6 = JsonCommand["especial"]["ALRM_LOGIC6"].as<bool>();
//     REMOTE_ALRM_LOGIC7 = JsonCommand["especial"]["ALRM_LOGIC7"].as<bool>();
//     REMOTE_ALRM_TON1 = JsonCommand["especial"]["ALRM_TON1"].as<String>();
//     REMOTE_ALRM_TON2 = JsonCommand["especial"]["ALRM_TON2"].as<String>();
//     REMOTE_ALRM_TON3 = JsonCommand["especial"]["ALRM_TON3"].as<String>();
//     REMOTE_ALRM_TON4 = JsonCommand["especial"]["ALRM_TON4"].as<String>();
//     REMOTE_ALRM_TON5 = JsonCommand["especial"]["ALRM_TON5"].as<String>();
//     REMOTE_ALRM_TON6 = JsonCommand["especial"]["ALRM_TON6"].as<String>();
//     REMOTE_ALRM_TON7 = JsonCommand["especial"]["ALRM_TON7"].as<String>();
//     REMOTE_ALRM_TOFF1 = JsonCommand["especial"]["ALRM_TOFF1"].as<String>();
//     REMOTE_ALRM_TOFF2 = JsonCommand["especial"]["ALRM_TOFF2"].as<String>();
//     REMOTE_ALRM_TOFF3 = JsonCommand["especial"]["ALRM_TOFF3"].as<String>();
//     REMOTE_ALRM_TOFF4 = JsonCommand["especial"]["ALRM_TOFF4"].as<String>();
//     REMOTE_ALRM_TOFF5 = JsonCommand["especial"]["ALRM_TOFF5"].as<String>();
//     REMOTE_ALRM_TOFF6 = JsonCommand["especial"]["ALRM_TOFF6"].as<String>();
//     REMOTE_ALRM_TOFF7 = JsonCommand["especial"]["ALRM_TOFF7"].as<String>();
//     REMOTE_ALRM_CONT1 = JsonCommand["especial"]["ALRM_CONT1"].as<int>();
//     REMOTE_ALRM_CONT2 = JsonCommand["especial"]["ALRM_CONT2"].as<int>();
//     REMOTE_ALRM_CONT3 = JsonCommand["especial"]["ALRM_CONT3"].as<int>();
//     REMOTE_ALRM_CONT4 = JsonCommand["especial"]["ALRM_CONT4"].as<int>();
//     REMOTE_ALRM_CONT5 = JsonCommand["especial"]["ALRM_CONT5"].as<int>();
//     REMOTE_ALRM_CONT6 = JsonCommand["especial"]["ALRM_CONT6"].as<int>();
//     REMOTE_ALRM_CONT7 = JsonCommand["especial"]["ALRM_CONT7"].as<int>();
//     REMOTE_R_NAME1 = JsonCommand["especial"]["R_NAME1"].as<String>();
//     REMOTE_R_NAME2 = JsonCommand["especial"]["R_NAME2"].as<String>();
//     REMOTE_R_STATUS1 = JsonCommand["especial"]["R_STATUS1"].as<bool>();
//     REMOTE_R_STATUS2 = JsonCommand["especial"]["R_STATUS2"].as<bool>();
//     REMOTE_R_LOGIC1 = JsonCommand["especial"]["R_LOGIC1"].as<bool>();
//     REMOTE_R_LOGIC2 = JsonCommand["especial"]["R_LOGIC2"].as<bool>();
// }

// -------------------------------------------------------------------
// Retorna segundos como "d:hh:mm"
// -------------------------------------------------------------------
String longTimeStr(const time_t &t)
{
    /*String s = String(t / SECS_PER_YEAR) + ':';
    if (year(t) < 10)
    {
        s += '0';
    }*/
    String s = String(t / SECS_PER_DAY) + ':';
    if (hour(t) < 10)
    {
        s += '0';
    }
    s += String(hour(t)) + ':';
    if (minute(t) < 10)
    {
        s += '0';
    }
    s += String(minute(t));
    /*s += String(minute(t)) + ':';
    if (second(t) < 10)
    {
        s += '0';
    }
    s += String(second(t));*/
    return s;
}
// -------------------------------------------------------------------
// Retorna la calidad de señal WIFI en % => 0 -100%
// -------------------------------------------------------------------
int getRSSIasQuality(int RSSI)
{
    int quality = 0;
    if (RSSI <= -100)
    {
        quality = 0;
    }
    else if (RSSI >= -50)
    {
        quality = 100;
    }
    else
    {
        quality = 2 * (RSSI + 100);
    }
    return quality;
}
// -------------------------------------------------------
// Sensor de temp interno CPU
//--------------------------------------------------------
#ifdef __cplusplus
extern "C"
{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
//---------------------------------------------------------
// Retorna la temperatura de CPU
//---------------------------------------------------------
float TempCPUValue()
{
    return temp_cpu = (temprature_sens_read() - 32) / 1.8;
}

//**************************************************************
/** Leer la temperatura del sensor DS19B20
 * void readSensor(){
 *  sensors.requestTemperatures();
 *  temperatureC= sensors.getTempCByIndex(0);
 *  temperatureF= sensors.getTempFByIndex(0);
 * }
 */
//****************************************************************

//----------------------------------------------------------------
// retorna el contenido del Body Enviado como JSON metodo POST/PUT
// lo que entiendo es que la data (informacion) llega de la pagina
// y la concatena ya que lo requiere la api
//----------------------------------------------------------------
String GetBodyContent(uint8_t *data, size_t len)
{
    String content = "";
    for (size_t i = 0; i < len; i++)
    {
        content.concat((char)data[i]);
    }
    return content;
}

// -------------------------------------------------------------------
// Retorna el Tipo de Encriptación según el codigo (0-1-2-3-4-5) para la wifi
// -------------------------------------------------------------------
String EncryptionType(int encryptionType)
{
    switch (encryptionType)
    {
    case (0):
        return "Open";
    case (1):
        return "WEP";
    case (2):
        return "WPA_PSK";
    case (3):
        return "WPA2_PSK";
    case (4):
        return "WPA_WPA2_PSK";
    case (5):
        return "WPA2_ENTERPRISE";
    default:
        return "UNKOWN";
    }
}
// ----------------------------------------------------------------------
// Función para reiniciar el dispositivo Global -> API, MQTT, WS
// ----------------------------------------------------------------------
void restart(String origin)
{
    log("INFO", "functions.hpp", "Dispositivo reiniciado desde: " + origin);
    Serial.println("[RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART]");
    Serial.println("[RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART][RESTART]");
    Serial.flush();
    ESP.restart();
}
// ----------------------------------------------------------------------
// Función para restablecer el dispositivo Global -> API, MQTT, WS
// ----------------------------------------------------------------------
void restore(String origin)
{
    log("INFO", "functions.hpp", "Reseteando configuraciones de fábrica...");
    settingsReset();
    log("INFO", "functions.hpp", "Reseteando archivo especial a valores de fábrica...");
    especialReset();
    log("INFO", "functions.hpp", "Salvando configuraciones de fábrica...");
    if (settingsSave())
    {
        log("INFO", "functions.hpp", "salvando nuevas configuraciones con valores de fábrica...");
        if (especialSave())
        {
            log("INFO", "functions.hpp", "salvando configuraciones especiales a valores de fábrica por comandos desde: " + origin);
            Serial.println("[RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE]");
            Serial.println("[RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE][RESTORE]");
            Serial.flush();
            ESP.restart();
        }
    }
}
//----------------------------------------------------------------------------
// maneja los relay desde la api y verificara el estado por minuto
// Funcion para operar los Relays de forma Global -> API
//----------------------------------------------------------------------------
// releTime
bool releprog1 = false;
bool releprog2 = false;
void ctrlRelays()
{
    if (programado1)
    { // si tiene hora de inicio programada el relay1
        if (TIMEONRELAY1 == releTime())
        {
            digitalWrite(RELAY1, HIGH);
            R_STATUS1 = true;
        }
        if (TIMEOFFRELAY1 == releTime())
        {
            digitalWrite(RELAY1, LOW);
            R_STATUS1 = LOW;
        }
    }
    if (programado2)
    { // si tiene hora de inicio programada el relay2
        if (TIMEONRELAY2 == releTime())
        {
            digitalWrite(RELAY2, HIGH);
            R_STATUS2 = true;
        }
        if (TIMEOFFRELAY2 == releTime())
        {
            digitalWrite(RELAY2, LOW);
            R_STATUS2 = LOW;
        }
    }

    if (R_TIMERON1 && !releprog1)
    {
        timerRelay1.attach(R_TIMER1, offRelay1);
        R_STATUS1 = true;
        releprog1 = true;
    }
    if (R_TIMERON2 && !releprog2)
    {
        timerRelay2.attach(R_TIMER2, offRelay2);
        R_STATUS2 = true;
        releprog2 = true;
    }

    if (R_LOGIC1)
    {
        digitalWrite(RELAY1, R_STATUS1);
    }
    else
    {
        digitalWrite(RELAY1, !R_STATUS1);
    }
    if (R_LOGIC2)
    {
        digitalWrite(RELAY2, R_STATUS2);
    }
    else
    {
        digitalWrite(RELAY2, !R_STATUS2);
    }
}
void offRelay1()
{
    R_TIMERON1 = false;
    digitalWrite(RELAY1, false);
    R_STATUS1 = false;
    timerRelay1.detach();
    releprog1 = false;
}
void offRelay2()
{
    R_TIMERON2 = false;
    digitalWrite(RELAY2, false);
    R_STATUS2 = false;
    timerRelay2.detach();
    releprog2 = false;
}
//--------------------------------------------------------------------------------
// Función para el dimmer dispositivo Global -> API, MQTT, WS
// ejemplo: {"protocol":"API", "output": "Dimmer", "value": 0-100}
//------------------------------------------------------------------------------
void dimmer(String dimmer)
{
    DynamicJsonDocument JsonDimmer(320);
    deserializeJson(JsonDimmer, dimmer);
    log("INFO", "functions.hpp", "Comando enviado desde: " + JsonDimmer["protocol"].as<String>() + " <=> " + JsonDimmer["output"].as<String>() + " <=> " + JsonDimmer["value"].as<String>() + " %");
    dim = JsonDimmer["value"].as<int>();
    if (dim > 100)
        dim = 100;
    if (dim < 0)
        dim = 0;
    if (settingsSave())
    {
        ledcWrite(ledChannel, dim * 2.55);
        // multiplicamos por 2.55*100 para llegar a 255 que sería el máximo a 8 bits = 3.3V
    }
}

// -------------------------------------------------------------------
// Fecha y Hora del Sistema
// -------------------------------------------------------------------
//------------------------------------------------------------------
// Setup de fecha y Hora Auto / Manual
//------------------------------------------------------------------
void timeSetup()
{ // verificar si esta tomando la fecha de manera automatica si no quitar la fecha automatica
    // y deja la pura manual

    setDyMsYr();

    if (time_ajuste)
    {
        rtc.setTime(time_sc, time_mn, time_hr, time_dy, time_mt, time_yr);
        log("INFO", "functions.hpp", "RTC set OK en Manual");
        // datos desde el Internet
    }
    else
    {
        if ((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA))
        {
            /* WiFi Conectada */
            ntpClient.begin();
            ntpClient.setPoolServerName(time_server); // servidor
            ntpClient.setTimeOffset(time_z_horaria);  // zona horaria
            ntpClient.update();
            log("INFO", "functions.hpp", "RTC set OK en Automatico (horario)");
        }
        else
        {
            /* Si no hay conexión a WiFi - No Internet */
            rtc.setTime(time_sc, time_mn, time_hr, time_dy, time_mt, time_yr);
            log("INFO", "functions.hpp", "RTC set OK Sin Conexión a internet");
        }
    }
}

// -------------------------------------------------------------------
// Función para seteo de Día, Mes y Año a las variables
// -------------------------------------------------------------------
void setDyMsYr()
{
    // 2022-09-07T23:47
    String str_date = time_date;
    time_sc = 0;
    time_mn = str_date.substring(14, 16).toInt(); // 47
    time_hr = str_date.substring(11, 13).toInt(); // 23
    time_dy = str_date.substring(8, 10).toInt();
    time_mt = str_date.substring(5, 7).toInt();
    time_yr = str_date.substring(0, 4).toInt(); // 2023
}

String getDateTime()
{

    char fecha[20];
    int dia = 0;
    int mes = 0;
    int anio = 0;
    int hora = 0;
    int minuto = 0;
    int segundo = 0;

    if (time_ajuste)
    { // Manual
        /* RTC */
        dia = rtc.getDay();
        mes = rtc.getMonth() + 1;
        anio = rtc.getYear();
        hora = rtc.getHour(true);
        minuto = rtc.getMinute();
        segundo = rtc.getSecond();
    }
    else
    { // Automatico
        // if((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA)){
        if ((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_AP_STA))
        {
            /* NTP */
            if (ntpClient.isTimeSet())
            {
                String formattedTime = ntpClient.getFormattedTime();
                // FORMAR FECHA DD-MM-YYYY DESDE EPOCH
                time_t epochTime = ntpClient.getEpochTime();
                struct tm *now = gmtime((time_t *)&epochTime);
                anio = now->tm_year + 1900;
                mes = now->tm_mon + 1;
                dia = now->tm_mday;
                // 12:00:00
                hora = ntpClient.getHours();
                minuto = ntpClient.getMinutes();
                segundo = ntpClient.getSeconds();
            }
        }
        else
        {
            /* RTC */
            dia = rtc.getDay();
            mes = rtc.getMonth() + 1;
            anio = rtc.getYear();
            hora = rtc.getHour(true);
            minuto = rtc.getMinute();
            segundo = rtc.getSecond();
        }
    }
    // sprintf( fecha, "%.2d-%.2d-%.4d %.2d:%.2d:%.2d", dia, mes, anio, hora, minuto, segundo);
    sprintf(fecha, "%.2d-%.2d-%.4d %.2d:%.2d", dia, mes, anio, hora, minuto);
    return String(fecha);
}

// horario de encendido y apagado del relevador
String releTime()
{

    char horas[5];
    int hora = 0;
    int minuto = 0;

    if (time_ajuste)
    { // Manual
        /* RTC */
        hora = rtc.getHour(true);
        minuto = rtc.getMinute();
    }
    else
    { // Automatico
        if ((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_AP_STA))
        {
            /* NTP */
            if (ntpClient.isTimeSet())
            {
                String formattedTime = ntpClient.getFormattedTime();
                // FORMAR FECHA DD-MM-YYYY DESDE EPOCH
                time_t epochTime = ntpClient.getEpochTime();
                struct tm *now = gmtime((time_t *)&epochTime);

                // 12:00:00
                hora = ntpClient.getHours();
                minuto = ntpClient.getMinutes();
            }
        }
        else
        {
            /* RTC */

            hora = rtc.getHour(true);
            minuto = rtc.getMinute();
        }
    }
    // sprintf( hora, "%.2d-%.2d-%.4d %.2d:%.2d:%.2d", dia, mes, anio, hora, minuto, segundo);
    sprintf(horas, "%.2d:%.2d", hora, minuto);
    return String(horas);
}

// -------------------------------------------------------------------
// contador de alarmas
// -------------------------------------------------------------------
void contadorAlarmas()
{
    int pines[5] = {ALRM_PIN1, ALRM_PIN2, ALRM_PIN3, ALRM_PIN4, ALRM_PIN5};
    bool logicas[5] = {ALRM_LOGIC1, ALRM_LOGIC2, ALRM_LOGIC3, ALRM_LOGIC4, ALRM_LOGIC5};
    String fechaAct[5] = {ALRM_TON1, ALRM_TON2, ALRM_TON3, ALRM_TON4, ALRM_TON5};
    String fechaClear[5] = {ALRM_TOFF1, ALRM_TOFF2, ALRM_TOFF3, ALRM_TOFF4, ALRM_TOFF5};
    for (int i = 0; i < 5; i++)
    {
        if (!logicas[i])
        { // si la logica es normal

            if (!digitalRead(pines[i]) && !cambiar[i])
            {
                cont[i]++;
                fechaAct[i] = getDateTime();

                fechaClear[i] = "";
                cambiar[i] = true;
            }
            else if (digitalRead(pines[i]) && cambiar[i])
            {
                cambiar[i] = false;
                fechaClear[i] = getDateTime();
            }
        }
        else
        { // si la logica es invertida

            if (digitalRead(pines[i]) && !cambiar[i])
            {
                cont[i]++;
                fechaAct[i] = getDateTime();
                fechaClear[i] = "";
                cambiar[i] = true;
            }
            else if (!digitalRead(pines[i]) && cambiar[i])
            {
                cambiar[i] = false;
                fechaClear[i] = getDateTime();
            }
        }
    }
    //}
    ALRM_CONT1 = cont[0];
    ALRM_CONT2 = cont[1];
    ALRM_CONT3 = cont[2];
    ALRM_CONT4 = cont[3];
    ALRM_CONT5 = cont[4];
    ALRM_TON1 = fechaAct[0];
    ALRM_TON2 = fechaAct[1];
    ALRM_TON3 = fechaAct[2];
    ALRM_TON4 = fechaAct[3];
    ALRM_TON5 = fechaAct[4];
    ALRM_TOFF1 = fechaClear[0];
    ALRM_TOFF2 = fechaClear[1];
    ALRM_TOFF3 = fechaClear[2];
    ALRM_TOFF4 = fechaClear[3];
    ALRM_TOFF5 = fechaClear[4];
}
// --------------------------------------------------------------
// Temperaturas y humedad
//  objeto DHT
DHT dht(DHTPIN, DHT22);
float Temperatura()
{
    tempC = dht.readTemperature();
    return tempC;
}
float Humedad()
{
    if (isnan(dht.readHumidity()))
    {
        humedad = 0;
    }
    else
    {
        humedad = dht.readHumidity();
    }
    return humedad;
}
float tempMin()
{

    float min = Temperatura();
    if (min <= 1)
    {
        min2 == min2;
    }
    else if (min < min2)
    {
        min2 = min;
    }
    // if (min < min2)
    // {
    //     min2 = min;
    // }
    // else if (min == 0)
    // {
    //     min2 = Temperatura();
    // }
    // Serial.println(min2);
    return min2;
}
float tempMax()
{

    float max = Temperatura();
    if (max > max2)
    {
        max2 = max;
    }
    // Serial.println(max2);
    return max2;
}
//---------------------------------------------------------------
// OLED
//---------------------------------------------------------------

void mostrar()
{

    if (wifi_mode == WIFI_AP)
    {
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0, 0);
        OLED.println(String(ap_ssid));
        OLED.println(ipStr(WiFi.softAPIP()));
        OLED.print(tempC); // no llamar a funciones
        OLED.print(" C ");
        OLED.print(humedad); // llamar a variables
        OLED.println("%");
        OLED.display();
        log("INFO", "functions.hpp", "Mostrando información en pantalla LCD (AP)");
    }
    else if (wifi_mode == WIFI_AP_STA)
    {
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0, 0);
        OLED.println(wifi_ssid);
        OLED.println(ipStr(WiFi.localIP()));
        OLED.print(Temperatura());
        OLED.print(" C ");
        OLED.print(Humedad());
        OLED.println("%");
        OLED.print(getDateTime());
        OLED.display();
        log("INFO", "functions.hpp", "Mostrando información en pantalla LCD (ESTACION)");
    }
    else
    {
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0, 0);
        OLED.println(ipStr(WiFi.localIP()));
        if (!digitalRead(26))
        {
            OLED.print(ALRM_NAME4);
            OLED.println(" OPERANDO");
        }
        else if (!digitalRead(27))
        {
            OLED.print(ALRM_NAME5);
            OLED.println(" OPERANDO");
        }
        else if (!digitalRead(27) && !digitalRead(27))
        {
            OLED.println("2 CLIMAS OPERANDO");
        }
        else
        {
            OLED.println("CLIMAS NO OPERANDO");
        }
        OLED.println(getDateTime());
        if (tempC < 2)
        {
            OLED.print("WAIT");
        }
        else
        {
            OLED.print(tempC);
        }
        OLED.print(" C   ");
        if (humedad < 2)
        {
            OLED.print("WAIT");
        }
        else
        {
            OLED.print(humedad);
        }
        OLED.println(" %");
        OLED.display();
    }
}

//----------------------------------------------------
// actulizacion de la variable time_now
//---------------------------------------------------
void actualizaTime()
{
    time_now = getDateTime();
    hora = releTime();
}

//--------------------------------------------------------------------------------
// Función para el Buzzer dispositivo Global -> API, MQTT, WS
// ejemplo: {"protocol":"API", "output": "buzzer", "value": false}
//------------------------------------------------------------------------------
void buzzer(String buzzer)
{
    DynamicJsonDocument JsonBuzzer(120);
    deserializeJson(JsonBuzzer, buzzer);
    log("INFO", "functions.hpp", "Comando enviado desde: " + JsonBuzzer["protocol"].as<String>() + " <=> " + JsonBuzzer["output"].as<String>() + " <=> " + JsonBuzzer["value"].as<String>());
    BUZZER_STATUS = JsonBuzzer["value"].as<bool>();
}
//------------------------------INTERRUPCIONES-------------------------------------------------
// Funcion para activar y desactivar los relevadores de manera local
//-------------------------------------------------------------------------------
void actRele()
{ /*  //jala solo para el d35 pero no para d32 ya que esta conectado diferente
  if (!digitalRead(ACTRELE1) && togle1)
  {
      togle1 = !togle1;
      if (!digitalRead(RELAY1))
      {
          digitalWrite(RELAY1, true);
          R_STATUS1 = true;
          tiempoDeInterrupcion = millis();
      }
      else if (digitalRead(RELAY1))
      {
          digitalWrite(RELAY1, false);
          R_STATUS1 = false;
          tiempoDeInterrupcion = millis();
      }
  }
  else
  {
      if (millis() - tiempoDeInterrupcion > tiempoDeRebote)
      {
          togle1 = !togle1;
          tiempoDeInterrupcion = millis();
      }
  }*/
    //-----------------------------------------------------------
    if (!digitalRead(RELAY1) && !digitalRead(ACTRELE1) && togle1)
    {
        togle1 = !togle1;
        digitalWrite(RELAY1, true);
        R_STATUS1 = true;
        tiempoDeInterrupcion = millis();
    }
    else if (digitalRead(RELAY1) && !digitalRead(ACTRELE1) && togle1)
    {
        togle1 = !togle1;
        digitalWrite(RELAY1, false);
        R_STATUS1 = false;
        tiempoDeInterrupcion = millis();
    }
    else
    {
        if (millis() - tiempoDeInterrupcion > tiempoDeRebote)
        {
            togle1 = true;
        }
    }
    //-----------------------------------------------------------
    if (!digitalRead(RELAY2) && !digitalRead(ACTRELE2) && togle2)
    {
        togle2 = !togle2;
        digitalWrite(RELAY2, true);
        R_STATUS2 = true;
        tiempoDeInterrupcion1 = millis();
    }
    else if (digitalRead(RELAY2) && !digitalRead(ACTRELE2) && togle2)
    {
        togle2 = !togle2;
        digitalWrite(RELAY2, false);
        R_STATUS2 = false;
        tiempoDeInterrupcion1 = millis();
    }
    else
    {
        if (millis() - tiempoDeInterrupcion1 > tiempoDeRebote)
        {
            togle2 = true;
        }
    }
}
/*
void IRAM_ATTR funcDeInterrupcion1()
{ // esta funcion de interrupcion no se manda a llamar se ejecuta sola cuando sucede la interrupcion
    if (millis() - tiempoDeInterrupcion > tiempoDeRebote)
    {
        Serial.println("cambiando el relevador1");
        R_STATUS1 = !R_STATUS1;

        tiempoDeInterrupcion = millis();
    }
}
void IRAM_ATTR funcDeInterrupcion2()
{ // esta funcion de interrupcion no se manda a llamar se ejecuta sola cuando sucede la interrupcion
    if (millis() - tiempoDeInterrupcion > tiempoDeRebote)
    {
        Serial.println("cambiando el relevador2");
        R_STATUS2 = !R_STATUS2;
        tiempoDeInterrupcion = millis();
    }
}*/
//-------------------------------------------------------------------------------
// Funcion para activar y desactivar los relevadores con respecto al tiempo
//-------------------------------------------------------------------------------
void temporelevares()
{
    if (TEMPORIZADOR1)
    {
        if (TIMEONRELAY1 == releTime())
        {
            digitalWrite(RELAY1, true);
            R_STATUS1 = true;
        }
        if (TIMEOFFRELAY1 == releTime())
        {
            digitalWrite(RELAY1, false);
            R_STATUS1 = false;
        }
    }
    if (TEMPORIZADOR2)
    {
        if (TIMEONRELAY2 == releTime())
        {
            digitalWrite(RELAY2, true);
            R_STATUS2 = true;
        }
        if (TIMEOFFRELAY2 == releTime())
        {
            digitalWrite(RELAY2, false);
            R_STATUS2 = false;
        }
    }
}
// Empaqueta el JSON para enviar por WS (progress en %)
String getSendJson(String msg, String type)
{
    String response = "";
    DynamicJsonDocument doc(64);
    doc["type"] = type;
    doc["msg"] = msg;
    serializeJson(doc, response);
    return response;
}

// funciopn printProgress
void printProgress(size_t prog, size_t sz)
{
    int progress = (prog * 100) / content_len;
    if (progress == 10)
    {
        wsMessageSend(getSendJson(String(10), "update"), "", "");
    }
}