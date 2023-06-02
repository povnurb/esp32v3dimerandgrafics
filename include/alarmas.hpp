/**
 * Tambien corresponde a la configuracion especial
*/
//declaracion de funciones a usar en este archivo
float TempCPUValue();
float Temperatura();
float Humedad();
float tempMin();
float tempMax();
void IRAM_ATTR activarAlarmas();
// ---------------------------------------------------
// Zona de alarmas
// ---------------------------------------------------
uint8_t ALRM_PIN1=5, ALRM_PIN2=14, ALRM_PIN3=16, ALRM_PIN4=17, ALRM_PIN5=18, ALRM_PIN6=26, ALRM_PIN7=27;
bool    ALRM_STATUS1,ALRM_STATUS2,ALRM_STATUS3,ALRM_STATUS4,ALRM_STATUS5,ALRM_STATUS6,ALRM_STATUS7;
bool    ALRM_LOGIC1,ALRM_LOGIC2,ALRM_LOGIC3,ALRM_LOGIC4,ALRM_LOGIC5,ALRM_LOGIC6,ALRM_LOGIC7;
String  ALRM_NAME1, ALRM_NAME2, ALRM_NAME3, ALRM_NAME4, ALRM_NAME5, ALRM_NAME6, ALRM_NAME7;
String  ALRM_TON1, ALRM_TON2, ALRM_TON3, ALRM_TON4, ALRM_TON5, ALRM_TON6, ALRM_TON7;
String  ALRM_TOFF1, ALRM_TOFF2, ALRM_TOFF3, ALRM_TOFF4, ALRM_TOFF5, ALRM_TOFF6, ALRM_TOFF7;
int     ALRM_CONT1, ALRM_CONT2, ALRM_CONT3, ALRM_CONT4, ALRM_CONT5, ALRM_CONT6, ALRM_CONT7; //registra el valor del contador
int cont[7] = {ALRM_CONT1, ALRM_CONT2, ALRM_CONT3, ALRM_CONT4,ALRM_CONT5, ALRM_CONT6, ALRM_CONT7};//lleva la contavilizacion en un for
bool cambiar[7] = {false,false,false,false,false,false,false};//ayuda al contador a idicar si va a cambiar de numero 
bool Activar = false; //envia por mqtt que se hace una prueba de todas las alarmas en este caso 7 alarmas

// -------------------------------------------------------------------
struct Alarma{
    uint8_t PIN; //5,14,16,17,18,26,27
    String NAME;
    bool LOGICA;
    int MODE;
};

// -------------------------------------------------------------------
// DEFINICION DEL PIN (alarma1)
// PIN: GPIO5 | LOGICA: 0 | MODE: INPUT
// -------------------------------------------------------------------
Alarma alarma1;
Alarma alarma2;
Alarma alarma3;
Alarma alarma4;
Alarma alarma5;
Alarma alarma6;
Alarma alarma7;
// -------------------------------------------------------------
// SETUP DEL PIN ALARMAS
// -------------------------------------------------------------
void setupPinAlarma1(){

    if (!ALRM_LOGIC1){
        alarma1 = {ALRM_PIN1,ALRM_NAME1,ALRM_LOGIC1,INPUT_PULLUP};
        pinMode(alarma1.PIN,alarma1.MODE);
        if (digitalRead(alarma1.PIN)){
            ALRM_STATUS1=true;
        }else
        {
            ALRM_STATUS1=false;
        }
    }else{
        alarma1 = {ALRM_PIN1,ALRM_NAME1,ALRM_LOGIC1,INPUT_PULLDOWN};
        pinMode(alarma1.PIN,alarma1.MODE);
        if (digitalRead(alarma1.PIN)){
            ALRM_STATUS1=false;
        }else
        {
            ALRM_STATUS1=true;
        }
    }
}

