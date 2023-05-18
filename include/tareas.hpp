//archivo de tareas (task) 

//---------------------------------------------------------------
// Tarea Loop WiFi & Reconectar
//---------------------------------------------------------------
void TaskWifiReconnect(void *pvParamenters){
    //para mantener conectado ya sea en modo ap o ap_sta al wifi
    (void) pvParamenters;
    while(1){//loop infinito
        vTaskDelay(10/portTICK_PERIOD_MS);
        if(wifi_app == WIFI_AP_STA){
            wifiLoop();
        }else if(wifi_app == WIFI_AP){
            wifiAPLoop();
        }else{

        }
    }
}
//---------------------------------------------------------------
// Tarea Loop MQTT & Reconectar
//---------------------------------------------------------------
void TaskMqttReconnect(void *pvParamenters){
    //para mantener conectado ya sea en modo ap o ap_sta al wifi
    (void) pvParamenters;
    while(1){//loop infinito
         if((WiFi.status() == WL_CONNECTED)&&(wifi_app == WIFI_AP_STA)){
            if(mqtt_server != 0){ //ya estaconectado
                //llamar a la funcion del loop mqtt
                mqttloop();
                //Enviar por MQTT el json siempre y cuando este habilitado el envio
                if(mqttClient.connected()&&mqtt_status_send){
                    if(millis()-lasMsg > mqtt_time_interval){
                        lasMsg = millis();
                        mqtt_publish();
                        log("INFO","tareas.hpp","Mensaje enviado por MQTT...");
                    }
                }
            }
         }
    }
}