#include <Arduino.h>

int readLoudness()
{
	return analogRead(LOUDNESS_SENSOR);
}