void setupPinAlarma2(){
    if (!ALRM_LOGIC2){
        alarma2 = {ALRM_PIN2,ALRM_NAME2,ALRM_LOGIC2,INPUT_PULLUP};
        pinMode(alarma2.PIN,alarma2.MODE);
        if (digitalRead(alarma2.PIN)){
            ALRM_STATUS2=true;
        }else
        {
            ALRM_STATUS2=false;
        }
    }else{
        alarma2 = {ALRM_PIN2,ALRM_NAME2,ALRM_LOGIC2,INPUT_PULLDOWN};
        pinMode(alarma2.PIN,alarma2.MODE);
        if (digitalRead(alarma2.PIN)){
            ALRM_STATUS2=false;
        }else
        {
            ALRM_STATUS2=true;
        }
    }
}

void setupPinAlarma3(){
    if (!ALRM_LOGIC3){
        alarma3 = {ALRM_PIN3,ALRM_NAME3,ALRM_LOGIC3,INPUT_PULLUP};
        pinMode(alarma3.PIN,alarma3.MODE);
        if (digitalRead(alarma3.PIN)){
            ALRM_STATUS3=true;
        }else
        {
            ALRM_STATUS3=false;
        }
    }else{
        alarma3 = {ALRM_PIN3,ALRM_NAME3,ALRM_LOGIC3,INPUT_PULLDOWN};
        pinMode(alarma3.PIN,alarma3.MODE);
        if (digitalRead(alarma3.PIN)){
            ALRM_STATUS3=false;
        }else
        {
            ALRM_STATUS3=true;
        }
    }
}

void setupPinAlarma4(){
    if (!ALRM_LOGIC4){
        alarma4 = {ALRM_PIN4,ALRM_NAME4,ALRM_LOGIC4,INPUT_PULLUP};
        pinMode(alarma4.PIN,alarma4.MODE);
        if (digitalRead(alarma4.PIN)){
            ALRM_STATUS4=true;
        }else
        {
            ALRM_STATUS4=false;
        }
    }else{
        alarma4 = {ALRM_PIN4,ALRM_NAME4,ALRM_LOGIC4,INPUT_PULLDOWN};
        pinMode(alarma4.PIN,alarma4.MODE);
        if (digitalRead(alarma4.PIN)){
            ALRM_STATUS4=false;
        }else
        {
            ALRM_STATUS4=true;
        }
    }
}

void setupPinAlarma5(){
    if (!ALRM_LOGIC5){
        alarma5 = {ALRM_PIN5,ALRM_NAME5,ALRM_LOGIC5,INPUT_PULLUP};
        pinMode(alarma5.PIN,alarma5.MODE);
        if (digitalRead(alarma5.PIN)){
            ALRM_STATUS5=true;
        }else
        {
            ALRM_STATUS5=false;
        }
    }else{
        alarma5 = {ALRM_PIN5,ALRM_NAME5,ALRM_LOGIC5,INPUT_PULLDOWN};
        pinMode(alarma5.PIN,alarma5.MODE);
        if (digitalRead(alarma5.PIN)){
            ALRM_STATUS5=false;
        }else
        {
            ALRM_STATUS5=true;
        }
    }
}

void setupPinAlarma6(){  
    if (!ALRM_LOGIC6){
        alarma6 = {ALRM_PIN6,ALRM_NAME6,ALRM_LOGIC6,INPUT_PULLUP};
        pinMode(alarma6.PIN,alarma6.MODE);
        if (digitalRead(alarma6.PIN)){
            ALRM_STATUS6=true;
        }else
        {
            ALRM_STATUS6=false;
        }
    }else{
        alarma6 = {ALRM_PIN6,ALRM_NAME6,ALRM_LOGIC6,INPUT_PULLDOWN};
        pinMode(alarma6.PIN,alarma6.MODE);
        if (digitalRead(alarma6.PIN)){
            ALRM_STATUS6=false;
        }else
        {
            ALRM_STATUS6=true;
        }
    }
}

