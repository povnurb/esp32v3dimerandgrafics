//declaracion de la variable
void settingsReset();
void especialReset();
//definicion de la varible
// -------------------------------------------------------------------
// Valores de Fábrica de las variables
// -------------------------------------------------------------------
void settingsReset(){
    /********************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     *******************************************************************************/
    strlcpy(device_id, DeviceID().c_str(), sizeof(device_id));  
    strlcpy(device_name, extractNumbers(DeviceID()).c_str(), sizeof(device_name)); 
    strlcpy(device_user, "admin", sizeof(device_user));
    strlcpy(device_password, "personal", sizeof(device_password));
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo cliente:                                                      *
     ***********************************************************************************************/
    wifi_mode = true; //false en modo ap, //true en modo cliente              
    strlcpy(wifi_ssid,"INFINITUM59W1_2.4",sizeof(wifi_ssid)); //char             
    strlcpy(wifi_password,"unJvpTX5Vp",sizeof(wifi_password)); //char         
    strlcpy(wifi_ssid2,"INFINITUMD378",sizeof(wifi_ssid2));             
    strlcpy(wifi_password2,"Pm2Kj1Jg6j",sizeof(wifi_password2)); //char         
    wifi_ip_static = false; //false es dhcp //true es estatico
    strlcpy(wifi_ipv4,"192.168.1.69",sizeof(wifi_ipv4));   //es una variable de tipo char                        
    strlcpy(wifi_subnet,"255.255.255.0",sizeof(wifi_subnet));   //es una variable de tipo char                                 
    strlcpy(wifi_gateway,"192.168.1.1",sizeof(wifi_gateway));   //es una variable de tipo char                                    
    strlcpy(wifi_dns_primary,"8.8.8.8",sizeof(wifi_dns_primary));   //es una variable de tipo char                            
    strlcpy(wifi_dns_secondary,"8.8.4.4",sizeof(wifi_dns_secondary));   //es una variable de tipo char
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo AccesPoint:                                                   * 
     ***********************************************************************************************/
    strlcpy(ap_ssid,DeviceID().c_str(),sizeof(ap_ssid)); //char             
    strlcpy(ap_password,"personal",sizeof(ap_password)); //char  
    ap_chanel = 9;                  
    ap_visibility = false; //false es visible //true oculta             
    ap_connect = 4;     //hasta 8 segun la libreria wifi.h
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el MQTT:                                                                      *
     * *********************************************************************************************/  
    mqtt_enable = true; //estaba en false
    strlcpy(mqtt_server, "iotmx.com",sizeof(mqtt_server));
    mqtt_port = 1883;       //verificar
    mqtt_retain = true;     //verificar false
    mqtt_qos = 0;
    strlcpy(mqtt_id, DeviceID().c_str(),sizeof(mqtt_id));
    strlcpy(mqtt_user, "lalo",sizeof(mqtt_user));
    strlcpy(mqtt_password, "public",sizeof(mqtt_password));
    mqtt_clean_sessions = true;        //true significa que eliminará todos los temas suscritos (topicos)
    strlcpy(mqtt_willTopic, PathMqttTopic("status").c_str(),sizeof(mqtt_willTopic)); 
    strlcpy(mqtt_extraTopic, "",sizeof(mqtt_extraTopic)); 
    strlcpy(mqtt_willMessage, "{\"connected\":false}",sizeof(mqtt_willMessage)); 
    mqtt_willQoS = 0;
    mqtt_willRetain = true;   //verificar false
    mqtt_time_send = true;
    mqtt_time_interval = 60000;   //esta en milisegundo
    mqtt_status_send = true;  
    /**
     * Zona de Relay
    */
    //RELAY_PIN[8] ={33,15,0,0,0,0,0,0}
    //-------------------------------------------------------------------
    // Zona Dimer
    //-------------------------------------------------------------------
    dim = 0;
    log("INFO","settingsReset.hpp","Se reiniciaron todos los valores por defecto.");
}

void especialReset(){
    /********************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     * variables para los actuadres RELAYS                                          *
     *******************************************************************************/
    strlcpy(R_NAME1, "RELAY1", sizeof(R_NAME1));  //variables de tipo char primero se define la variable (R_NAME1)
    strlcpy(R_NAME2, "RELAY2", sizeof(R_NAME2));  //variables de tipo char primero se define la variable (R_NAME2)
    R_STATUS1 = false; 
    R_STATUS2 = false; 
    R_LOGIC1 = true;   //verificar si no es true ------------------------------------------
    R_LOGIC2 = true;   //verificar si no es true ------------------------------------------
    R_DESCRIPTION1 = "Control D33";
    R_DESCRIPTION2 = "control D15";
    R_TIME1=0;        //tiempo que permanecio el relay operando ON por minutos 
    R_TIME2=0;        //tiempo que permanecio el relay operando ON en minutos
    R_TIMERON1=false;         //indica si se activa el timer del relevador1
    R_TIMERON2=false;         //indica si se activa el timer del relevador2
    TEMPORIZADOR1=false;      //indica si hay un control por horario
    TEMPORIZADOR2=false;      //indica si hay un control por hirario
    R_EVERYDAY1=true;          //si el control se activa todos los dias
    R_EVERYDAY2=true;          //si el control se activa todos los dias
    TIMEONRELAY1="00:00";       //indica a que horas se prende
    TIMEOFFRELAY1="00:00";      //indica a que horas se apaga
    TIMEONRELAY2="00:00";       //indica a que horas se prende
    TIMEOFFRELAY2="00:00";      //indica a que horas se apaga

    /************************************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para los sensores de alarmas:                                                      *
     ***********************************************************************************************/
    ALRM_LOGIC1 = false;
    ALRM_NAME1 = "ALARMA_1";
    ALRM_LOGIC2 = false;
    ALRM_NAME2 = "ALARMA_2";
    ALRM_LOGIC3 = false;
    ALRM_NAME3 = "ALARMA_3";
    ALRM_LOGIC4 = false;
    ALRM_NAME4 = "ALARMA_4";
    ALRM_LOGIC5 = false;
    ALRM_NAME5 = "ALARMA_5";
    ALRM_LOGIC6 = false;
    ALRM_NAME6 = "ALARMA_6";
    ALRM_LOGIC7 = false;
    ALRM_NAME7 = "ALARMA_7";
    /************************************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para los nombres de las alarmas emitidos por el protocolo espnow                   *
     ***********************************************************************************************/

    cambio11="";cambio21="";cambio31="";cambio41="";cambio51="";cambio61="";cambio71="";cambio81="";
    cambio12="";cambio22="";cambio32="";cambio42="";cambio52="";cambio62="";cambio72="";cambio82="";
    cambio13="";cambio23="";cambio33="";cambio43="";cambio53="";cambio63="";cambio73="";cambio83="";
    cambio14="";cambio24="";cambio34="";cambio44="";cambio54="";cambio64="";cambio74="";cambio84="";

    //-------------------------------------------------------------------
    // Zona Dimer
    //-------------------------------------------------------------------
    dim = 0;
    log("INFO","settingsReset.hpp","Se reiniciaron todos los valores de fabrica.");
}

