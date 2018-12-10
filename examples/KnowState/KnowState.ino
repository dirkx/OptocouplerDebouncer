#include <OptoDebounce.h>

OptoDebounce sensor = OptoDebounce( 2 );

void setup() {

 sensor.onOptoChange([](OptoDebounce::state_t state) {
   switch (state) {
     case OptoDebounce::ON:
       Serial.println("Change to on");
       break;
     case OptoDebounce::OFF:
       Serial.println("Change to off");
       break;
     case OptoDebounce::UNKNOWN:
       Serial.println("Unknown state");
       break;
   }
 });

}

void loop() {
}

