#include <Arduino.h>

/*
* Temp sensor:
* https://www.seeedstudio.com/Grove-Temperature-Sensor-p-774.html
*
** NCP18WF104F03RC
*/

float getTemperature(int pin) {
    int a = analogRead(pin);

    const int B=4250;                 // B value of the thermistor

    float R = 1023.0/((float)a)-1.0;

    R = 100000.0*R;

    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;

    return temperature;
}
