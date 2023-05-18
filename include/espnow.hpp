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
  }
  log("INFO","espnow.hpp","ESP Board MAC Address: " + String(WiFi.macAddress()));
}