// archivo de tareas (task)

//---------------------------------------------------------------
// Tarea Loop WiFi & Reconectar
//---------------------------------------------------------------
void TaskWifiReconnect(void *pvParamenters)
{
    esp_task_wdt_reset(); // agregado
    //  para mantener conectado ya sea en modo ap o ap_sta al wifi
    vTaskDelay(10 / portTICK_PERIOD_MS); // podria ser necesario
    (void)pvParamenters;
    while (1)
    { // loop infinito
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (wifi_app == WIFI_AP_STA)
        {
            wifiLoop();
        }
        else if (wifi_app == WIFI_AP)
        {
            wifiAPLoop();
        }
        else
        {
        }
    }
}
//---------------------------------------------------------------
// Tarea Loop MQTT & Reconectar
//---------------------------------------------------------------
void TaskMqttReconnect(void *pvParamenters)
{
    // para mantener conectado ya sea en modo ap o ap_sta al wifi
    (void)pvParamenters;
    while (1)
    {                                        // loop infinito
        esp_task_wdt_reset();                // agregado
        vTaskDelay(10 / portTICK_PERIOD_MS); // podria ser necesario
        if ((WiFi.status() == WL_CONNECTED) && (wifi_app == WIFI_AP_STA))
        {
            if (mqtt_server != 0)
            { // ya estaconectado
                // llamar a la funcion del loop mqtt
                mqttloop();
                // if (mqtt_enable2)
                // {
                //     mqttloop2(); // agregado
                // }
                // Enviar por MQTT el json siempre y cuando este habilitado el envio
                if (mqttClient.connected() && mqtt_status_send)
                {
                    if (millis() - lasMsg > mqtt_time_interval)
                    {
                        lasMsg = millis();
                        mqtt_publish();
                        log("INFO", "tareas.hpp", "Mensaje enviado por MQTT...");
                        Serial.flush();
                    }
                }
            }
        }
    }
}
// ----------------------------------------------------------------------
// Tarea MQTT LED pestañeo
// ----------------------------------------------------------------------
void TaskMQTTLed(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        if (mqtt_enable && mqttClient.connected())
        {
            digitalWrite(MQTTLED, HIGH);
            vTaskDelay(50 / portTICK_PERIOD_MS);
            digitalWrite(MQTTLED, LOW);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            digitalWrite(MQTTLED, LOW);
        }
    }
}
// tareas agregadas por Eduardo Sanchez
// ----------------------------------------------------------------------
// Tarea mostrar pantalla LCD
// ----------------------------------------------------------------------
void TaskLCD(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        mostrar();
        // Serial.println(tempC);
        // vTaskDelay(1000 / portTICK_PERIOD_MS); // probar con 200 o con 1000 y sustituir todo
        vTaskDelay(1000);
    }
}
// ----------------------------------------------------------------------
// Tarea mostrar pantalla LCD
// ----------------------------------------------------------------------
void Taskrelcambio(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        actRele(); // activa los relevadores de manera local
    }
}
// ---------------------------------------------------------------------
// Tarea de encender los relevadores en tiempo
// --------------------------------------------------------------------
void TaskTimeRele(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        temporelevares(); // activa los relevadores de manera local
    }
}
//---------------------------------------------------------
// tarea websocket cada 3 segundos
//----------------------------------------------------------
void TaskWsSend(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS); // esto es igual a 3 segundo lo cual hara lo siguiente:
        wsMessageSend(getJsonIndex(), "", "");
    }
}
//----------------------------------------------------
// tarea para verificar si se requiere restaurar de fabrica
//-----------------------------------------------------------
void TaskRestore(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS); // esto es igual a 3 segundo lo cual hara lo siguiente:
        resetIntLoop();
    }
}
//-----------------------------------------------
// tarea de los valores para la grafica parece no ser necesario se uso un tickers
//------------------------------------------------
void TaskMuestra(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
        muestra();
    }
}
// ---------------------------------------------------------------------
// Tarea que verifica el estado de los relays
// --------------------------------------------------------------------
void TaskVerRelay(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        vTaskDelay(10 / portTICK_PERIOD_MS);
        ctrlRelays(); // activa los relevadores de manera local
    }
}
// ---------------------------------------------------------------------
// Tarea que verifica el estado del potenciometro
// --------------------------------------------------------------------
void TaskSerialDimer(void *pvParameters)
{
    (void)pvParameters;
    while (1)
    {
        // vTaskDelay(2000 / portTICK_PERIOD_MS);
        vTaskDelay(1000);
        serialDimer(); // muestra el valor del potenciometro
    }
}