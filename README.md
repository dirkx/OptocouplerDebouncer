# Optocoupler Debouncer

It is common to use an optocoupler to detect if there is AC power.

However if you directly connect these to your Arduino/ESP32 board, without some RC damping, - you will typically get a 100 Hz pulse.

This library provides a simple way to get a 'on/off' asynchronous call

## Electronics

Typical electronics (using a PC814 that has two LEDs, saving us having to add a diode):


       o---------+---+        ------
                 |   |     +-| 220k |---+ IO arduino, pull-up
    100-400V    --- ---    |  ------    
       AC       / \ \ /   K
                --- ---    |  ------    
                 |   |     +-| 220k |---+ 0V arduino
       o---------+---+       ------


## Software

Simple scanning in the loop:

	#include <OptoDebounce.h>
	OptoDebounce sensor = OptoDebounce( 2 );

	void setup() {};

	void loop() {
	 	...
  		Serial.printf(sensor.state());
    }

Or getting a call whenever the power goes on or off:

	#include <OptoDebounce.h>

	OptoDebounce sensor = OptoDebounce( 2 );

	void setup() {
		 sensor.onOptoOn([]() {
	   Serial.println("Its on !");
	 });

	 sensor.onOptoOff([]() {
   		Serial.println("Its off!t");
	 });
	}

	void loop() {
		...
	}

Using a callback whenever things change:

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
 		 }});
	}

	void loop() {
	}

