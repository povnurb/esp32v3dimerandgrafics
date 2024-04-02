// declaracion de la variable
void settingsReset();
void especialReset();
// definicion de la varible
//  -------------------------------------------------------------------
//  Valores de Fábrica de las variables
//  -------------------------------------------------------------------
void settingsReset()
{
    /********************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     *******************************************************************************/
    strlcpy(LUGAR, "IGL-PTTI", sizeof(LUGAR));
    strlcpy(device_id, DeviceID().c_str(), sizeof(device_id));
    strlcpy(device_name, extractNumbers(DeviceID()).c_str(), sizeof(device_name)); // probar con iglptti
    strlcpy(device_user, "admin", sizeof(device_user));
    strlcpy(device_password, "personal", sizeof(device_password));
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo cliente:                                                      *
     ***********************************************************************************************/
    wifi_mode = true;                                            // false en modo ap, //true en modo cliente
    strlcpy(wifi_ssid, "INFINITUM59W1_2.4", sizeof(wifi_ssid));  // char
    strlcpy(wifi_password, "unJvpTX5Vp", sizeof(wifi_password)); // char
    strlcpy(wifi_ssid2, "INFINITUMD378", sizeof(wifi_ssid2));
    strlcpy(wifi_password2, "Pm2Kj1Jg6j", sizeof(wifi_password2));      // char
    wifi_ip_static = false;                                             // false es dhcp //true es estatico
    strlcpy(wifi_ipv4, "192.168.1.69", sizeof(wifi_ipv4));              // es una variable de tipo char
    strlcpy(wifi_subnet, "255.255.255.0", sizeof(wifi_subnet));         // es una variable de tipo char
    strlcpy(wifi_gateway, "192.168.1.1", sizeof(wifi_gateway));         // es una variable de tipo char
    strlcpy(wifi_dns_primary, "8.8.8.8", sizeof(wifi_dns_primary));     // es una variable de tipo char
    strlcpy(wifi_dns_secondary, "8.8.4.4", sizeof(wifi_dns_secondary)); // es una variable de tipo char
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo AccesPoint:                                                   *
     ***********************************************************************************************/
    strlcpy(ap_ssid, DeviceID().c_str(), sizeof(ap_ssid)); // char
    strlcpy(ap_password, "personal", sizeof(ap_password)); // char
    ap_chanel = 9;
    ap_visibility = false; // false es visible //true oculta
    ap_connect = 4;        // hasta 8 segun la libreria wifi.h
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el MQTT:                                                                      *
     * *********************************************************************************************/
    mqtt_enable = true; // estaba en false
    strlcpy(mqtt_server, "iotmx.com", sizeof(mqtt_server));
    mqtt_port = 1883;   // verificar
    mqtt_retain = true; // verificar false
    mqtt_qos = 0;
    strlcpy(mqtt_id, DeviceID().c_str(), sizeof(mqtt_id));
    strlcpy(mqtt_user, "lalo", sizeof(mqtt_user));
    strlcpy(mqtt_password, "public", sizeof(mqtt_password));
    mqtt_clean_sessions = true; // true significa que eliminará todos los temas suscritos (topicos)
    strlcpy(mqtt_willTopic, PathMqttTopic("status").c_str(), sizeof(mqtt_willTopic));

    strlcpy(mqtt_willMessage, "{\"connected\":false}", sizeof(mqtt_willMessage));
    mqtt_willQoS = 0;
    mqtt_willRetain = true; // verificar false
    mqtt_time_send = true;
    mqtt_time_interval = 60000; // esta en milisegundo
    mqtt_status_send = true;
    //--------------------------------------------------------------------------------------------
    /********************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     * variables para los actuadores RELAYS                                          *
     *******************************************************************************/
    strlcpy(R_NAME1, "RELAY1", sizeof(R_NAME1)); // variables de tipo char primero se define la variable (R_NAME1)
    strlcpy(R_NAME2, "RELAY2", sizeof(R_NAME2)); // variables de tipo char primero se define la variable (R_NAME2)
    R_STATUS1 = false;
    R_STATUS2 = false;
    R_LOGIC1 = true; // verificar si no es true ------------------------------------------
    R_LOGIC2 = true; // verificar si no es true ------------------------------------------
    R_DESCRIPTION1 = "Control D04";
    R_DESCRIPTION2 = "control D02";
    R_TIMERON1 = false;      // indica si se activa el timer del relevador1
    R_TIMERON2 = false;      // indica si se activa el timer del relevador2
    TEMPORIZADOR1 = false;   // indica si hay un control por horario
    TEMPORIZADOR2 = false;   // indica si hay un control por hirario
    TIMEONRELAY1 = "00:00";  // indica a que horas se prende
    TIMEOFFRELAY1 = "00:00"; // indica a que horas se apaga
    TIMEONRELAY2 = "00:00";  // indica a que horas se prende
    TIMEOFFRELAY2 = "00:00"; // indica a que horas se apaga
    programado1 = false;
    programado2 = false;
    /************************************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para los sensores de alarmas:                                                      *
     ***********************************************************************************************/
    ALRM_LOGIC1 = false;
    ALRM_NAME1 = "ALTA TEMPERATURA EN SALA";
    ALRM_LOGIC2 = false;
    ALRM_NAME2 = "F. COMPRESOR A";
    ALRM_LOGIC3 = false;
    ALRM_NAME3 = "F. COMPRESOR B";
    ALRM_LOGIC4 = false;
    ALRM_NAME4 = "CLIMA A";
    ALRM_LOGIC5 = false;
    ALRM_NAME5 = "CLIMA B";
    //--------------------------------------------------------------------------------------------
    //  Zona Dimer
    //-------------------------------------------------------------------
    dim = 0;
    // -------------------------------------------------------------------
    // Time settings.json
    // -------------------------------------------------------------------
    time_ajuste = true;
    strlcpy(time_date, "2024-06-04T14:09", sizeof(time_date));
    time_z_horaria = -14400;
    strlcpy(time_server, "time.nist.gov", sizeof(time_server));
    /**
     * Zona de Relay
     */
    // RELAY_PIN[8] ={33,15,0,0,0,0,0,0}
    log("INFO", "settingsReset.hpp", "Se reiniciaron todos los valores por defecto.");
}

