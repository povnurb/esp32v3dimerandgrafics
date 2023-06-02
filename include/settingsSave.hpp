//declaracion
bool settingsSave(); //Salva los nuevos valores 
bool especialSave();//configuraciones especiales de actuadores (alarmas y relevadores)
//definicion
bool settingsSave(){
    //StaticJsonDocument<capacitySettings> jsonSettings; //diferente definicion
    DynamicJsonDocument jsonSettings(capacitySettings); //capacidad definida en globales.hpp (5KB)
    //jsonSettings es el nombre del documento JSON
    //acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará file 
    File file = SPIFFS.open("/settings.json","w+"); //modo lectura (w+) con el path / que es en raiz y el nombre es settings con la extención json
    // cuando se abre asi se borra el contenido y se sobre escribe 
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    if(file){ //si se puede abrir el archivo en modo escritura
        /********************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo file                         *
         *******************************************************************************/
        jsonSettings["device_id"]=device_id;  
        jsonSettings["device_name"]=device_name; 
        jsonSettings["device_user"]=device_user;
        jsonSettings["device_password"]=device_password;
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo file                                         *
         * variables para el Wifi en modo cliente:                                                      *
         ***********************************************************************************************/
        JsonObject wifiObj = jsonSettings.createNestedObject("wifi"); //definicion para crear la estructura
        wifiObj["wifi_mode"] = wifi_mode;  
        wifiObj["wifi_ssid"]= wifi_ssid;            
        wifiObj["wifi_password"]= wifi_password;          
        wifiObj["wifi_ssid2"]= wifi_ssid2;             
        wifiObj["wifi_password2"]= wifi_password2;         
        wifiObj["wifi_ip_static"] = wifi_ip_static;      //es un bool       
        wifiObj["wifi_ipv4"]= wifi_ipv4;
        wifiObj["wifi_subnet"]= wifi_subnet;
        wifiObj["wifi_gateway"]= wifi_gateway;
        wifiObj["wifi_dns_primary"]= wifi_dns_primary;
        wifiObj["wifi_dns_secondary"]= wifi_dns_secondary;
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para el Wifi en modo AccesPoint:                                                   * 
         ***********************************************************************************************/
        wifiObj["ap_ssid"] = ap_ssid; //char             
        wifiObj["ap_password"] = ap_password; //char  
        wifiObj["ap_chanel"] = ap_chanel;                  
        wifiObj["ap_visibility"] = ap_visibility;              
        wifiObj["ap_connect"] = ap_connect; 
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para el MQTT:                                                                      *
         * *********************************************************************************************/ 
        JsonObject mqttObj = jsonSettings.createNestedObject("mqtt"); 
        mqttObj["mqtt_enable"] = mqtt_enable;
        mqttObj["mqtt_server"] = mqtt_server;
        mqttObj["mqtt_port"] = mqtt_port;
        mqttObj["mqtt_retain"] = mqtt_retain;
        mqttObj["mqtt_qos"] = mqtt_qos;
        mqttObj["mqtt_id"] = mqtt_id;
        mqttObj["mqtt_user"] = mqtt_user;
        mqttObj["mqtt_password"] = mqtt_password;
        mqttObj["mqtt_clean_sessions"] = mqtt_clean_sessions;
        mqttObj["mqtt_willTopic"] = mqtt_willTopic; 

        mqttObj["mqtt_willMessage"] = mqtt_willMessage; 
        mqttObj["mqtt_willQoS"] = mqtt_willQoS;
        mqttObj["mqtt_willRetain"] = mqtt_willRetain;   
        mqttObj["mqtt_time_send"] = mqtt_time_send;
        mqttObj["mqtt_time_interval"] = mqtt_time_interval;   
        mqttObj["mqtt_status_send"] = mqtt_status_send;  

        // ---------------------------------------------------------------------------------
        // TIME settings.json
        // -------------------------------------------------------------------
        jsonSettings["time_ajuste"] = time_ajuste;
        jsonSettings["time_date"] = time_date;
        jsonSettings["time_z_horaria"] = time_z_horaria;
        jsonSettings["time_server"] = time_server;


        jsonSettings["file_version"] = "3.0.0";
        serializeJsonPretty(jsonSettings,file); //escribe en el archivo file
        file.close();// ya que todo se leera se cierra el archivo
        log("INFO","settingsSave.hpp","Configuración general guardada correctamente.");
        //serializeJsonPretty(jsonSettings,Serial); //lo escribe en el monitor serial
        return true;
    }else{
        log("ERROR","settingsSave.hpp","Falló el guardado de la configuracion General.");
        return false;
    }
}

