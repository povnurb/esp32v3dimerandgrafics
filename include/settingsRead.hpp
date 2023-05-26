//--------------------------------------------------------------------
//Declaracion de funciones antes de la definicion
//---------------------------------------------------------------------
//es especial por que la intencio en modifcar con un archivo json enviado por mqtt
bool settingsRead(); //lee el valor de las variables preestablecidas
bool especialRead(); //lee los valores de los actuadores preestablecidos anteriormente
void settingsReset();   //resetea a los valores preestablecidos
void especialReset();    //resetea los valores de los actuadores a los configurados de fabrica
//-------------------------------------------------------------
// definicion de bool settiigsRead() consiste en leer settings.json
//-------------------------------------------------------------
bool settingsRead(){
    DynamicJsonDocument jsonSettings(capacitySettings); //capacidad definida en globales.hpp (5KB)
    //jsonSettings es el nombre del documento JSON
    //acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará file 
    File file = SPIFFS.open("/settings.json","r"); //modo lectura (r) con el path / que es en raiz y el nombre es settings con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    //se realizara una comparacion if la cual la su interpretación sería, si no se puede deserializar el file en el documento jsonStting 
    //la cual sucede la primera vez por que no existe informacion en file
    if(deserializeJson(jsonSettings,file)){ //si no se puede asi funciona esa libreria (deserilizejson) osea si todo ok manda un 0
        settingsReset(); //se ejecuta esta funcion la cual formatea a los valores de fabrica
        DeserializationError error = deserializeJson(jsonSettings, file);//arroja un error y es guardado como error de DeserializationError
        log("ERROR","settings.hhp", "Falló la lectura de las configuraciones, tomando valores por defecto");
        if(error){ //si hay un error nos indicara el error (por que fallo)
            log("ERROR","settingsRead.hhp", String(error.c_str())); //c_str() es un metodo que convierte a String
        }
        return false; //retornamos un valor de false de que no fue posible la deserializacion
    }else{
        //osea, que si fue posible deserializar de file el archivo en la memoria ()-> jsonSettings
        //asignamos lo que esta en jsonSettings a las variable en memorias en tiempo de ejecucion.
        /********************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo globales.hpp                 *
         *******************************************************************************/
        strlcpy(device_id, jsonSettings["device_id"], sizeof(device_id));  //variables de tipo char primero se define la variable (device_id)
        strlcpy(device_name, jsonSettings["device_name"], sizeof(device_name)); //luego el dato (jsonSettings["device_name"])
        strlcpy(device_user, jsonSettings["device_user"], sizeof(device_user));// y por ultimo el tamaño
        strlcpy(device_password, jsonSettings["device_password"], sizeof(device_password));
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para el Wifi en modo cliente:                                                      *
         ***********************************************************************************************/
        wifi_mode = jsonSettings["wifi"]["wifi_mode"]; //como no es un char es diferente este es un bool              
        strlcpy(wifi_ssid,jsonSettings["wifi"]["wifi_ssid"],sizeof(wifi_ssid)); //char             
        strlcpy(wifi_password,jsonSettings["wifi"]["wifi_password"],sizeof(wifi_password)); //char         
        strlcpy(wifi_ssid2,jsonSettings["wifi"]["wifi_ssid2"],sizeof(wifi_ssid2));             
        strlcpy(wifi_password2,jsonSettings["wifi"]["wifi_password2"],sizeof(wifi_password2)); //char         
        wifi_ip_static = jsonSettings["wifi"]["wifi_ip_static"];      //es un bool       
        strlcpy(wifi_ipv4,jsonSettings["wifi"]["wifi_ipv4"],sizeof(wifi_ipv4));   //es una variable de tipo char                        
        strlcpy(wifi_subnet,jsonSettings["wifi"]["wifi_subnet"],sizeof(wifi_subnet));   //es una variable de tipo char                                 
        strlcpy(wifi_gateway,jsonSettings["wifi"]["wifi_gateway"],sizeof(wifi_gateway));   //es una variable de tipo char                                    
        strlcpy(wifi_dns_primary,jsonSettings["wifi"]["wifi_dns_primary"],sizeof(wifi_dns_primary));   //es una variable de tipo char                            
        strlcpy(wifi_dns_secondary,jsonSettings["wifi"]["wifi_dns_secondary"],sizeof(wifi_dns_secondary));   //es una variable de tipo char
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para el Wifi en modo AccesPoint:                                                   * 
         ***********************************************************************************************/
        strlcpy(ap_ssid,jsonSettings["wifi"]["ap_ssid"],sizeof(ap_ssid)); //char             
        strlcpy(ap_password,jsonSettings["wifi"]["ap_password"],sizeof(ap_password)); //char  
        ap_chanel = jsonSettings["wifi"]["ap_chanel"];                  
        ap_visibility = jsonSettings["wifi"]["ap_visibility"];              
        ap_connect = jsonSettings["wifi"]["ap_connect"]; 
        /************************************************************************************************
         * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para el MQTT:                                                                      *
         * *********************************************************************************************/  
        mqtt_enable = jsonSettings["mqtt"]["mqtt_enable"];
        strlcpy(mqtt_server, jsonSettings["mqtt"]["mqtt_server"],sizeof(mqtt_server));
        mqtt_port = jsonSettings["mqtt"]["mqtt_port"];
        mqtt_retain = jsonSettings["mqtt"]["mqtt_retain"];
        mqtt_qos = jsonSettings["mqtt"]["mqtt_qos"];
        strlcpy(mqtt_id, jsonSettings["mqtt"]["mqtt_id"],sizeof(mqtt_id));
        strlcpy(mqtt_user, jsonSettings["mqtt"]["mqtt_user"],sizeof(mqtt_user));
        strlcpy(mqtt_password, jsonSettings["mqtt"]["mqtt_password"],sizeof(mqtt_password));
        mqtt_clean_sessions = jsonSettings["mqtt"]["mqtt_clean_sessions"];
        strlcpy(mqtt_willTopic, jsonSettings["mqtt"]["mqtt_willTopic"],sizeof(mqtt_willTopic)); 
        strlcpy(mqtt_extraTopic, jsonSettings["mqtt"]["mqtt_extraTopic"],sizeof(mqtt_extraTopic));
        strlcpy(mqtt_willMessage, jsonSettings["mqtt"]["mqtt_willMessage"],sizeof(mqtt_willMessage)); 
        mqtt_willQoS = jsonSettings["mqtt"]["mqtt_willQoS"];
        mqtt_willRetain = jsonSettings["mqtt"]["mqtt_willRetain"];   
        mqtt_time_send = jsonSettings["mqtt"]["mqtt_time_send"];
        mqtt_time_interval = jsonSettings["mqtt"]["mqtt_time_interval"];   
        mqtt_status_send = jsonSettings["mqtt"]["mqtt_status_send"];  

        file.close();// ya que todo se leera se cierra el archivo
        log("INFO","settingsRead.hpp","Lectura de las configuraciones OK.");
        return true;
    }
}

