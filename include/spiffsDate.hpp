

// definiciones
void dataDateReset()
{
    log("INFO", "spiffsDate.hpp", "Reiniciando valores de fecha y hora de fabrica");
    time_dateSalvado = time_date;
    dataDateSave();
}
bool dataDateSave()
{
    DynamicJsonDocument jsonDataTime(capacityDate); // capacidad definida en globales.hpp (5KB)
    // jsonData es el nombre del documento JSON
    // acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará data
    File dataTime = SPIFFS.open("/dataTime.json", "w+"); // modo escritura (w+) con el path / que es en raiz y el nombre es dataTime con la extención json
    // cuando se abre asi se borra el contenido y se sobre escribe
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    if (dataTime)
    { // si se puede abrir el archivo en modo escritura
        /********************************************************************************
         * Zona configuracion Dispositivo dataTime.json asignando nuevo valores         *
         * a las variables que se encuentran en el archivo file                         *
         *******************************************************************************/

        RTC.read(tm);
        jsonDataTime["dia"] = tm.Day;
        jsonDataTime["mes"] = tm.Month;
        jsonDataTime["anio"] = tmYearToCalendar(tm.Year); // no se porque pero se tiene que restar solo tm.year da 72
        jsonDataTime["hora"] = tm.Hour;
        jsonDataTime["minuto"] = tm.Minute;

        serializeJsonPretty(jsonDataTime, dataTime); // escribe en el archivo dataTime
        // serializeJsonPretty(jsonDataTime, Serial);
        dataTime.close(); // ya que todo se leera se cierra el archivo
        // Serial.println("*******************************");
        log("INFO", "spiffsDate.hpp", "fecha guardada en dataTime.Json");
        Serial.flush();
        // serializeJsonPretty(jsonSettings,Serial); //lo escribe en el monitor serial
    }
    else
    {
        log("ERROR", "spiffsDate.hpp", "no se puede obteber informacion de las fechas");
        return false;
    }

    return true;
}
bool dataDateRead()
{
    log("INFO", "spiffsDate.hpp", "Entrando a leer los datos de la fecha y hora");
    DynamicJsonDocument jsonDataTime(capacityDate); // capacidad definida en globales.hpp (512B)
    // jsonSettings es el nombre del documento JSON
    // acontinuacion una variable de tipo File que se llama de la libreria SPIFFS que se llamará file
    File dataTime = SPIFFS.open("/dataTime.json", "r"); // modo lectura (r) con el path / que es en raiz y el nombre es settings con la extención json
    // mas info sobre SPIFFS https://www.diarioelectronicohoy.com/blog/funcionalidad-de-spiffs
    // se realizara una comparacion if la cual la su interpretación sería, si no se puede deserializar el file en el documento jsonStting
    // la cual sucede la primera vez por que no existe informacion en file
    if (deserializeJson(jsonDataTime, dataTime))
    {                                                                         // si no se puede asi funciona esa libreria (deserilizejson) osea si todo ok manda un 0
        dataDateReset();                                                      // se ejecuta esta funcion la cual formatea a los valores de fabrica en este caso a 0
        DeserializationError error = deserializeJson(jsonDataTime, dataTime); // arroja un error y es guardado como error de DeserializationError
        log("ERROR", "spiffsDate.hhp", "Falló la lectura de las FECHA, tomando valores por defecto");
        if (error)
        {                                                          // si hay un error nos indicara el error (por que fallo)
            log("ERROR", "spiffsDate.hhp", String(error.c_str())); // c_str() es un metodo que convierte a String
        }
        return false; // retornamos un valor de false de que no fue posible la deserializacion
    }
    else
    {
        // osea, que si fue posible deserializar de dataTime el archivo en la memoria ()-> jsonDataTime
        // asignamos lo que esta en jsonDataTime a las variable en memorias en tiempo de ejecucion.
        /********************************************************************************
         * Zona recuperacion de temperaturas de dataTime.json asignando nuevo valores         *
         * a las variables que se encuentran en las variables de vTemp                 *
         *******************************************************************************/

        tm.Day = jsonDataTime["dia"];
        tm.Month = jsonDataTime["mes"];
        tm.Year = CalendarYrToTm(jsonDataTime["anio"]);
        tm.Hour = jsonDataTime["hora"];
        tm.Minute = jsonDataTime["minuto"];
        RTC.write(tm);
    }
    dataTime.close(); // ya que todo se leera se cierra el archivo
    log("INFO", "spiffsDate.hpp", "Lectura de las Fechas OK.");
    Serial.flush();

    return true;
}