#include <OptoDebounce.h>

OptoDebounce sensor = OptoDebounce( 2 );

void setup() {
}

void loop() {
 static unsigned long last = 0;

 // Report the current every 1000mSeconds, eveyr second.
 if (millis() - last > 1000) {
  last = millis();
  Serial.printf("State of the opto is %s\n", sensor.state() ? "on" : "off");
 }
}