bool especialRead(){
    DynamicJsonDocument jsonEspecial(capacitySettings); //capacidad definida en globales.hpp (5KB)
    //jsonEspecial es el nombre del nuevo documento JSON
    //acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará especial 
    File especial = SPIFFS.open("/especial.json","r"); //modo lectura (r) con el path / que es en raiz y el nombre es especial con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    //se realizara una comparacion if la cual la su interpretación sería, si no se puede deserializar el file en el documento jsonStting 
    //la cual sucede la primera vez por que no existe informacion en especial
    if(deserializeJson(jsonEspecial,especial)){ //si no se puede asi funciona esa libreria (deserilizejson) osea si todo ok manda un 0
        especialReset(); //se ejecuta esta funcion la cual formatea a los valores de fabrica
        DeserializationError error = deserializeJson(jsonEspecial, especial);//arroja un error y es guardado como error de DeserializationError
        log("ERROR","especial.hhp", "Falló la lectura de las configuraciones, tomando valores por defecto");
        if(error){ //si hay un error nos indicara el error (por que fallo)
            log("ERROR","especialRead.hhp", String(error.c_str())); //c_str() es un metodo que convierte a String
        }
        return false; //retornamos un valor de false de que no fue posible la deserializacion
    }else{
        //osea, que si fue posible deserializar de especial el archivo en la memoria ()-> jsonEspecial
        //asignamos lo que esta en jsonEspecial a las variable en memorias en tiempo de ejecucion.
        /********************************************************************************
         * Zona configuracion Dispositivo especial.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo globales.hpp                 *
         * variables para los actuadres RELAYS                                          *
         *******************************************************************************/
        strlcpy(R_NAME1, jsonEspecial["RELAY"]["R_NAME1"], sizeof(R_NAME1));  //variables de tipo char primero se define la variable (R_NAME1)
        strlcpy(R_NAME2, jsonEspecial["RELAY"]["R_NAME2"], sizeof(R_NAME2));  //variables de tipo char primero se define la variable (R_NAME2)
        R_STATUS1 = jsonEspecial["RELAY"]["R_STATUS1"].as<bool>(); //como no es un char es diferente este es un bool 
        R_STATUS2 = jsonEspecial["RELAY"]["R_STATUS2"].as<bool>(); //como no es un char es diferente este es un bool 
        R_LOGIC1 = jsonEspecial["RELAY"]["R_LOGIC1"].as<bool>();
        R_LOGIC2 = jsonEspecial["RELAY"]["R_LOGIC2"].as<bool>();
        R_DESCRIPTION1 = jsonEspecial["RELAY"]["R_DESCRIPTION1"].as<String>();
        R_DESCRIPTION2 = jsonEspecial["RELAY"]["R_DESCRIPTION2"].as<String>();
        R_TIMERON1=jsonEspecial["RELAY"]["R_TIMERON1"].as<bool>();         //indica si se activa el timer del relevador1
        R_TIMERON2=jsonEspecial["RELAY"]["R_TIMERON2"].as<bool>();         //indica si se activa el timer del relevador2
        R_TIME1=jsonEspecial["RELAY"]["R_TIME1"];        //tiempo que permanecio el relay operando ON por minutos 
        R_TIME2=jsonEspecial["RELAY"]["R_TIME2"];        //tiempo que permanecio el relay operando ON en minutos
        TEMPORIZADOR1=jsonEspecial["RELAY"]["TEMPORIZADOR1"].as<bool>();      //indica si hay un control por horario
        TEMPORIZADOR2=jsonEspecial["RELAY"]["TEMPORIZADOR2"].as<bool>();      //indica si hay un control por hirario
        R_EVERYDAY1=jsonEspecial["RELAY"]["R_EVERYDAY1"].as<bool>();          //si el control por tiempo solo se activa una vez en caso de false queda establecido todos los dias
        R_EVERYDAY2=jsonEspecial["RELAY"]["R_EVERYDAY2"].as<bool>();          //si el control por tiempo solo se activa una vez
        TIMEONRELAY1=jsonEspecial["RELAY"]["TIMEONRELAY1"].as<String>();       //indica a que horas se prende
        TIMEOFFRELAY1=jsonEspecial["RELAY"]["TIMEOFFRELAY1"].as<String>();      //indica a que horas se apaga
        TIMEONRELAY2=jsonEspecial["RELAY"]["TIMEONRELAY2"].as<String>();       //indica a que horas se prende
        TIMEOFFRELAY2=jsonEspecial["RELAY"]["TIMEOFFRELAY2"].as<String>();      //indica a que horas se apaga
        programado1=jsonEspecial["RELAY"]["programado1"].as<bool>();
        programado2=jsonEspecial["RELAY"]["programado2"].as<bool>();
        /************************************************************************************************
         * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para los sensores de alarmas:                                                      *
         ***********************************************************************************************/
        ALRM_LOGIC1 = jsonEspecial["ALRMS"]["ALRM_LOGIC1"].as<bool>();
        ALRM_NAME1 = jsonEspecial["ALRMS"]["ALRM_NAME1"].as<String>();
        ALRM_LOGIC2 = jsonEspecial["ALRMS"]["ALRM_LOGIC2"].as<bool>();
        ALRM_NAME2 = jsonEspecial["ALRMS"]["ALRM_NAME2"].as<String>();
        ALRM_LOGIC3 = jsonEspecial["ALRMS"]["ALRM_LOGIC3"].as<bool>();
        ALRM_NAME3 = jsonEspecial["ALRMS"]["ALRM_NAME3"].as<String>();
        ALRM_LOGIC4 = jsonEspecial["ALRMS"]["ALRM_LOGIC4"].as<bool>();
        ALRM_NAME4 = jsonEspecial["ALRMS"]["ALRM_NAME4"].as<String>();
        ALRM_LOGIC5 = jsonEspecial["ALRMS"]["ALRM_LOGIC5"].as<bool>();
        ALRM_NAME5 = jsonEspecial["ALRMS"]["ALRM_NAME5"].as<String>();
        ALRM_LOGIC6 = jsonEspecial["ALRMS"]["ALRM_LOGIC6"].as<bool>();
        ALRM_NAME6 = jsonEspecial["ALRMS"]["ALRM_NAME6"].as<String>();
        ALRM_LOGIC7 = jsonEspecial["ALRMS"]["ALRM_LOGIC7"].as<bool>();
        ALRM_NAME7 = jsonEspecial["ALRMS"]["ALRM_NAME7"].as<String>();
        /************************************************************************************************
         * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
         * a las variables que se encuentran en el archivo globales.hpp                                 *
         * variables para los nombres de alarmas emitidos por el protocolo espnow                       *
         ***********************************************************************************************/

        cambio11 = jsonEspecial["espnow"]["cambio11"].as<String>();
        cambio21 = jsonEspecial["espnow"]["cambio21"].as<String>();
        cambio31 = jsonEspecial["espnow"]["cambio31"].as<String>();
        cambio41 = jsonEspecial["espnow"]["cambio41"].as<String>();
        cambio51 = jsonEspecial["espnow"]["cambio51"].as<String>();
        cambio61 = jsonEspecial["espnow"]["cambio61"].as<String>();
        cambio71 = jsonEspecial["espnow"]["cambio71"].as<String>();
        cambio81 = jsonEspecial["espnow"]["cambio81"].as<String>();
        //----------------------------------------------
        cambio12 = jsonEspecial["espnow"]["cambio12"].as<String>();
        cambio22 = jsonEspecial["espnow"]["cambio22"].as<String>();
        cambio32 = jsonEspecial["espnow"]["cambio32"].as<String>();
        cambio42 = jsonEspecial["espnow"]["cambio42"].as<String>();
        cambio52 = jsonEspecial["espnow"]["cambio52"].as<String>();
        cambio62 = jsonEspecial["espnow"]["cambio62"].as<String>();
        cambio72 = jsonEspecial["espnow"]["cambio72"].as<String>();
        cambio82 = jsonEspecial["espnow"]["cambio82"].as<String>();
        //----------------------------------------------
        cambio13 = jsonEspecial["espnow"]["cambio13"].as<String>();
        cambio23 = jsonEspecial["espnow"]["cambio23"].as<String>();
        cambio33 = jsonEspecial["espnow"]["cambio33"].as<String>();
        cambio43 = jsonEspecial["espnow"]["cambio43"].as<String>();
        cambio53 = jsonEspecial["espnow"]["cambio53"].as<String>();
        cambio63 = jsonEspecial["espnow"]["cambio63"].as<String>();
        cambio73 = jsonEspecial["espnow"]["cambio73"].as<String>();
        cambio83 = jsonEspecial["espnow"]["cambio83"].as<String>();
        //----------------------------------------------
        cambio14 = jsonEspecial["espnow"]["cambio14"].as<String>();
        cambio24 = jsonEspecial["espnow"]["cambio24"].as<String>();
        cambio34 = jsonEspecial["espnow"]["cambio34"].as<String>();
        cambio44 = jsonEspecial["espnow"]["cambio44"].as<String>();
        cambio54 = jsonEspecial["espnow"]["cambio54"].as<String>();
        cambio64 = jsonEspecial["espnow"]["cambio64"].as<String>();
        cambio74 = jsonEspecial["espnow"]["cambio74"].as<String>();
        cambio84 = jsonEspecial["espnow"]["cambio84"].as<String>();
        //-------------------------------------------------------------------
        // Zona Dimer
        //-------------------------------------------------------------------
        dim = jsonEspecial["DIMER"]["dim_value"];
        
        especial.close();// ya que todo se leera se cierra el archivo
        log("INFO","settingsRead.hpp","Lectura de las configuraciones de los actuadores OK.");
        return true;
    }
}