void setupPinAlarma7(){
    if (!ALRM_LOGIC7){
        alarma7 = {ALRM_PIN7,ALRM_NAME7,ALRM_LOGIC7,INPUT_PULLUP};
        pinMode(alarma7.PIN,alarma7.MODE);
        if (digitalRead(alarma7.PIN)){
            ALRM_STATUS7=true;
        }else
        {
            ALRM_STATUS7=false;
        }
    }else{
        alarma7 = {ALRM_PIN7,ALRM_NAME7,ALRM_LOGIC7,INPUT_PULLDOWN};
        pinMode(alarma7.PIN,alarma7.MODE);
        if (digitalRead(alarma7.PIN)){
            ALRM_STATUS7=false;
        }else
        {
            ALRM_STATUS7=true;
        }
    }
}

void setupAlarmas(){
    setupPinAlarma1();
    setupPinAlarma2();
    setupPinAlarma3();
    setupPinAlarma4();
    setupPinAlarma5();
    setupPinAlarma6();
    setupPinAlarma7();
}
// -------------------------------------------------------------
// Envio de las ALARMAS por MQTT o WS
// -------------------------------------------------------------

String OnOffAlarmas(){
    String response = "";
	DynamicJsonDocument jsonDoc(2500); //2500
    jsonDoc["type"] = "alarmas";
    if (!alarma1.LOGICA){
        if (digitalRead(alarma1.PIN)){
            jsonDoc["ALRM_STATUS1"] = true;
            ALRM_STATUS1=true;
        }else
        {
           jsonDoc["ALRM_STATUS1"] = false;
           ALRM_STATUS1=false;
        }
    }else{
        if (digitalRead(alarma1.PIN)){
            jsonDoc["ALRM_STATUS1"] = false;
            ALRM_STATUS1=false;
        }else
        {
           jsonDoc["ALRM_STATUS1"] = true;
           ALRM_STATUS1=true;
        }
    }
    if (!alarma2.LOGICA){
        if (digitalRead(alarma2.PIN)){
            jsonDoc["ALRM_STATUS2"] = true;
            ALRM_STATUS2=true;
        }else
        {
           jsonDoc["ALRM_STATUS2"] = false;
           ALRM_STATUS2=false;
        }
    }else{
        if (digitalRead(alarma2.PIN)){
            jsonDoc["ALRM_STATUS2"] = false;
            ALRM_STATUS2=false;
        }else
        {
           jsonDoc["ALRM_STATUS2"] = true;
           ALRM_STATUS2=true;
        }
    }
    if (!alarma3.LOGICA){
        if (digitalRead(alarma3.PIN)){
            jsonDoc["ALRM_STATUS3"] = true;
            ALRM_STATUS3=true;
        }else
        {
           jsonDoc["ALRM_STATUS3"] = false;
           ALRM_STATUS3=false;
        }
    }else{
        if (digitalRead(alarma3.PIN)){
            jsonDoc["ALRM_STATUS3"] = false;
            ALRM_STATUS3=false;
        }else
        {
           jsonDoc["ALRM_STATUS3"] = true;
           ALRM_STATUS3=true;
        }
    }
    if (!alarma4.LOGICA){
        if (digitalRead(alarma4.PIN)){
            jsonDoc["ALRM_STATUS4"] = true;
            ALRM_STATUS4=true;
        }else
        {
           jsonDoc["ALRM_STATUS4"] = false;
           ALRM_STATUS5=false;
        }
    }else{
        if (digitalRead(alarma4.PIN)){
            jsonDoc["ALRM_STATUS4"] = false;
            ALRM_STATUS4=false;
        }else
        {
           jsonDoc["ALRM_STATUS4"] = true;
           ALRM_STATUS4=true;
        }
    }
    if (!alarma5.LOGICA){
        if (digitalRead(alarma5.PIN)){
            jsonDoc["ALRM_STATUS5"] = true;
            ALRM_STATUS5=true;
        }else
        {
           jsonDoc["ALRM_STATUS5"] = false;
           ALRM_STATUS5=false;
        }
    }else{
        if (digitalRead(alarma5.PIN)){
            jsonDoc["ALRM_STATUS5"] = false;
            ALRM_STATUS5=false;
        }else
        {
           jsonDoc["ALRM_STATUS5"] = true;
           ALRM_STATUS5=true;
        }
    }
    if (!alarma6.LOGICA){
        if (digitalRead(alarma6.PIN)){
            jsonDoc["ALRM_STATUS6"] = true;
            ALRM_STATUS6=true;
        }else
        {
           jsonDoc["ALRM_STATUS6"] = false;
           ALRM_STATUS6=false;
        }
    }else{
        if (digitalRead(alarma6.PIN)){
            jsonDoc["ALRM_STATUS6"] = false;
            ALRM_STATUS6=false;
        }else
        {
           jsonDoc["ALRM_STATUS6"] = true;
           ALRM_STATUS6=true;
        }
    }
    if (!alarma7.LOGICA){
        if (digitalRead(alarma7.PIN)){
            jsonDoc["ALRM_STATUS7"] = true;
            ALRM_STATUS7=true;
        }else
        {
           jsonDoc["ALRM_STATUS7"] = false;
           ALRM_STATUS7=false;
        }
    }else{
        if (digitalRead(alarma7.PIN)){
            jsonDoc["ALRM_STATUS7"] = false;
            ALRM_STATUS7=false;
        }else
        {
           jsonDoc["ALRM_STATUS7"] = true;
           ALRM_STATUS7=true;
        }
    }
    
    jsonDoc["ALRM_CONT1"]=ALRM_CONT1;jsonDoc["ALRM_CONT5"]=ALRM_CONT5;
    jsonDoc["ALRM_CONT2"]=ALRM_CONT2;jsonDoc["ALRM_CONT6"]=ALRM_CONT6;
    jsonDoc["ALRM_CONT3"]=ALRM_CONT3;jsonDoc["ALRM_CONT7"]=ALRM_CONT7;
    jsonDoc["ALRM_CONT4"]=ALRM_CONT4;
    jsonDoc["ALRM_TON1"]=ALRM_TON1;jsonDoc["ALRM_TON2"]=ALRM_TON2;
    jsonDoc["ALRM_TON3"]=ALRM_TON3;jsonDoc["ALRM_TON4"]=ALRM_TON4;
    jsonDoc["ALRM_TON5"]=ALRM_TON5;jsonDoc["ALRM_TON6"]=ALRM_TON6;
    jsonDoc["ALRM_TON7"]=ALRM_TON7;
    jsonDoc["ALRM_TOFF1"]=ALRM_TOFF1;jsonDoc["ALRM_TOFF2"]=ALRM_TOFF2;
    jsonDoc["ALRM_TOFF3"]=ALRM_TOFF3;jsonDoc["ALRM_TOFF4"]=ALRM_TOFF4;
    jsonDoc["ALRM_TOFF5"]=ALRM_TOFF5;jsonDoc["ALRM_TOFF6"]=ALRM_TOFF6;
    jsonDoc["ALRM_TOFF7"]=ALRM_TOFF7;
    jsonDoc["dim"]=String(dim);
    jsonDoc["t_cpu"] = String(round(TempCPUValue()),1);
    jsonDoc["tC"] = String(round(Temperatura()),1);
    jsonDoc["hum"] = String(round(Humedad()),1); //tempMin() //tempMax()
    jsonDoc["tmin"] = String(round(tempMin()),1);
    jsonDoc["tmax"] = String(round(tempMax()),1);
    serializeJson(jsonDoc, response);
	return response;
}
// ----------------------------------------------------------------------------------
// Activar Alarmas
// ----------------------------------------------------------------------------------
void activarAlarma(){ //para provar despues en casa

int pines[7] = {ALRM_PIN1,ALRM_PIN2,ALRM_PIN3,ALRM_PIN4,ALRM_PIN5,ALRM_PIN6,ALRM_PIN7};
bool logicas[7] = {ALRM_LOGIC1,ALRM_LOGIC2,ALRM_LOGIC3,ALRM_LOGIC4,ALRM_LOGIC5,ALRM_LOGIC6,ALRM_LOGIC7};
    if(digitalRead(15)){
        prueba = true;//prueba de alarmas envio por mqtt
        //digitalWrite(32,HIGH);
        normalizar=false;
        for (int i=0; i < 7; i++){
            if(!logicas[i]){
                pinMode(pines[i],OUTPUT); //activa las alarmas de manera remota
                digitalWrite(pines[i],LOW);
                cont[i]=0;
                
            }else{
                pinMode(pines[i],INPUT_PULLUP);
                digitalWrite(pines[i],HIGH);//tenia 1
                cont[i]=0;
            }
            
        }
    }else if(!digitalRead(15)&&!normalizar){
        normalizar=true;
        setupAlarmas();
        prueba = false;//se quita la prueba de alarmas por mqtt
        for (int i=0; i < 7; i++){ //por que antes eran 8 alarmas ahora son 7
            cont[i]=0;
        }
    }
}
//------------------------------------------------------------------------------------------------------------
// Setup pin Alarmas por interrupcion boton en sitio
// ----------------------------------------------------------------------------------------------------------
void setupPinActivarAlarmas(){
    pinMode(BOTON2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BOTON2), activarAlarmas, FALLING);  // Flanco de bajada
}
void IRAM_ATTR activarAlarmas(){
    if(millis() - tiempoDeInterrupcion > tiempoDeRebote){
    Serial.println("Interrupción34");
    if(togle){
        Serial.println("Activando Alarmas");
        digitalWrite(15, HIGH); //cuando se activa el pin15 se activan las alarmas
    }else{
        Serial.println("Desactivando Alarmas");
        digitalWrite(15, LOW);
        }
    tiempoDeInterrupcion = millis();
    togle ^= true;
    } 
}
// nueva funcion estatus
void statusAlarmVariables(){//modifica el estado de las variables
    if (!alarma1.LOGICA){
        if (digitalRead(alarma1.PIN)){
            ALRM_STATUS1=true;
        }else
        {
           ALRM_STATUS1=false;
        }
    }else{
        if (digitalRead(alarma1.PIN)){
            ALRM_STATUS1=false;
        }else
        {
           ALRM_STATUS1=true;
        }
    }
    if (!alarma2.LOGICA){
        if (digitalRead(alarma2.PIN)){
            ALRM_STATUS2=true;
        }else
        {
           ALRM_STATUS2=false;
        }
    }else{
        if (digitalRead(alarma2.PIN)){
            ALRM_STATUS2=false;
        }else
        {
           ALRM_STATUS2=true;
        }
    }
    if (!alarma3.LOGICA){
        if (digitalRead(alarma3.PIN)){
            ALRM_STATUS3=true;
        }else
        {
           ALRM_STATUS3=false;
        }
    }else{
        if (digitalRead(alarma3.PIN)){
            ALRM_STATUS3=false;
        }else
        {
           ALRM_STATUS3=true;
        }
    }
    if (!alarma4.LOGICA){
        if (digitalRead(alarma4.PIN)){
            ALRM_STATUS4=true;
        }else
        {
           ALRM_STATUS5=false;
        }
    }else{
        if (digitalRead(alarma4.PIN)){
            ALRM_STATUS4=false;
        }else
        {
           ALRM_STATUS4=true;
        }
    }
    if (!alarma5.LOGICA){
        if (digitalRead(alarma5.PIN)){
            ALRM_STATUS5=true;
        }else
        {
           ALRM_STATUS5=false;
        }
    }else{
        if (digitalRead(alarma5.PIN)){
            ALRM_STATUS5=false;
        }else
        {
           ALRM_STATUS5=true;
        }
    }
    if (!alarma6.LOGICA){
        if (digitalRead(alarma6.PIN)){
            ALRM_STATUS6=true;
        }else
        {
           ALRM_STATUS6=false;
        }
    }else{
        if (digitalRead(alarma6.PIN)){
            ALRM_STATUS6=false;
        }else
        {
           ALRM_STATUS6=true;
        }
    }
    if (!alarma7.LOGICA){
        if (digitalRead(alarma7.PIN)){
            ALRM_STATUS7=true;
        }else
        {
           ALRM_STATUS7=false;
        }
    }else{
        if (digitalRead(alarma7.PIN)){
            ALRM_STATUS7=false;
        }else
        {
           ALRM_STATUS7=true;
        }
    }
}