bool especialSave(){
    DynamicJsonDocument jsonEspecial(capacitySettings); //capacidad definida en globales.hpp (5KB)
    //jsonEspecial es el nombre del nuevo documento JSON
    //acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará especial 
    File especial = SPIFFS.open("/especial.json","w+"); //modo lectura (w+) con el path / que es en raiz y el nombre es especial con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    if(especial){//si se puede abrir el archivo en modo lectura
        /********************************************************************************
         * Zona configuracion Dispositivo especial.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo especial                     *
         * variables para los actuadres RELAYS                                          *
         *******************************************************************************/
        JsonObject relayObj = jsonEspecial.createNestedObject("RELAY");
        relayObj["R_NAME1"] = R_NAME1;  //variables de tipo char primero se define la variable (R_NAME1)
        relayObj["R_NAME2"] = R_NAME2;  //variables de tipo char primero se define la variable (R_NAME2)
        relayObj["R_STATUS1"] = R_STATUS1; //como no es un char es diferente este es un bool 
        relayObj["R_STATUS2"] = R_STATUS2; //como no es un char es diferente este es un bool 
        relayObj["R_LOGIC1"] = R_LOGIC1;
        relayObj["R_LOGIC2"] = R_LOGIC2;
        relayObj["R_DESCRIPTION1"] = R_DESCRIPTION1;
        relayObj["R_DESCRIPTION2"] = R_DESCRIPTION2;
        relayObj["R_TIMERON1"]=R_TIMERON1;       //indica si se activa el timer del relevador1
        relayObj["R_TIMERON2"]=R_TIMERON2;       //indica si se activa el timer del relevador2
        relayObj["TEMPORIZADOR1"]=TEMPORIZADOR1;    //indica si hay un control por horario
        relayObj["TEMPORIZADOR2"]=TEMPORIZADOR2;    //indica si hay un control por hirario
        relayObj["TIMEONRELAY1"]=TIMEONRELAY1;       //indica a que horas se prende
        relayObj["TIMEOFFRELAY1"]=TIMEOFFRELAY1;      //indica a que horas se apaga
        relayObj["TIMEONRELAY2"]=TIMEONRELAY2;       //indica a que horas se prende
        relayObj["TIMEOFFRELAY2"]=TIMEOFFRELAY2;      //indica a que horas se apaga
        relayObj["programado1"]=programado1;
        relayObj["programado2"]=programado2;
        /************************************************************************************************
         * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para los sensores de alarmas:                                                      *
         ***********************************************************************************************/
        JsonObject alrmsObj = jsonEspecial.createNestedObject("ALRMS");
        alrmsObj["ALRM_LOGIC1"] = ALRM_LOGIC1;
        alrmsObj["ALRM_NAME1"] = ALRM_NAME1;
        alrmsObj["ALRM_CONT1"] = ALRM_CONT1;
        alrmsObj["ALRM_TON1"] = ALRM_TON1;
        alrmsObj["ALRM_TOFF1"] = ALRM_TOFF1;
        alrmsObj["ALRM_LOGIC2"] = ALRM_LOGIC2;
        alrmsObj["ALRM_NAME2"] = ALRM_NAME2;
        alrmsObj["ALRM_CONT2"] = ALRM_CONT2;
        alrmsObj["ALRM_TON2"] = ALRM_TON2;
        alrmsObj["ALRM_TOFF2"] = ALRM_TOFF2;
        alrmsObj["ALRM_LOGIC3"] = ALRM_LOGIC3;
        alrmsObj["ALRM_NAME3"] = ALRM_NAME3;
        alrmsObj["ALRM_CONT3"] = ALRM_CONT3;
        alrmsObj["ALRM_TON3"] = ALRM_TON3;
        alrmsObj["ALRM_TOFF3"] = ALRM_TOFF3;
        alrmsObj["ALRM_LOGIC4"] = ALRM_LOGIC4;
        alrmsObj["ALRM_NAME4"] = ALRM_NAME4;
        alrmsObj["ALRM_CONT4"] = ALRM_CONT4;
        alrmsObj["ALRM_TON4"] = ALRM_TON4;
        alrmsObj["ALRM_TOFF4"] = ALRM_TOFF4;
        alrmsObj["ALRM_LOGIC5"] = ALRM_LOGIC5;
        alrmsObj["ALRM_NAME5"] = ALRM_NAME5;
        alrmsObj["ALRM_CONT5"] = ALRM_CONT5;
        alrmsObj["ALRM_TON5"] = ALRM_TON5;
        alrmsObj["ALRM_TOFF5"] = ALRM_TOFF5;
        alrmsObj["ALRM_LOGIC6"] = ALRM_LOGIC6;
        alrmsObj["ALRM_NAME6"] = ALRM_NAME6;
        alrmsObj["ALRM_CONT6"] = ALRM_CONT6;
        alrmsObj["ALRM_TON6"] = ALRM_TON6;
        alrmsObj["ALRM_TOFF6"] = ALRM_TOFF6;
        alrmsObj["ALRM_LOGIC7"] = ALRM_LOGIC7;
        alrmsObj["ALRM_NAME7"] = ALRM_NAME7;
        alrmsObj["ALRM_CONT7"] = ALRM_CONT7;
        alrmsObj["ALRM_TON7"] = ALRM_TON7;
        alrmsObj["ALRM_TOFF7"] = ALRM_TOFF7;
        /************************************************************************************************
        * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
        * a las variables que se encuentran en el archivo globales.hpp                                 *
        * variables para los nombres de las alarmas emitidos por el protocolo espnow                   *
        ***********************************************************************************************/

        JsonObject espnowObj = jsonEspecial.createNestedObject("espnow");
        espnowObj["cambio11"]= cambio11;
        espnowObj["cambio21"]= cambio21;
        espnowObj["cambio31"]= cambio31;
        espnowObj["cambio41"]= cambio41;
        espnowObj["cambio51"]= cambio51;
        espnowObj["cambio61"]= cambio61;
        espnowObj["cambio71"]= cambio71;
        espnowObj["cambio81"]= cambio81;
        //----------------------------------------------
        espnowObj["cambio12"]= cambio12;
        espnowObj["cambio22"]= cambio22;
        espnowObj["cambio32"]= cambio32;
        espnowObj["cambio42"]= cambio42;
        espnowObj["cambio52"]= cambio52;
        espnowObj["cambio62"]= cambio62;
        espnowObj["cambio72"]= cambio72;
        espnowObj["cambio82"]= cambio82;
        //----------------------------------------------
        espnowObj["cambio13"]= cambio13;
        espnowObj["cambio23"]= cambio23;
        espnowObj["cambio33"]= cambio33;
        espnowObj["cambio43"]= cambio43;
        espnowObj["cambio53"]= cambio53;
        espnowObj["cambio63"]= cambio63;
        espnowObj["cambio73"]= cambio73;
        espnowObj["cambio83"]= cambio83;
        //----------------------------------------------
        espnowObj["cambio14"]= cambio14;
        espnowObj["cambio24"]= cambio24;
        espnowObj["cambio34"]= cambio34;
        espnowObj["cambio44"]= cambio44;
        espnowObj["cambio54"]= cambio54;
        espnowObj["cambio64"]= cambio64;
        espnowObj["cambio74"]= cambio74;
        espnowObj["cambio84"]= cambio84;
        /************************************************************************************************
         * Nuevos valores a las variables que se encuentran en el archivo globales.hpp                  *
         * variables para el MQTT2:                                                                     *
         * *********************************************************************************************/ 
        JsonObject mqtt2Obj = jsonEspecial.createNestedObject("mqtt2");
        mqtt2Obj["mqtt_enable2"]=mqtt_enable2;
        mqtt2Obj["mqtt_user2"]=mqtt_user2;
        mqtt2Obj["mqtt_id2"]=mqtt_id2;
        mqtt2Obj["mqtt_server2"]=mqtt_server2;
        mqtt2Obj["mqtt_port2"]=mqtt_port2; 
        mqtt2Obj["mqtt_willTopic2"]=mqtt_willTopic2;
        mqtt2Obj["mqtt_password2"]=mqtt_password2;
        mqtt2Obj["mqtt_willQoS2"]=mqtt_willQoS2; 
        mqtt2Obj["mqtt_clean_sessions2"]=mqtt_clean_sessions2; 
        mqtt2Obj["mqtt_time_send2"] = mqtt_time_send2;
        mqtt2Obj["mqtt_willRetain2"]=mqtt_willRetain2; 
        mqtt2Obj["mqtt_willMessage2"]=mqtt_willMessage2;
        mqtt2Obj["mqtt_extraTopic"] = mqtt_extraTopic;
        mqtt2Obj["mqtt_retain2"]=mqtt_retain2;
        mqtt2Obj["mqtt_qos2"] = mqtt_qos2;
        mqtt2Obj["mqtt_time_interval2"] = mqtt_time_interval2;
        mqtt2Obj["mqtt_status_send2"] = mqtt_status_send2;
        //-------------------------------------------------------------------
        // Zona Dimer
        //-------------------------------------------------------------------
        JsonObject dimmerObj = jsonEspecial.createNestedObject("DIMER");
        dimmerObj["dim_value"] = dim;
        
        serializeJsonPretty(jsonEspecial,especial); //lo escribe en el archivo especial
        especial.close();// ya que todo se leera se cierra el archivo
        Serial.println("");
        log("INFO","settingsSave.hpp","Configuraciones guardadas de los actuadores OK.");
        //serializeJsonPretty(jsonEspecial,Serial); //muestra la informacion guardada en el monitor serial
        return true;
    }else{
        log("ERROR","settingsSave.hpp","Falló el guardado de la configuracion Especial.");
        return false;
    }
}