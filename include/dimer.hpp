// crear una tarea para el valor de lectura del potenciometro de cada 100 milisegundos

// changing the LED brightness with PWM
// ledcWrite(0, dutyCycle);
// ledcWrite(1, dutyCycle);

// realiza la lectura de potenciometro para actualizar la salida del Dimmer
// outVal = map(analogRead(INPOT), 1, 4096, 100, 0); // analogRead(analog_pin),
// min_analog, max_analog, 100%, 0%);
// dimmer.setPower(outVal); // name.setPower(0%-100%)

void serialDimer()
{
    dutyCycle = map(analogRead(INPOT), 1, 4096, 255, 0); // imprime de 1 a 255
    Serial.println(dutyCycle);
    // realiza la lectura de potenciometro para actualizar la salida del Dimmer
    outVal = map(analogRead(INPOT), 1, 4096, 100, 0); // analogRead(analog_pin),
    // min_analog, max_analog, 100%, 0%); // es el ciclo de trabajo
    dimmerReal.setPower(outVal); // name.setPower(0%-100%)
}