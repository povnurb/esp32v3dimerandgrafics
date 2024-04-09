

// definiciones
void dataGraficasReset()
{
    log("INFO", "spiffsGraficas.hpp", "Reiniciando valores");
    for (int i = 0; i < NUM_VALORES; i++)
    {
        vTemp[i] = 0;
    }
    for (int i = 0; i < NUM_VALORES; i++)
    {
        vHum[i] = 0;
    }
    log("INFO", "spiffsGraficas.hpp", "Guardando valores de 0 en Temperatura y humedad");
    dataGraficasSave();
}
bool dataGraficasSave()
{
    DynamicJsonDocument jsonDataTemp(capacityData); // capacidad definida en globales.hpp (5KB)
    // jsonData es el nombre del documento JSON
    // acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará data
    File dataTemp = SPIFFS.open("/dataTemp.json", "w+"); // modo escritura (w+) con el path / que es en raiz y el nombre es dataTemp con la extención json
    // cuando se abre asi se borra el contenido y se sobre escribe
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    if (dataTemp)
    { // si se puede abrir el archivo en modo escritura
        /********************************************************************************
         * Zona configuracion Dispositivo dataTemp.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo file                         *
         *******************************************************************************/
        for (int i = 0; i < NUM_VALORES; i++)
        {
            jsonDataTemp[i] = vTemp[i];
        }
        serializeJsonPretty(jsonDataTemp, dataTemp); // escribe en el archivo file
        // serializeJsonPretty(jsonDataTemp, Serial);
        dataTemp.close(); // ya que todo se leera se cierra el archivo
        // Serial.println("*******************************");
        log("INFO", "spiffsGraficas.hpp", "valor de Temperatura guardada en dataTemp.Json");
        Serial.flush();
        // serializeJsonPretty(jsonSettings,Serial); //lo escribe en el monitor serial
    }
    else
    {
        log("ERROR", "spiffsGraficas.hpp", "no se puede obteber informacion de la data de temperaturas o no existe");
        return false;
    }
    DynamicJsonDocument jsonDataHum(capacityData);     // capacidad definida en globales.hpp (5KB)
    File dataHum = SPIFFS.open("/dataHum.json", "w+"); // modo escritura (w+) con el path / que es en raiz y el nombre es dataHum con la extención json
    // cuando se abre asi se borra el contenido y se sobre escribe
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    if (dataHum)
    { // si se puede abrir el archivo en modo escritura
        /********************************************************************************
         * Zona configuracion Dispositivo dataHum.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo file                         *
         *******************************************************************************/
        for (int i = 0; i < NUM_VALORES; i++)
        {
            jsonDataHum[i] = vHum[i];
        }
        serializeJsonPretty(jsonDataHum, dataHum); // escribe en el archivo file
        // serializeJsonPretty(jsonDataHum, Serial);
        dataHum.close(); // ya que todo se leera se cierra el archivo
        // Serial.println("************************************");
        log("INFO", "spiffsGraficas.hpp", "Valor de Humedad guardad en dataHum.Json");
        Serial.flush();
        // serializeJsonPretty(jsonSettings,Serial); //lo escribe en el monitor serial
    }
    else
    {
        log("ERROR", "spiffsGraficas.hpp", "no se puede obteber informacion de la data de temperaturas o no existe");
        return false;
    }
    // mostrarValoresTemp();
    // mostrarValoresHum();
    return true;
}
bool dataGraficasRead()
{
    log("INFO", "spiffsGraficas.hpp", "Entrando a leer los datos de temperatura y humedad");
    DynamicJsonDocument jsonDataTemp(capacityData); // capacidad definida en globales.hpp (512B)
    // jsonSettings es el nombre del documento JSON
    // acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará file
    File dataTemp = SPIFFS.open("/dataTemp.json", "r"); // modo lectura (r) con el path / que es en raiz y el nombre es settings con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    // se realizara una comparacion if la cual la su interpretación sería, si no se puede deserializar el file en el documento jsonStting
    // la cual sucede la primera vez por que no existe informacion en file
    if (deserializeJson(jsonDataTemp, dataTemp))
    {                                                                         // si no se puede asi funciona esa libreria (deserilizejson) osea si todo ok manda un 0
        dataGraficasReset();                                                  // se ejecuta esta funcion la cual formatea a los valores de fabrica en este caso a 0
        DeserializationError error = deserializeJson(jsonDataTemp, dataTemp); // arroja un error y es guardado como error de DeserializationError
        log("ERROR", "spiffsGraficas.hhp", "Falló la lectura de las temperaturas, tomando valores por defecto");
        if (error)
        {                                                              // si hay un error nos indicara el error (por que fallo)
            log("ERROR", "spiffsGraficas.hhp", String(error.c_str())); // c_str() es un metodo que convierte a String
        }
        return false; // retornamos un valor de false de que no fue posible la deserializacion
    }
    else
    {
        // osea, que si fue posible deserializar de dataTemp el archivo en la memoria ()-> jsonDataTemp
        // asignamos lo que esta en jsonDataTemp a las variable en memorias en tiempo de ejecucion.
        /********************************************************************************
         * Zona recuperacion de temperaturas de dataTemp.json asignando nuevo valores         *
         * a las variables que se encuentran en las variables de vTemp                 *
         *******************************************************************************/
        for (int i = 0; i < NUM_VALORES; i++)
        {
            // jsonDataTemp[i] = vTemp[i]; //error
            vTemp[i] = jsonDataTemp[i];
        }
    }
    dataTemp.close(); // ya que todo se leera se cierra el archivo
    log("INFO", "spiffsGraficas.hpp", "Lectura de las temperaturas OK.");
    Serial.flush();
    DynamicJsonDocument jsonDataHum(capacityData); // capacidad definida en globales.hpp (512B)
    // jsonSettings es el nombre del documento JSON
    // acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará file
    File dataHum = SPIFFS.open("/dataHum.json", "r"); // modo lectura (r) con el path / que es en raiz y el nombre es settings con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    // se realizara una comparacion if la cual la su interpretación sería, si no se puede deserializar el file en el documento jsonStting
    // la cual sucede la primera vez por que no existe informacion en file
    if (deserializeJson(jsonDataHum, dataHum))
    {                                                                       // si no se puede asi funciona esa libreria (deserilizejson) osea si todo ok manda un 0
        dataGraficasReset();                                                // se ejecuta esta funcion la cual formatea a los valores de fabrica en este caso a 0
        DeserializationError error = deserializeJson(jsonDataHum, dataHum); // arroja un error y es guardado como error de DeserializationError
        log("ERROR", "spiffsGraficas.hhp", "Falló la lectura de las Humedades, tomando valores por defecto");
        if (error)
        {                                                              // si hay un error nos indicara el error (por que fallo)
            log("ERROR", "spiffsGraficas.hhp", String(error.c_str())); // c_str() es un metodo que convierte a String
        }
        return false; // retornamos un valor de false de que no fue posible la deserializacion
    }
    else
    {
        // osea, que si fue posible deserializar de dataTemp el archivo en la memoria ()-> jsonDataHum
        // asignamos lo que esta en jsonDataHum a las variable en memorias en tiempo de ejecucion.
        /********************************************************************************
         * Zona recuperacion de temperaturas de dataHum.json asignando nuevo valores         *
         * a las variables que se encuentran en las variables de vHum                 *
         *******************************************************************************/
        for (int i = 0; i < NUM_VALORES; i++)
        {
            // jsonDataHum[i] = vHum[i];
            vHum[i] = jsonDataHum[i];
        }
    }
    // final
    // mostrarValoresTemp();
    // mostrarValoresHum();
    dataHum.close(); // ya que todo se leera se cierra el archivo
    log("INFO", "spiffsGraficas.hpp", "Lectura de las Humedades OK.");
    Serial.flush();
    return true;
}