#include "blinkleds.hpp"
// archivo de funciones que se ocuparán en el programa
/**
 * void log Genera mensajes personalizados en el puerto Serial
*/
void log(String type, String archivo,String msg){ //[INFO][ARCHIVO] msg
    Serial.println("["+ type +"]"+"["+ archivo +"] "+msg);
}
//-------------------------------------------------------------
//Definir la plataforma
//Optiene la Plataforma de hardware
//-------------------------------------------------------------
String platform(){
    #ifdef ARDUINO_ESP32_DEV
        return "ESP32";
    #endif
}
//----------------------------------------------------------------
//funcion de HEX a String osea de hexadecimal a String
//----------------------------------------------------------------
String HexToStr(const unsigned long &h, const byte &l){ //vairable de tipo byte - l es un puntero
    String s;
    s= String(h, HEX);
    s.toUpperCase();
    s = ("00000000" + s).substring(s.length() + 8 - l);
    //Serial.println("s: "+ s);
    return s;
}
//-----------------------------------------------------------------------
// Crea un ID unico desde la direccion MAC
// Retorna los ultimos 4 Bytes del MAC pero rotados
//----------------------------------------------------------------------
String UniqueID(){
    char uniqueid[15];
    uint64_t chipid = ESP.getEfuseMac();
    uint16_t chip = (uint16_t)(chipid >> 32);
    snprintf(uniqueid, 15, "%04X", chip); //formato de entrega
    //log("INFO","functions.hpp","uniqueid: "+ String(uniqueid));
    return uniqueid;
}

String DeviceID(){
    String d = String(platform())+ HexToStr(ESP.getEfuseMac(),8) + String(UniqueID());
    //log("INFO","functions.hpp","DeviceID: "+ d); //muestra en consola
    return String(platform())+ HexToStr(ESP.getEfuseMac(),8) + String(UniqueID());
}
//------------------------------------------------------------
// para el mDNS es la siguiente funcion extractNumbers 
// quita los numeros de una cadena
//------------------------------------------------------------
String extractNumbers(String e){
    String inputString = e;
    inputString.toLowerCase(); //todo a minuscula
    String outString = ""; //variable que estrega esta funcion
    int len = inputString.length();
    for (size_t i = 0; i < len; i++){
        if(!isDigit((char)inputString[i])){
            outString += (char)inputString[i]; //cuando sea un caracter lo agrega a la salida
        }
    }
    return outString;
}

//-----------------------------------------------------------------
//el path para el mqtt (el topic) ejemplo:  emqx/ESP32XXXXXXXX/status = true si esta conectado y false desconectado
//-----------------------------------------------------------------
String PathMqttTopic(String topic){
    return String(String(mqtt_user)+"/"+String(mqtt_id)+"/"+topic);
}

