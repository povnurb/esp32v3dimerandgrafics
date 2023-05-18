#include <Arduino.h>

bool ioBlink = false;
unsigned long milOld;
int rndTemp = 0;    //temporizador rando
//--------------------------------------------------
// off un Led/Relay/Actuador
//--------------------------------------------------
void setOffSingle(int _pin){
    digitalWrite(_pin, LOW); //0, false, off
}
//--------------------------------------------------
// On un led/Relay/Actuador
//--------------------------------------------------
void setOnSingle(int _pin){
    digitalWrite(_pin, HIGH); //1, true, on
}
//-------------------------------------------------
//On/Off un Led/Relay/Actuador segun Estados
//-------------------------------------------------
void setOnOffSingle(int _pin, bool status){
    if(status){
        digitalWrite(_pin, HIGH);
    }else{
        digitalWrite(_pin, LOW);
    }
}
//----------------------------------------------------------------
//Simple blinking function - pestañeo para Alarmas tiempo variable
//----------------------------------------------------------------
void blinkSingle (int speed, int _pin){
    if ((milOld + speed) < millis()){
        milOld = millis();
        if(ioBlink == false){
            digitalWrite(_pin, HIGH);
            ioBlink = true;
        }else{
            digitalWrite(_pin, LOW);
            ioBlink = false;
        }
    }
}
// ----------------------------------------------------------------
// Parpadeo Asincrónico Tiempo on y Tiempo off tiempo varible
// ----------------------------------------------------------------
void blinkSingleAsy(int timeHigh, int timeLow, int _pin){
    if(ioBlink == false){
        if((milOld + timeHigh) < millis()){
            milOld = millis();
            digitalWrite(_pin, LOW);
            ioBlink = true;
        }
    }else{
        if((milOld + timeLow) < millis()){
            milOld = millis();
            digitalWrite(_pin, HIGH);
            ioBlink = false;
        }
    }
}
//------------------------------------------------------------------
// Blinking with randomised delay como para TX/RX de Datos en MQTT
//-------------------------------------------------------------------
void blinkRandomSingle(int minTime, int maxTime, int _pin){
    if((milOld + rndTemp) < millis()){
        milOld = millis();
        rndTemp = random (minTime, maxTime);
        if(ioBlink == false){
            digitalWrite(_pin, HIGH);
            ioBlink = true;
        }else{
            digitalWrite(_pin, LOW);
            ioBlink = false;
        }
    }
}