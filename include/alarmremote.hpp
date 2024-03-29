#include <PubSubClient.h>
#include <WiFi.h> 
//#define MQTT_MAX_PACKET_SIZE 1024*5 //otra manera de aumentar el buffer pero da un warning
// inicia en video 49 y 50
WiFiClient wifiClient2; //este es el puntero a la conexión mqtt2
PubSubClient mqttClient2(wifiClient2); //se define el cliente mqtt2 sobre la conexión wifi

//Variables
char topic2[150]; 
String topic2_subscribe; //este se saca de una funcion que convierte el mqtt_extraTopic agregando "/"+topic2
String Json2();
void callback2(char *topic2, byte *payload, unsigned int length); //declaracion de una funcion que se va a usar
char topic2Expecial[150]; // se requiere un topic2 para recibir informacion de manera remota 
String topic2_subscribeEsp;
//variable que guarda la data para ser enviadad por MQTT
String mqtt_data2 = "";

//los tiempos para intentar la reconexión a MQTT
long lastMqttReconnectAttempt2 = 0;

//tiempo para enviar el mensaje (publicar)
long lasMsg2 = 0;

//Declaracion de funciones que se usaran en este archivo
String PathMqttTopic2(String topic2);
//----------------------------------------------------------------------
// MQTT Connectar a otro broker o al mismo
// ---------------------------------------------------------------------
bool mqtt_connect2(){
    mqttClient2.setServer(mqtt_server2, mqtt_port2);
    mqttClient2.setCallback(callback2);
    log("INFO","alarmremote.hpp","Intentando conexión al Broker MQTT ...");
    strlcpy(mqtt_willTopic2, PathMqttTopic2("status").c_str(),sizeof(mqtt_willTopic2));
    String topic2_publish = PathMqttTopic2("status"); //publicara
    topic2_publish.toCharArray(mqtt_willTopic2, 150); //will significa mensaje que ultima voluntad

    // https://pubsubclient.knolleary.net/api.html
    //boolean connect (clientID, [username, password], [willTopic2, willQoS, willRetain, willMessage], [cleanSession])
    if(mqttClient2.connect(mqtt_id2, mqtt_user2, mqtt_password2, mqtt_willTopic2, mqtt_willQoS2, mqtt_willRetain2, mqtt_willMessage2, mqtt_clean_sessions2)){
        log("INFO","alarmremote.hpp","Conectado al Broker2 MQTT");

        mqttClient2.setBufferSize(1024*5); //ampliar el buffer de 256 a 5KB
        log("INFO","alarmremote.hpp","Buffer MQTT Size: " + String(mqttClient2.getBufferSize()) + " Bytes");
        //tambien se puede modificar en la libreria en el path
        //.pio/libdeps/esp32dev/PubSubClient/src/PubSubClient.h #define MQTT_MAX_PACKET_SIZE 256 => 1024*5
        topic2_subscribe = PathMqttTopic2("device"); //donde llegan los comando
        topic2_subscribe.toCharArray(topic2, 150);
        if(mqttClient2.subscribe(topic2, mqtt_qos2)){
            log("INFO","alarmremote.hpp","Suscrito: " +String(topic2));
        }else{
            log("ERROR","alarmremote.hpp","MQTT - Falló suscripción: " + String(topic2));
        }
        //se genera el path de especial y se mete a un String llamado topic2_subscribeEsp
        topic2_subscribeEsp = PathMqttTopic2("especial"); //donde se modifica los nombres de las alarmas entre otras cosas
        //la siguiente funcion lo convierte en un char Array 
        topic2_subscribeEsp.toCharArray(topic2Expecial, 150);
        if(mqttClient2.subscribe(topic2Expecial, mqtt_qos2)){
            log("INFO","alarmremote.hpp","Suscrito: " +String(topic2Expecial));
        }else{
            log("ERROR","alarmremote.hpp","MQTT - Falló suscripción: " + String(topic2Expecial));
        }

    
        mqttClient2.publish(mqtt_willTopic2, "{\"connected\": true}", mqtt_retain);
 
    }else{
        log("ERROR","alarmremote.hpp","MQTT - Falló, código de error = " + String(mqttClient2.state()));
        /**
         * -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
         * -3 : MQTT_CONNECTION_LOST - the network connection was broken
         * -2 : MQTT_CONNECT_FAILED - the network connection failed
         * -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
         * 0 : MQTT_CONNECTED - the client is connected
         * 1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
         * 2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
         * 3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
         * 4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
         * 5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
        */
        return (0); //esto es igual a false
    }
    return (1); //esto es igual a true
}

