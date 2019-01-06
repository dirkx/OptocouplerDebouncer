#include <OptoDebounce.h>
#include "Arduino.h"
#include <FunctionalInterrupt.h>

// #include <portmacro.h>

class OptoDebounce;

// Glue logic (ESP32's ticker does not yet have TArgs). See
// https://github.com/esp8266/Arduino/pull/5030 for what 
// is needed. Likewise in case we use interupts.
//
#ifdef OB_POLLING
static void _sample(OptoDebounce * c) {
	c->sample();
};
#else

#ifdef ESP32
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
#endif

#ifdef ESP32
void IRAM_ATTR _handleInterrupt(void * arg) {
#else
void ICACHE_RAM_ATTR _handleInterrupt(void * arg) {
#endif
        OptoDebounce * c = (OptoDebounce *)arg;
#ifdef ESP32
        portENTER_CRITICAL_ISR(&mux); // prolly a bit over the top.
#else 
	noInterrupts();
#endif
        c->_interruptCounter++;
#ifdef ESP32
        portEXIT_CRITICAL_ISR(&mux);
#else
	interrupts();
#endif
}
#endif

OptoDebounce::OptoDebounce(uint8_t pin, uint16_t graceMs, uint16_t sampleFrequency ) 
	: _pin(pin),  _msTrigger(graceMs), _hz(sampleFrequency)
{
     pinMode(_pin, INPUT_PULLUP);
#ifdef OB_POLLING
     assert(_hz >= 500);

     _ticker = new Ticker();
     _ticker->attach_ms(1000 / _hz /* mSecond */, _sample,this);
#else
#ifdef ESP32
     attachInterruptArg(digitalPinToInterrupt(_pin), &_handleInterrupt, this, RISING);
#else
     attachInterrupt(digitalPinToInterrupt(_pin), std::bind(&OptoDebounce::_handleInt,this), RISING);
#endif
#endif
     _sum = 0;
}

#ifdef ESP32
void OptoDebounce::_handleInt() {
#else
void ICACHE_RAM_ATTR OptoDebounce::_handleInt() {
#endif
#ifdef ESP32
        portENTER_CRITICAL_ISR(&mux); // prolly a bit over the top.
#else 
	noInterrupts();
#endif
        _interruptCounter++;
#ifdef ESP32
        portEXIT_CRITICAL_ISR(&mux);
#else
	interrupts();
#endif
}

OptoDebounce::~OptoDebounce() {
     if (_ticker)
	_ticker->detach();
     _ticker = NULL;
}

OptoDebounce& OptoDebounce::onOptoOn(THandlerFunction_Callback fn) {
     _callbackOn = fn;
     return *this;
};

OptoDebounce& OptoDebounce::onOptoOff(THandlerFunction_Callback fn) {
     _callbackOff = fn;
     return *this;
};

OptoDebounce& OptoDebounce::onOptoChange(THandlerFunction_CallbackWithState fn) {
     _callback = fn;
     return *this;
};

#ifdef OB_POLLING
void OptoDebounce::_sample() {
     int m = 100; // _msTrigger *  _hz / 1000;

     _sum = (_sum * m + digitalRead(_pin)) / (m + 1);
     bool newState = (_sum > 0.8 ) ? true : false;

     if (newState == _state)
	return;

     _process(newState);
}
#endif

void OptoDebounce::loop() {
#if 1
     // should see around 100 passes through null per
     // second if there is an AC voltage, otherwhise
     // permanentish high.
     // 
     if (millis() - last < 1000) 
	return;
     last = millis();

     unsigned int c = _interruptCounter;
     _interruptCounter = 0;

     // we're seeing 20 (of the about 50) expected pass
     // through 0 interrupts. So there is prolly power.
     //
     bool newState = (c > 5);

     if (newState == _state)
	return;
#else
     if (millis() - last < 250) 
        return;
     last = millis();
     int c = 0;
     for(int i = 0; i < 40; i++) {
		c += digitalRead(_pin);
		delay(1);
     };
     // Pulled low most of the time ?
     bool newState = c < 20;
#endif

     _state = newState;
     if (_callback)
       _callback(newState ? ON : OFF);

     if (newState && _callbackOn)
       _callbackOn();

     if (!newState && _callbackOff)
       _callbackOff();
};

bool OptoDebounce::state() {
     return _state;
};

double OptoDebounce::s() {
     return _sum;
};
