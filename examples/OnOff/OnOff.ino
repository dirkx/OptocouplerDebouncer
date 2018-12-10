#include <OptoDebounce.h>

OptoDebounce sensor = OptoDebounce( 2 );

void setup() {

 // Or alternatively - listen to just what you need
 // by getting a On or Off callback.
 //
 sensor.onOptoOn([]() {
   Serial.println("Its on !");
 });

 sensor.onOptoOff([]() {
   Serial.println("Its off!t");
 });
}

void loop() {
 // put your main code here, to run repeatedly:
}

