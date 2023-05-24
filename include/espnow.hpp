/** aun falta por implementar tiene que ser despues del wifi
 * https://randomnerdtutorials.com/esp32-esp-now-wi-fi-web-server/
*/
//esta libreria pertenece al esp32 por lo tanto no hay que bajarla
void setupEspnow(){//en main.cpp     
  if (esp_now_init()) {
    log("ERROR","espnow.hpp","Iniciando protocolo ESP-NOW....");
    //while (true);
  }else{
    log("INFO","espnow.hpp","Protocolo ESP-NOW inicializado.");
    //necesito el canal para pasarselo al protocolo espnow
    wifi_chanel = WiFi.channel();
  }
  log("INFO","espnow.hpp","ESP Board MAC Address: " + String(WiFi.macAddress()));
  log("INFO","espnow.hpp","Canal Wifi: " + String(wifi_chanel));
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

// callback funcion que se ejecuta cuando un dato es recivido
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
    String jsonString="";
    // Copies the sender mac address to a string
    char macStr[18]; //direccion mac de donde recive la informacion
    log("INFO","espnow.hpp","Paquete de informacion recivido de: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
    //todos estos valores los envia el transmisor
    if(incomingReadings.id==1){
      board["modoSend1"] = String(incomingReadings.modoSend);
      board["id1"] = incomingReadings.id;
      board["nameNodo1"] = String(incomingReadings.nameNodo);
      board["temp1"] = incomingReadings.temp;
      board["hum1"] = incomingReadings.hum;
      board["dBmWiFi1"] = incomingReadings.readingId; //aqui esta llegando la señal wiFi
      board["min1"] = incomingReadings.min; //temperatura min
      board["max1"] = incomingReadings.max; //temperatura maxima
    //--------------------------------------------------------------
      if(cambio11==""){
        board["alr11"] = incomingReadings.alr1;
      }else{board["alr11"] =cambio11;} 
      if(cambio21==""){
        board["alr21"] = incomingReadings.alr2;
      }else{board["alr21"] =cambio21;}
      if(cambio31==""){
        board["alr31"] = incomingReadings.alr3;
      }else{board["alr31"] =cambio31;}
      if(cambio41==""){
        board["alr41"] = incomingReadings.alr4;
      }else{board["alr41"] =cambio41;}
      if(cambio51==""){
        board["alr51"] = incomingReadings.alr5;
      }else{board["alr51"] =cambio51;}
      if(cambio61==""){
        board["alr61"] = incomingReadings.alr6;
      }else{board["alr61"] =cambio61;}
      if(cambio71==""){
        board["alr71"] = incomingReadings.alr7;
      }else{board["alr71"] =cambio71;}
      if(cambio81==""){
        board["alr81"] = incomingReadings.alr8;
      }else{board["alr81"] =cambio81;}
      board["va11"] = incomingReadings.va1;
      board["va21"] = incomingReadings.va2;
      board["va31"] = incomingReadings.va3;
      board["va41"] = incomingReadings.va4;
      board["va51"] = incomingReadings.va5;
      board["va61"] = incomingReadings.va6;
      board["va71"] = incomingReadings.va7;
      board["va81"] = incomingReadings.va8;
    }
    if(incomingReadings.id==2){
      board["modoSend2"] = String(incomingReadings.modoSend);
      board["id2"] = incomingReadings.id;
      board["nameNodo2"] = String(incomingReadings.nameNodo);
      board["temp2"] = incomingReadings.temp;
      board["hum2"] = incomingReadings.hum;
      board["dBmWiFi2"] = incomingReadings.readingId; //aqui esta llegando la señal wiFi
      board["min2"] = incomingReadings.min; //temperatura min
      board["max2"] = incomingReadings.max; //temperatura maxima
    //--------------------------------------------------------------
      if(cambio12==""){
        board["alr12"] = incomingReadings.alr1;
      }else{board["alr12"] =cambio12;}
      if(cambio22==""){ 
        board["alr22"] = incomingReadings.alr2;
      }else{board["alr22"] =cambio22;}
      if(cambio32==""){
        board["alr32"] = incomingReadings.alr3;
      }else{board["alr32"] =cambio32;}
      if(cambio42==""){
        board["alr42"] = incomingReadings.alr4;
      }else{board["alr42"] =cambio42;}
      if(cambio52==""){
        board["alr52"] = incomingReadings.alr5;
      }else{board["alr52"] =cambio52;}
      if(cambio62==""){
        board["alr62"] = incomingReadings.alr6;
      }else{board["alr62"] =cambio62;}
      if(cambio72==""){
        board["alr72"] = incomingReadings.alr7;
      }else{board["alr72"] =cambio72;}
      if(cambio82==""){
        board["alr82"] = incomingReadings.alr8;
      }else{board["alr82"] =cambio82;}
      board["va12"] = incomingReadings.va1;
      board["va22"] = incomingReadings.va2;
      board["va32"] = incomingReadings.va3;
      board["va42"] = incomingReadings.va4;
      board["va52"] = incomingReadings.va5;
      board["va62"] = incomingReadings.va6;
      board["va72"] = incomingReadings.va7;
      board["va82"] = incomingReadings.va8;
    }
    if(incomingReadings.id==3){
      board["modoSend3"] = String(incomingReadings.modoSend);
      board["id3"] = incomingReadings.id;
      board["nameNodo3"] = String(incomingReadings.nameNodo);
      board["temp3"] = incomingReadings.temp;
      board["hum3"] = incomingReadings.hum;
      board["dBmWiFi3"] = incomingReadings.readingId; //aqui esta llegando la señal wiFi
      board["min3"] = incomingReadings.min; //temperatura min
      board["max3"] = incomingReadings.max; //temperatura maxima
    //--------------------------------------------------------------
      if(cambio13==""){
        board["alr13"] = incomingReadings.alr1;
      }else{board["alr13"] =cambio13;}
      if(cambio23==""){ 
        board["alr23"] = incomingReadings.alr2;
      }else{board["alr23"] =cambio23;}
      if(cambio33==""){
        board["alr33"] = incomingReadings.alr3;
      }else{board["alr33"] =cambio33;}
      if(cambio43==""){
        board["alr43"] = incomingReadings.alr4;
      }else{board["alr43"] =cambio43;}
      if(cambio53==""){
        board["alr53"] = incomingReadings.alr5;
      }else{board["alr53"] =cambio53;}
      if(cambio63==""){
        board["alr63"] = incomingReadings.alr6;
      }else{board["alr63"] =cambio63;}
      if(cambio73==""){
        board["alr73"] = incomingReadings.alr7;
      }else{board["alr73"] =cambio73;}
      if(cambio83==""){
        board["alr83"] = incomingReadings.alr8;
      }else{board["alr83"] =cambio83;}
      board["va13"] = incomingReadings.va1;
      board["va23"] = incomingReadings.va2;
      board["va33"] = incomingReadings.va3;
      board["va43"] = incomingReadings.va4;
      board["va53"] = incomingReadings.va5;
      board["va63"] = incomingReadings.va6;
      board["va73"] = incomingReadings.va7;
      board["va83"] = incomingReadings.va8;
    }
    if(incomingReadings.id==4){
      board["modoSend4"] = String(incomingReadings.modoSend);
      board["id4"] = incomingReadings.id;
      board["nameNodo4"] = String(incomingReadings.nameNodo);
      board["temp4"] = incomingReadings.temp;
      board["hum4"] = incomingReadings.hum;
      board["dBmWiFi4"] = incomingReadings.readingId; //aqui esta llegando la señal wiFi
      board["min4"] = incomingReadings.min; //temperatura min
      board["max4"] = incomingReadings.max; //temperatura maxima
    //--------------------------------------------------------------
      if(cambio14==""){
        board["alr14"] = incomingReadings.alr1;
      }else{board["alr14"] =cambio14;}
      if(cambio24==""){ 
        board["alr24"] = incomingReadings.alr2;
      }else{board["alr24"] =cambio24;}
      if(cambio34==""){
        board["alr34"] = incomingReadings.alr3;
      }else{board["alr34"] =cambio34;}
      if(cambio44==""){
        board["alr44"] = incomingReadings.alr4;
      }else{board["alr44"] =cambio44;}
      if(cambio54==""){
        board["alr54"] = incomingReadings.alr5;
      }else{board["alr54"] =cambio54;}
      if(cambio64==""){
        board["alr64"] = incomingReadings.alr6;
      }else{board["alr64"] =cambio64;}
      if(cambio74==""){
        board["alr74"] = incomingReadings.alr7;
      }else{board["alr74"] =cambio74;}
      if(cambio84==""){
        board["alr84"] = incomingReadings.alr8;
      }else{board["alr84"] =cambio84;}
      board["va14"] = incomingReadings.va1;
      board["va24"] = incomingReadings.va2;
      board["va34"] = incomingReadings.va3;
      board["va44"] = incomingReadings.va4;
      board["va54"] = incomingReadings.va5;
      board["va64"] = incomingReadings.va6;
      board["va74"] = incomingReadings.va7;
      board["va84"] = incomingReadings.va8;
    }
    //--------------------------------------------------------------
    //board["tmp"] = 0; //no recuerdo a de ser el tiempo
    // la siguiente linea es para int (enteros)
    //board["readingId"] = String(incomingReadings.readingId);
    //hay que serealizar para convertir el objeto en cadena
    log("INFO","espnow.hpp","Informacion recivida ");
    //Serial.println(incomingReadings.nameNodo);
    //log("INFO","espnow.hpp","Protocolo de envio: ");
    Serial.println(incomingReadings.modoSend); //muestra en consola el protocolo de recepción
    serializeJson(board,jsonString);
    //serializeJsonPretty(board,Serial);//muestra el resultado en el monitor
    jsonStringApi=jsonString; //lo metemos a la variable que se mostrara en la API
    //events.send(jsonString.c_str(), "new_readings", millis()); //parece que esto se envia a una página web html
    Serial.printf("Board ID %u: %u bytes\n", incomingReadings.id, len);
    // para imprimir enteros
    //Serial.printf("readingID value: %d \n", incomingReadings.readingId);
} 