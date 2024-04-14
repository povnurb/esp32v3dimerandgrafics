// crear una tarea para el valor de lectura del potenciometro de cada 100 milisegundos
// https://www.youtube.com/watch?v=bvKoG7kB8Z8
// este circuito cuando la señal pasa por cero genera un pulso
// changing the LED brightness with PWM configuracion del PWM en el setup()
// ledcWrite(0, dutyCycle);//el 0 es un canal
// ledcWrite(1, dutyCycle);//el 1 es otro canal

void serialDimer()
{
    dutyCycle = map(analogRead(INPOT), 1, 4096, 1, 16); // imprime de 1 a 15
    // realiza la lectura de potenciometro para actualizar la salida del Dimmer
    outVal = map(analogRead(INPOT), 1, 4096, 0, 101); // analogRead(analog_pin),
    // min_analog, max_analog, 100%, 0%); // es el ciclo de trabajo
    dimmerReal.setPower(outVal); // name.setPower(0%-100%)
    muestraCadamin = dutyCycle;
    // if (muestraDimerOLED != dutyCycle)
    // {
    //     // meter un tiempo de muestra en pantalla OLED
    //     timeOled = millis();
    //     do
    //     {
    //         muestraDimerOLED = dutyCycle;
    //         // crear una tarea de 10 segundos mostrando el valor en el display
    //         OLED.clearDisplay();
    //         OLED.setTextSize(1);
    //         OLED.setTextColor(WHITE);
    //         OLED.setCursor(0, 0);
    //         OLED.println("Muestras cada:");
    //         OLED.setTextSize(3);
    //         OLED.print(" ");
    //         OLED.print(dutyCycle);
    //         OLED.println(" Min.");
    //         OLED.display();
    //         dim = outVal;
    //     } while (millis() - timeOled >= timeMuestraOLED);
    // }
    // else
    // {
    //     mostrar();
    // }
}