void especialReset()
{
    /********************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     *******************************************************************************/
    strlcpy(LUGAR, "IGL-PTTI", sizeof(LUGAR));
    strlcpy(device_id, DeviceID().c_str(), sizeof(device_id));
    strlcpy(device_name, extractNumbers(DeviceID()).c_str(), sizeof(device_name));
    strlcpy(device_user, "admin", sizeof(device_user));
    strlcpy(device_password, "personal", sizeof(device_password));
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo cliente:                                                      *
     ***********************************************************************************************/
    wifi_mode = false;                                           // false en modo ap, //true en modo cliente
    strlcpy(wifi_ssid, "INFINITUMD378", sizeof(wifi_ssid));      // char
    strlcpy(wifi_password, "Pm2Kj1Jg6j", sizeof(wifi_password)); // char
    strlcpy(wifi_ssid2, "INFINITUM59W1_2.4", sizeof(wifi_ssid2));
    strlcpy(wifi_password2, "unJvpTX5Vp", sizeof(wifi_password2));      // char
    wifi_ip_static = false;                                             // false es dhcp //true es estatico
    strlcpy(wifi_ipv4, "192.168.1.69", sizeof(wifi_ipv4));              // es una variable de tipo char
    strlcpy(wifi_subnet, "255.255.255.0", sizeof(wifi_subnet));         // es una variable de tipo char
    strlcpy(wifi_gateway, "192.168.1.1", sizeof(wifi_gateway));         // es una variable de tipo char
    strlcpy(wifi_dns_primary, "8.8.8.8", sizeof(wifi_dns_primary));     // es una variable de tipo char
    strlcpy(wifi_dns_secondary, "8.8.4.4", sizeof(wifi_dns_secondary)); // es una variable de tipo char
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el Wifi en modo AccesPoint:                                                   *
     ***********************************************************************************************/
    strlcpy(ap_ssid, DeviceID().c_str(), sizeof(ap_ssid)); // char
    strlcpy(ap_password, "personal", sizeof(ap_password)); // char
    ap_chanel = 9;
    ap_visibility = false; // false es visible //true oculta
    ap_connect = 4;        // hasta 8 segun la libreria wifi.h
    /************************************************************************************************
     * Zona configuracion Dispositivo settings.json asignando nuevo valores                         *
     * a las variables que se encuentran en el archivo globales.hpp                                 *
     * variables para el MQTT:                                                                      *
     * *********************************************************************************************/
    mqtt_enable = false; // estaba en false
    strlcpy(mqtt_server, "iotmx.com", sizeof(mqtt_server));
    mqtt_port = 1883;   // verificar
    mqtt_retain = true; // verificar false
    mqtt_qos = 0;
    strlcpy(mqtt_id, DeviceID().c_str(), sizeof(mqtt_id));
    strlcpy(mqtt_user, "lalo", sizeof(mqtt_user));
    strlcpy(mqtt_password, "public", sizeof(mqtt_password));
    mqtt_clean_sessions = true; // true significa que eliminará todos los temas suscritos (topicos)
    strlcpy(mqtt_willTopic, PathMqttTopic("status").c_str(), sizeof(mqtt_willTopic));

    strlcpy(mqtt_willMessage, "{\"connected\":false}", sizeof(mqtt_willMessage));
    mqtt_willQoS = 0;
    mqtt_willRetain = true; // verificar false
    mqtt_time_send = true;
    mqtt_time_interval = 60000; // esta en milisegundo
    mqtt_status_send = true;
    //--------------------------------------------------------------------------------------------
    /********************************************************************************
     * Zona configuracion Dispositivo especial.json asignando nuevo valores         *
     * a las variables que se encuentran en el archivo globales.hpp                 *
     * variables para los actuadores RELAYS                                          *
     *******************************************************************************/
    strlcpy(R_NAME1, "RELAY1", sizeof(R_NAME1)); // variables de tipo char primero se define la variable (R_NAME1)
    strlcpy(R_NAME2, "RELAY2", sizeof(R_NAME2)); // variables de tipo char primero se define la variable (R_NAME2)
    R_STATUS1 = false;
    R_STATUS2 = false;
    R_LOGIC1 = true; // verificar si no es true ------------------------------------------
    R_LOGIC2 = true; // verificar si no es true ------------------------------------------
    R_DESCRIPTION1 = "Control D04";
    R_DESCRIPTION2 = "control D02";
    R_TIMERON1 = false;      // indica si se activa el timer del relevador1
    R_TIMERON2 = false;      // indica si se activa el timer del relevador2
    TEMPORIZADOR1 = false;   // indica si hay un control por horario
    TEMPORIZADOR2 = false;   // indica si hay un control por hirario
    TIMEONRELAY1 = "00:00";  // indica a que horas se prende
    TIMEOFFRELAY1 = "00:00"; // indica a que horas se apaga
    TIMEONRELAY2 = "00:00";  // indica a que horas se prende
    TIMEOFFRELAY2 = "00:00"; // indica a que horas se apaga
    programado1 = false;
    programado2 = false;
    // /************************************************************************************************
    //  * Zona configuracion Dispositivo especial.json asignando nuevo valores                         *
    //  * a las variables que se encuentran en el archivo globales.hpp                                 *
    //  * variables para los sensores de alarmas:                                                      *
    //  ***********************************************************************************************/
    ALRM_LOGIC1 = false;
    ALRM_NAME1 = "ALTA TEMPERATURA EN SALA";
    ALRM_LOGIC2 = false;
    ALRM_NAME2 = "F. COMPRESOR A";
    ALRM_LOGIC3 = false;
    ALRM_NAME3 = "F. COMPRESOR B";
    ALRM_LOGIC4 = false;
    ALRM_NAME4 = "CLIMA A";
    ALRM_LOGIC5 = false;
    ALRM_NAME5 = "CLIMA B";
    // //-------------------------------------------------------------------
    // // Zona Dimer
    // //-------------------------------------------------------------------
    dim = 0;
    // -------------------------------------------------------------------
    // Time settings.json
    // -------------------------------------------------------------------
    time_ajuste = true;
    strlcpy(time_date, "2024-06-04T00:00", sizeof(time_date));
    time_z_horaria = -14400;
    strlcpy(time_server, "time.nist.gov", sizeof(time_server));
    log("INFO", "specialReset.hpp", "Se reiniciaron todos los valores de fabrica.");
}