//Definicion de los pines GPIO
/**
 * #define RELAY1 33     //GPIO33 para el relay pero ya no se va a utilizar ya que lo modificare por comandos MQTT
#define RELAY2 15     //GPIO15 para el relay pero ya no se va a utilizar ya que lo modificare por comandos MQTT
#define DIMMER 25     //GPIO25 se modificara por comando por eso no se define
#define WIFILED 12      //GPIO12 LED INDICADOR WIFI
#define MQTTLED 13      //GPIO13 LED INDICADOR MQTT
#define BUZZER  32      //GPIO32 ZUMBADOR
*/
void gpioDefine(){
    //PINES
    pinMode(WIFILED, OUTPUT);
    pinMode(MQTTLED, OUTPUT);
    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    // PWM (DIMMER) dimerizar el led o foco o motor
    ledcSetup(ledChanel, freq, resolution);
    ledcAttachPin(DIMMER, ledChanel);
    //BUZZER
    pinMode(BUZZER, OUTPUT);
    // poner todos lo configurado en 0 o apagado
    setOffSingle(WIFILED);
    setOffSingle(MQTTLED);
    setOffSingle(RELAY1);
    setOffSingle(RELAY2);
    setOffSingle(BUZZER);
    ledcWrite(ledChanel, 0); //puede ir de 0 a 255 ya que la resolucion es de 8 bits
}
// -------------------------------------------------------------------
// Convierte un char a IP
// -------------------------------------------------------------------
uint8_t ip[4];    // Variable función convertir string a IP 
IPAddress CharToIP(const char *str){ //convierte de char a IP
    sscanf(str, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
}
// -------------------------------------------------------------------
// Retorna IPAddress en formato "n.n.n.n" osea de IP a String
// -------------------------------------------------------------------
String ipStr(const IPAddress &ip){    
    String sFn = "";
    for (byte bFn = 0; bFn < 3; bFn++){
        sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
    }
    sFn += String(((ip >> 8 * 3)) & 0xFF);
    return sFn;
}

//----------------------------------------------------------------------------
// OnOffRelays(command)     mensajes desde el broker
//Funcion para operar los Relays de forma Global -> API, MQTT, WS ejemplo:
//ejemplo tópico: lalo/ESP3263A7DBCC2CC1/command
// {"protocol":"MQTT", "output": "RELAY1", "value":true }
//----------------------------------------------------------------------------
bool OnOffRelays(String command){
    DynamicJsonDocument JsonCommand(320);
    deserializeJson (JsonCommand, command);
    log("INFO","functions.hpp","Comando enviado desde: "+JsonCommand["protocol"].as<String>()+" <=> "+JsonCommand["output"].as<String>()+ " <=> "+JsonCommand["value"].as<String>());

    if(JsonCommand["value"] == true ){
        if (JsonCommand["output"] == R_NAME1){
            if (R_LOGIC1){
                digitalWrite(RELAY1, HIGH);
                R_STATUS1 = true; 
            }else{
                digitalWrite(RELAY1, LOW);
                R_STATUS1 = false; 
            }
            return true;
        }else if(JsonCommand["output"] == R_NAME2){
            if (R_LOGIC2){
                digitalWrite(RELAY2, HIGH);
                R_STATUS2 = true; 
            }else{
                digitalWrite(RELAY2, LOW);
                R_STATUS2 = false;
            }
            return true;
        }else if(JsonCommand["output"] == "BUZZER"){
            digitalWrite(BUZZER, HIGH);
            BUZZER_STATUS = true;
            return true;
        }else{
            log("ERROR","functions.hpp","Actuador No Reconocido " + JsonCommand["output"].as<String>());
            return false;
        }
    }else if(JsonCommand["value"] == false ){
        if (JsonCommand["output"] == R_NAME1){
            if(R_LOGIC1){
                digitalWrite(RELAY1, LOW);
                R_STATUS1 = false; 
            }else{
                digitalWrite(RELAY1, HIGH);
                R_STATUS1 = true; 
            }
            return true;
        }else if(JsonCommand["output"] == R_NAME2){
            if (R_LOGIC2){
                digitalWrite(RELAY2, LOW);
                R_STATUS2 = false;
            }else{
                digitalWrite(RELAY2, HIGH);
                R_STATUS2 = true;
            }
            return true;
        }else if(JsonCommand["output"] == "BUZZER"){
            digitalWrite(BUZZER, LOW);
            BUZZER_STATUS = false;
            return true;
        }else{
            log("ERROR","functions.hpp","Actuador No Reconocido" + JsonCommand["output"].as<String>());
            return false;
        }
    }else{
        log("WARNING","functions.hpp","Comando NO reconocido" + JsonCommand["value"].as<String>());
        return false;
    }
}
//----------------------------------------------------------------------------
// mensajes desde el broker
//funcion que modifica los valores de las variables del dispositivo
//ejemplo topico: lalo/ESP3263A7DBCC2CC1/especial
// {"protocol":"MQTT", "varible": "RELAY1", "value":"CERRADURA" }
// {"protocol":"MQTT", "varible": "RELAY2", "value":"PUERTA"}
//----------------------------------------------------------------------------
bool especial(String command){
    DynamicJsonDocument JsonCommand(320);
    deserializeJson (JsonCommand, command);
    log("INFO","functions.hpp","Comando enviado desde: "+JsonCommand["protocol"].as<String>()+" <=> "+JsonCommand["varible"].as<String>()+ " <=> "+JsonCommand["value"].as<String>());

    if (JsonCommand["varible"] == R_NAME1){
        strlcpy(R_NAME1, JsonCommand["value"], sizeof(R_NAME1));
        return true;
    }
    if (JsonCommand["varible"] == R_NAME2){
        strlcpy(R_NAME2, JsonCommand["value"], sizeof(R_NAME2));
        return true;
    }
    if (JsonCommand["varible"] == R_LOGIC1){
        R_LOGIC1 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == R_LOGIC2){
        R_LOGIC2 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == R_DESCRIPTION1){
        R_DESCRIPTION1 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == R_DESCRIPTION2){
        R_DESCRIPTION2 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC1){
        ALRM_LOGIC1 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC2){
        ALRM_LOGIC2 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC3){
        ALRM_LOGIC3 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC4){
        ALRM_LOGIC4 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC5){
        ALRM_LOGIC5 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC6){
        ALRM_LOGIC6 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_LOGIC7){
        ALRM_LOGIC7 = JsonCommand["value"].as<bool>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME1){
        ALRM_NAME1 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME2){
        ALRM_NAME2 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME3){
        ALRM_NAME3 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME4){
        ALRM_NAME4 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME5){
        ALRM_NAME5 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME6){
        ALRM_NAME6 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == ALRM_NAME7){
        ALRM_NAME7 = JsonCommand["value"].as<String>();
        return true;
    }
    if (JsonCommand["varible"] == "CONTRASEÑA"){
        // {"protocol":"MQTT", "varible": "REINICIAR", "value":"211179" }
        if(JsonCommand["value"] == CONTRASENA_REINICIAR){
            REINICIAR=true;     //falta aplicar
        }else if(JsonCommand["value"] == CONTRASENA_DE_FABRICA){
            // {"protocol":"MQTT", "varible": "DEFABRICA", "value":"21111979" }
            DEFABRICA=true;     //falta aplicar
        }else{
            log("ERROR","functions.hpp","Contraseña Incorrecta");
            return false;
        }
        return true;
    }
    else{                                                 
        log("ERROR","functions.hpp","variable no reconocida " + JsonCommand["varible"].as<String>());
        return false;
    }
}
void muestraInfoMqtt(String command){
    DynamicJsonDocument JsonCommand(capacitySettings);
    deserializeJson (JsonCommand, command);
    log("INFO","functions","pendiente de mostrar informacion");
}
// -------------------------------------------------------------------
// Retorna segundos como "d:hh:mm"
// -------------------------------------------------------------------
String longTimeStr(const time_t &t){ 
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
int getRSSIasQuality(int RSSI){
    int quality = 0;
    if(RSSI <= -100){
        quality = 0;
    }else if (RSSI >= -50){
        quality = 100;
    }else{
       quality = 2 * (RSSI + 100);
    }
    return quality;
}
// -------------------------------------------------------
// Sensor de temp interno CPU
//--------------------------------------------------------
#ifdef __cplusplus
extern "C"{
#endif
    uint8_t temprature_sens_read();
#ifdef __cplusplus
}
#endif
uint8_t temprature_sens_read();
//---------------------------------------------------------
// Retorna la temperatura de CPU
//---------------------------------------------------------
float TempCPUValue(){
    return temp_cpu = (temprature_sens_read()-32)/1.8;
}

//**************************************************************
/** Leer la temperatura del sensor
 * void readSensor(){
 *  sensors.requestTemperatures();
 *  temperatureC= sensors.getTempCByIndex(0);
 *  temperatureF= sensors.getTempFByIndex(0);
 * }
*/
//****************************************************************
// -------------------------------------------------------------------
// Fecha y Hora del Sistema
// -------------------------------------------------------------------

String getDateTime(){
    
    char fecha[20];
    int dia = 0;
    int mes = 0;
    int anio = 0;
    int hora = 0;
    int minuto = 0;
    int segundo = 0;

    if(time_ajuste){ // Manual
        /* RTC */
        dia = rtc.getDay();
        mes = rtc.getMonth()+1;
        anio = rtc.getYear();
        hora = rtc.getHour(true);
        minuto = rtc.getMinute();
        segundo = rtc.getSecond();
    }else{ // Automatico
        if((WiFi.status() == WL_CONNECTED) && (wifi_mode == WIFI_STA)){
            /* NTP */
            if(ntpClient.isTimeSet()) {
                String formattedTime = ntpClient.getFormattedTime();
                // FORMAR FECHA DD-MM-YYYY DESDE EPOCH
                time_t epochTime = ntpClient.getEpochTime();
                struct tm *now = gmtime ((time_t *)&epochTime); 
                anio = now->tm_year+1900;
                mes =  now->tm_mon+1;
                dia =  now->tm_mday;
                // 12:00:00
                hora = ntpClient.getHours();
                minuto = ntpClient.getMinutes();
                segundo = ntpClient.getSeconds();
            }  
        }else{
            /* RTC */
            dia = rtc.getDay();
            mes = rtc.getMonth()+1;
            anio = rtc.getYear();
            hora = rtc.getHour(true);
            minuto = rtc.getMinute();
            segundo = rtc.getSecond();
        }                   
    }	
    //sprintf( fecha, "%.2d-%.2d-%.4d %.2d:%.2d:%.2d", dia, mes, anio, hora, minuto, segundo);
    sprintf( fecha, "%.2d-%.2d-%.4d %.2d:%.2d", dia, mes, anio, hora, minuto);
	return String( fecha );
}

// -------------------------------------------------------------------
//contador de alarmas
void contadorAlarmas(){
    int pines[7] = {ALRM_PIN1,ALRM_PIN2,ALRM_PIN3,ALRM_PIN4,ALRM_PIN5,ALRM_PIN6,ALRM_PIN7};
    bool logicas[7] = {ALRM_LOGIC1,ALRM_LOGIC2,ALRM_LOGIC3,ALRM_LOGIC4,ALRM_LOGIC5,ALRM_LOGIC6,ALRM_LOGIC7};
    String fechaAct[7] = {ALRM_TON1,ALRM_TON2,ALRM_TON3,ALRM_TON4,ALRM_TON5,ALRM_TON6,ALRM_TON7};
    String fechaClear[7] = {ALRM_TOFF1,ALRM_TOFF2,ALRM_TOFF3,ALRM_TOFF4,ALRM_TOFF5,ALRM_TOFF6,ALRM_TOFF7};
        for (int i=0; i < 7; i++){
            if(!logicas[i]){ //si la logica es normal
        
                if(!digitalRead(pines[i]) && !cambiar[i]){
                    cont[i]++;
                    fechaAct[i] = getDateTime();
                   
                    fechaClear[i] = "";
                    cambiar[i]=true;
                }else if(digitalRead(pines[i]) && cambiar[i]){
                    cambiar[i]=false;
                    fechaClear[i] = getDateTime();
                }
            }else{  //si la logica es invertida
            
                if(digitalRead(pines[i]) && !cambiar[i]){
                    cont[i]++;
                    fechaAct[i] = getDateTime();
                    fechaClear[i] = "";
                    cambiar[i]=true;
                }else if(!digitalRead(pines[i]) && cambiar[i]){
                    cambiar[i]=false;
                    fechaClear[i] = getDateTime();
                }
            }
        }
    //}
    ALRM_CONT1=cont[0];ALRM_CONT2=cont[1];
    ALRM_CONT3=cont[2];ALRM_CONT4=cont[3];
    ALRM_CONT5=cont[4];ALRM_CONT6=cont[5];
    ALRM_CONT7=cont[6];
    ALRM_TON1=fechaAct[0];ALRM_TON2=fechaAct[1];
    ALRM_TON3=fechaAct[2];ALRM_TON4=fechaAct[3];
    ALRM_TON5=fechaAct[4];ALRM_TON6=fechaAct[5];
    ALRM_TON7=fechaAct[6];
    ALRM_TOFF1=fechaClear[0];ALRM_TOFF2=fechaClear[1];
    ALRM_TOFF3=fechaClear[2];ALRM_TOFF4=fechaClear[3];
    ALRM_TOFF5=fechaClear[4];ALRM_TOFF6=fechaClear[5];
    ALRM_TOFF7=fechaClear[6];
    
}
// --------------------------------------------------------------
// Temperaturas y humedad
//  objeto DHT
DHT dht(DHTPIN, DHT22); 
float Temperatura(){
    return tempC = dht.readTemperature();
}
float Humedad(){
    return humedad = dht.readHumidity();
}
float tempMin(){
  
  float min = Temperatura();
  if(min < min2){
    min2 = min;
  }else if(min == 0){
    min2 = Temperatura();
  }
  //Serial.println(min2);
  return min2;
}  
float tempMax(){
  
  float max = Temperatura();
  if(max > max2){
    max2 = max;
  }
  //Serial.println(max2);
  return max2;
}
//---------------------------------------------------------------
// OLED
//---------------------------------------------------------------
/*
void mostrar(){
    if(wifi_mode == WIFI_AP){
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0,0);
        OLED.println(String(ap_ssid));
        OLED.println(ipStr(WiFi.softAPIP()));
        OLED.print(Temperatura());
        OLED.print(" C ");
        OLED.print(Humedad());
        OLED.println("%");
        OLED.display();
    }else if(wifi_mode == WIFI_AP_STA){
        OLED.clearDisplay();
        OLED.setTextSize(1);
        OLED.setTextColor(WHITE);
        OLED.setCursor(0,0);
        OLED.println(wifi_ssid);
        OLED.println(ipStr(WiFi.localIP()));
        OLED.print(Temperatura());
        OLED.print(" C ");
        OLED.print(Humedad());
        OLED.println("%");
        OLED.print(getDateTime());
        OLED.display();
    }
}

//funciones del protocolo ESP-NOW
typedef struct struct_message {
    String modoSend; //modo recibido
    int id;         //identificador de esp que envia
    String nameNodo; //nombre del nodo
    float temp;     //Temperatura de la sala
    float hum;      //humedad de la sala
    int readingId;  //numero de envio ya no ahora es señal RSSI
    float min;
    float max;
    //------------------------------------------------------------
    String alr1,alr2,alr3,alr4,alr5,alr6,alr7,alr8;
    bool  va1,va2,va3,va4,va5,va6,va7,va8;
    //------------------------------------------------------------
} struct_message;

struct_message incomingReadings; //es una instancia de la estructura del mensaje que se recibe

StaticJsonDocument<500> board; //verificar con mas capacidad

//AsyncWebServer server(80);
//AsyncEventSource events("/events"); //en el html espnow.html requiere la libreria ESPAsyncWebServer

// callback funcion que se ejecuta cuando un dato es recivido
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
    String jsonString;
    // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("[INFO: funciones.hpp] Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  //todos estos valores los envia el transmisor
  board["modoSend"] = String(incomingReadings.modoSend);
  board["id"] = incomingReadings.id;
  board["nameNodo"] = String(incomingReadings.nameNodo);
  board["temperature"] = incomingReadings.temp;
  board["humidity"] = incomingReadings.hum;
  board["readingId"] = incomingReadings.readingId; //aqui esta llegando la señal wiFi
  board["min"] = incomingReadings.min; //temperatura min
  board["max"] = incomingReadings.max; //temperatura maxima
  //--------------------------------------------------------------
  board["alr1"] = incomingReadings.alr1; 
  board["alr2"] = incomingReadings.alr2;
  board["alr3"] = incomingReadings.alr3;
  board["alr4"] = incomingReadings.alr4;
  board["alr5"] = incomingReadings.alr5;
  board["alr6"] = incomingReadings.alr6;
  board["alr7"] = incomingReadings.alr7;
  board["alr8"] = incomingReadings.alr8;
  board["va1"] = incomingReadings.va1;
  board["va2"] = incomingReadings.va2;
  board["va3"] = incomingReadings.va3;
  board["va4"] = incomingReadings.va4;
  board["va5"] = incomingReadings.va5;
  board["va6"] = incomingReadings.va6;
  board["va7"] = incomingReadings.va7;
  board["va8"] = incomingReadings.va8;
  //--------------------------------------------------------------
  board["tmp"] = 0;
  // la siguiente linea es para int (enteros)
  //board["readingId"] = String(incomingReadings.readingId);
  //hay que serealizar para convertir el objeto en cadena
  Serial.println(incomingReadings.nameNodo);
  Serial.println(incomingReadings.modoSend);
  serializeJson(board,jsonString);
  //events.send(jsonString.c_str(), "new_readings", millis()); //parece que esto se envia a una página web html
  
  Serial.printf("Board ID %u: %u bytes\n", incomingReadings.id, len);
  Serial.println("Nombre del nodo: "+ incomingReadings.nameNodo);
  Serial.printf("t value: %4.2f \n", incomingReadings.temp);
  Serial.printf("h value: %4.2f \n", incomingReadings.hum);
  Serial.printf("readingID value: %4.2f \n", incomingReadings.readingId);
  Serial.printf("min value: %4.2f \n", incomingReadings.min);
  Serial.printf("max value: %4.2f \n", incomingReadings.max);
  // para imprimir enteros
  //Serial.printf("readingID value: %d \n", incomingReadings.readingId);
  Serial.println();
  
} */