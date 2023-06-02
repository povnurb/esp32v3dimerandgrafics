#include <PubSubClient.h>
//#define MQTT_MAX_PACKET_SIZE 1024*5 //otra manera de aumentar el buffer pero da un warning
// inicia en video 49 y 50
WiFiClient wifiClient; //este es el puntero a la conexión mqtt
PubSubClient mqttClient(wifiClient); //se define el cliente mqtt sobre la conexión wifi

//Variables

char topic[150];
String topic_subscribe;
String Json();
void callback(char *topic, byte *payload, unsigned int length);
char topicExpecial[150]; // se requiere un topic para recibir informacaion de manera remota 
String topic_subscribeEsp;
//variable que guarda la data para ser enviadad por MQTT
String mqtt_data = "";

//los tiempos para intentar la reconexión a MQTT
long lastMqttReconnectAttempt = 0;

//tiempo para enviar el mensaje (publicar)
long lasMsg = 0;

//----------------------------------------------------------------------
// MQTT Connect
// ---------------------------------------------------------------------
bool mqtt_connect(){
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setCallback(callback);
    log("INFO","mqtt.hpp","Intentando conexión al Broker MQTT ...");

    String topic_publish = PathMqttTopic("status"); //publicara
    topic_publish.toCharArray(mqtt_willTopic, 150); //will significa mensaje que ultima voluntad

    // https://pubsubclient.knolleary.net/api.html
    //boolean connect (clientID, [username, password], [willTopic, willQoS, willRetain, willMessage], [cleanSession])
    if(mqttClient.connect(mqtt_id, mqtt_user, mqtt_password, mqtt_willTopic, mqtt_willQoS, mqtt_willRetain, mqtt_willMessage, mqtt_clean_sessions)){
        log("INFO","mqtt.hpp","Conectado al Broker MQTT");

        mqttClient.setBufferSize(1024*5); //ampliar el buffer de 256 a 5KB
        log("INFO", "mqtt.hpp","Buffer MQTT Size: " + String(mqttClient.getBufferSize()) + " Bytes");
        //tambien se puede modificar en la libreria en el path
        //.pio/libdeps/esp32dev/PubSubClient/src/PubSubClient.h #define MQTT_MAX_PACKET_SIZE 256 => 1024*5
        topic_subscribe = PathMqttTopic("command"); //donde llegan los comando
        topic_subscribe.toCharArray(topic, 150);
        if(mqttClient.subscribe(topic, mqtt_qos)){
            log("INFO","mqtt.hpp","Suscrito: " + String(topic));
        }else{
            log("ERROR","mqtt.hpp","MQTT - Falló suscripción: " + String(topic));
        }
        //se genera el path de especial y se mete a un String llamado topic_subscribeEsp
        topic_subscribeEsp = PathMqttTopic("especial"); //donde se modifica los nombres de las alarmas entre otras cosas
        //la siguiente funcion lo convierte en un char Array 
        topic_subscribeEsp.toCharArray(topicExpecial, 150);
        if(mqttClient.subscribe(topicExpecial, mqtt_qos)){
            log("INFO","mqtt.hpp","Suscrito: " +String(topicExpecial));
        }else{
            log("ERROR","mqtt.hpp","MQTT - Falló suscripción: " + String(topicExpecial));
        }

        //si esta permitido publicar
        if(mqtt_status_send){
            mqttClient.publish(mqtt_willTopic, "{\"connected\": true}", mqtt_retain);
        }
    }else{
        log("ERROR","mqtt.hpp","MQTT - Falló, código de error = " + String(mqttClient.state()));
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
void callback(char *topic, byte *payload, unsigned int length){
    String command = "";
    String str_topic(topic);
    for(int16_t i = 0; i < length; i++){
        command+= (char)payload[i];
        //TODO: pestañeo de led MQTT
    }
    //metodo trim() que limpia los espacion
    command.trim();
    log("INFO","mqtt.hpp","MQTT Tópico  --> " + str_topic);
    log("INFO","mqtt.hpp","MQTT Mensaje --> " + command);

    if(str_topic == topic_subscribe){
        log("INFO","mqtt.hpp","Comando para los Actuadores");
        OnOffRelays(command);
    }
    if(str_topic == topic_subscribeEsp){
        log("INFO","mqtt.hpp","Modificacion Especial de valores");
        especial(command);
    }
    else{
        log("INFO","mqtt.hpp","Muestra información de otro dispositivo");
        muestraInfoMqtt(command);
    }
    //TODO: responder al mqtt el estado de los relays
}

// -----------------------------------------------------------------------------------
// Manejo de los mensajes Salientes
// -----------------------------------------------------------------------------------
void mqtt_publish(){
    String topic = PathMqttTopic("device");
    log("INFO","mqtt.hpp","publicando en: "+topic);
    mqtt_data = Json();
    //c_str() convierte un string en char
    mqttClient.publish(topic.c_str(), mqtt_data.c_str(), mqtt_retain);
    mqtt_data = ""; //limpiamos los valores de mqtt
}
// ----------------------------------------------------------------------------------
// JSON con información del dispositivo para envio por MQTT
// ----------------------------------------------------------------------------------
String Json(){
    String response;
    DynamicJsonDocument jsonDoc(1024*4);
    // Capturar el valor de temperatura
    //readSensor(); //toma lecturas del sensor DS18B20
    // Crear JSON
    jsonDoc["serial"] = DeviceID();
    jsonDoc["MACWiFi"] = String(WiFi.macAddress());
    //jsonDoc["deviceManufacturer"] = String(device_manufacturer);
    //jsonDoc["deviceFwVersion"] = device_fw_version;
    //jsonDoc["deviceHwVersion"] = String(device_hw_version);
    //jsonDoc["deviceSdk"] = String(ESP.getSdkVersion());

    JsonObject dataObj = jsonDoc.createNestedObject("data");
	//dataObj["RAM_Disponible"] = ESP.getFreeHeap();
	//dataObj["RAM_TOTAL"] = ESP.getHeapSize();
    //dataObj["Flas_disponible"] = ???; //aun no se
    dataObj["R_STATUS1"] = R_STATUS1 ? true : false;
	dataObj["R_STATUS2"] = R_STATUS2 ? true : false;
    dataObj["BUZZER_STATUS"] = BUZZER_STATUS ? true : false;
	dataObj["dimmer"] = dim;
	dataObj["cpuTemp"] = String(TempCPUValue());
	//dataObj["deviceDS18B20TempC"] = temperatureC; //se requiere libreria adicional comentada en platformio.ini
    //dataObj["deviceDS18B20TempF"] = temperatureF; //se requiere libreria adicional comentada en platformio.ini
    dataObj["tC"] = String(Temperatura());
    dataObj["hum"] = String(Humedad()); 
    dataObj["tmin"] = String(tempMin());
    dataObj["tmax"] = String(tempMax());
    //dataObj["activeTime"] = longTimeStr(millis() / 1000);
    //dataObj["deviceSpiffsUsed"] = String(SPIFFS.usedBytes());
    //dataObj["deviceCpuClock"] = String(getCpuFrequencyMhz());
    //dataObj["deviceFlashSize"] = String(ESP.getFlashChipSize() / (1024.0 * 1024), 2);
    //dataObj["deviceRestartS"] = String(device_restart);
	dataObj["mqttStatus"] = mqttClient.connected() ? true : false;
	dataObj["mqttServer"] = mqttClient.connected() ? F(mqtt_server) : F("server not connected");
	dataObj["wifiStatus"] = WiFi.status() == WL_CONNECTED ? true : false;
	dataObj["wifiRssiStatus"] = WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0;
	//dataObj["wifiQuality"] = WiFi.status() == WL_CONNECTED ? getRSSIasQuality(WiFi.RSSI()) : 0;
    // lo que se puede modificar de manera remota por mqtt
    JsonObject espObj = jsonDoc.createNestedObject("especial");
    espObj["R_NAME1"] = String(R_NAME1);
    espObj["R_NAME2"] = String(R_NAME2);
    espObj["R_LOGIC1"] = R_LOGIC1? true : false;
    espObj["R_LOGIC2"] = R_LOGIC2? true : false;
    espObj["R_DESCRIPTION1"] = String(R_DESCRIPTION1);
    espObj["R_DESCRIPTION2"] = String(R_DESCRIPTION2);
    espObj["ALRM_LOGIC1"] = ALRM_LOGIC1? true : false;
    espObj["ALRM_LOGIC2"] = ALRM_LOGIC2? true : false;
    espObj["ALRM_LOGIC3"] = ALRM_LOGIC3? true : false;
    espObj["ALRM_LOGIC4"] = ALRM_LOGIC4? true : false;
    espObj["ALRM_LOGIC5"] = ALRM_LOGIC5? true : false;
    espObj["ALRM_LOGIC6"] = ALRM_LOGIC6? true : false;
    espObj["ALRM_LOGIC7"] = ALRM_LOGIC7? true : false; 
    espObj["ALRM_NAME1"] = String(ALRM_NAME1);
    espObj["ALRM_NAME2"] = String(ALRM_NAME2);
    espObj["ALRM_NAME3"] = String(ALRM_NAME3);
    espObj["ALRM_NAME4"] = String(ALRM_NAME4);
    espObj["ALRM_NAME5"] = String(ALRM_NAME5);
    espObj["ALRM_NAME6"] = String(ALRM_NAME6);
    espObj["ALRM_NAME7"] = String(ALRM_NAME7);
    espObj["ALRM_STATUS1"] = ALRM_LOGIC1? (digitalRead(ALRM_PIN1)? true : false):(digitalRead(ALRM_PIN1)? false:true);
    espObj["ALRM_STATUS2"] = ALRM_LOGIC2? (digitalRead(ALRM_PIN2)? true : false):(digitalRead(ALRM_PIN2)? false:true);
    espObj["ALRM_STATUS3"] = ALRM_LOGIC3? (digitalRead(ALRM_PIN3)? true : false):(digitalRead(ALRM_PIN3)? false:true);
    espObj["ALRM_STATUS4"] = ALRM_LOGIC4? (digitalRead(ALRM_PIN4)? true : false):(digitalRead(ALRM_PIN4)? false:true);
    espObj["ALRM_STATUS5"] = ALRM_LOGIC5? (digitalRead(ALRM_PIN5)? true : false):(digitalRead(ALRM_PIN5)? false:true);
    espObj["ALRM_STATUS6"] = ALRM_LOGIC6? (digitalRead(ALRM_PIN6)? true : false):(digitalRead(ALRM_PIN6)? false:true);
    espObj["ALRM_STATUS7"] = ALRM_LOGIC7? (digitalRead(ALRM_PIN7)? true : false):(digitalRead(ALRM_PIN7)? false:true);
    //falta por enviar cosas para mi que este no se manda con remote si no se cacha
    espObj["ALRM_TON1"] = ALRM_TON1;
    espObj["ALRM_TON2"] = ALRM_TON2;
    espObj["ALRM_TON3"] = ALRM_TON3;
    espObj["ALRM_TON4"] = ALRM_TON4;
    espObj["ALRM_TON5"] = ALRM_TON5;
    espObj["ALRM_TON6"] = ALRM_TON6;
    espObj["ALRM_TON7"] = ALRM_TON7;
    espObj["ALRM_TOFF1"] = ALRM_TOFF1;
    espObj["ALRM_TOFF2"] = ALRM_TOFF2;
    espObj["ALRM_TOFF3"] = ALRM_TOFF3;
    espObj["ALRM_TOFF4"] = ALRM_TOFF4;
    espObj["ALRM_TOFF5"] = ALRM_TOFF5;
    espObj["ALRM_TOFF6"] = ALRM_TOFF6;
    espObj["ALRM_TOFF7"] = ALRM_TOFF7;
    espObj["ALRM_CONT1"] = ALRM_CONT1;
    espObj["ALRM_CONT2"] = ALRM_CONT2;
    espObj["ALRM_CONT3"] = ALRM_CONT3;
    espObj["ALRM_CONT4"] = ALRM_CONT4;
    espObj["ALRM_CONT5"] = ALRM_CONT5;
    espObj["ALRM_CONT6"] = ALRM_CONT6;
    espObj["ALRM_CONT7"] = ALRM_CONT7;

    serializeJson(jsonDoc, response);
    return response;
}

//----------------------------------------------------------------------
// MQTT Loop Principal se encarga de mantener viva conexión MQTT
// ---------------------------------------------------------------------

void mqttloop(){
    if(mqtt_enable){
        if(!mqttClient.connected()){
            long now = millis();
            if(now < 60000 || ((now -lastMqttReconnectAttempt)>120000)){
                lastMqttReconnectAttempt = now;
                if(mqtt_connect()){
                    lastMqttReconnectAttempt = 0;
                }
            }
        }else{
            mqttClient.loop();//este metodo permita que siga escuchando
            //la informacion callback por ejemplo
            //escuchando las conexiones
        }
    }
}