// -----------------------------------------------------------------------------------
// Manejo de los mensajes Entrantes
// -----------------------------------------------------------------------------------
// void callback2(char *topic2, byte *payload, unsigned int length){
//     String command = ""; //lo que venga en device que es donde estoy publicando
//     String str_topic2(topic2);
//     for(int16_t i = 0; i < length; i++){
//         command+= (char)payload[i];
//         //TODO: pestañeo de led MQTT
//     }
//     //metodo trim() que limpia los espacion
//     command.trim();
//     log("INFO","alarmremote.hpp","MQTT Tópico  --> " + str_topic2);
//     //log("INFO","alarmremote.hpp","MQTT Mensaje --> " + command);

//     log("INFO","alarmremote.hpp","Muestra información de otro dispositivo");
//     muestraInfoMqtt2(command); //y agrega los valores a las variables
    
//     //TODO: responder al mqtt el estado de los relays
// }

// -----------------------------------------------------------------------------------
// Manejo de los mensajes Salientes aqui solo seran los estados de los relays y buzzer ****************TODO: aun no se ha hehco 28/05/2023
// -----------------------------------------------------------------------------------
void mqtt_publish2(){
    String topic2 = PathMqttTopic2("device");
    log("INFO","alarmremote.hpp","publicando en: "+topic2);
    mqtt_data2 = Json2();
    //c_str() convierte un string en char
    mqttClient2.publish(topic2.c_str(), mqtt_data2.c_str(), mqtt_retain);
    mqtt_data2 = ""; //limpiamos los valores de mqtt
}
// ----------------------------------------------------------------------------------
// JSON2 con información para el dispositivo remoto envio por MQTT TODO: aun pendiente**************************************
// ----------------------------------------------------------------------------------
String Json2(){
    String response;
    DynamicJsonDocument json2Doc(3000);
    
    JsonObject dataObj = json2Doc.createNestedObject("data");

    dataObj["R_STATUS1"] = false;
	dataObj["R_STATUS2"] = false;
    dataObj["BUZZER_STATUS"] = false;
	
    serializeJson(json2Doc, response);
    return response;
}

//----------------------------------------------------------------------
// MQTT Loop Principal se encarga de mantener viva conexión MQTT
// ---------------------------------------------------------------------

void mqttloop2(){
    
    char untopic[5];
    strlcpy(untopic, "+/#",sizeof(untopic));
    if(String(mqtt_extraTopic) == mismotopic){
        if(mqtt_enable2){
            if(!mqttClient2.connected()){
                long now = millis();
                if(now < 60000 || ((now -lastMqttReconnectAttempt2)>120000)){
                    lastMqttReconnectAttempt2 = now;
                    if(mqtt_connect2()){
                        lastMqttReconnectAttempt2 = 0;
                        mqtt2_status = true;
                    }else{
                        mqtt2_status = false;
                    }
                }
            }else{
                mqttClient2.loop();//este metodo permita que siga escuchando
                //la informacion callback2 por ejemplo
                //escuchando las conexiones
            }
        }
    }else{
        log("INFO","alarmremote.hpp","Cambio de topic");
        mismotopic = String(mqtt_extraTopic); //hubo un cambio de topic por lo tanto hay que deslogearse y desconectarse
        //realizar acciones para desconectarse
        mqttClient2.disconnect();
        log("INFO","alarmremote.hpp","Desconectando...");
        //realizar acciones para desuncribirse
        mqttClient2.unsubscribe(untopic);
        log("INFO","alarmremote.hpp","Deshabilitando subscripciones anteriores...");
    }

        
}

//funciones necesarias
String PathMqttTopic2(String topic2){
    return String(String(mqtt_extraTopic)+"/"+topic2);
}

