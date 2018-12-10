#include <OptoDebounce.h>
#include "Arduino.h"

class OptoDebounce;

// Glue logic (ESP32's ticker does not yet have TArgs). See
// https://github.com/esp8266/Arduino/pull/5030 for what 
// is needed.
//
static void _sample(uint32_t arg) {
	OptoDebounce * c = (OptoDebounce*)arg;
	c->sample();
};

OptoDebounce::OptoDebounce(uint8_t pin, uint16_t graceMs, uint16_t sampleFrequency ) 
	: _pin(pin),  _msTrigger(graceMs), _hz(sampleFrequency)
{
     _ticker = new Ticker();
     _ticker->attach_ms(1000 / _hz /* mSecond */, _sample,(uint32_t) this);
     pinMode(_pin, INPUT_PULLUP); // analog input.
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

// Keep this routine as short and concise as posible; as we run on
// an interrupt/timer.
//
void OptoDebounce::sample() {
     _sum += digitalRead(_pin) ? 1 : 0;
     _n ++;

     if (_n < _msTrigger *  _hz / 1000)
	return;

     bool nstate = (_sum < _n / 2) ? false : true;
     _n = 0;
     _sum = 0;

     if (nstate == _state)
	return;

     _state = nstate;
     if (_callback)
       _callback(nstate ? ON : OFF);

     if (nstate && _callbackOn)
       _callbackOn();

     if (!nstate && _callbackOff)
       _callbackOff();
};

bool OptoDebounce::state() {
     return _state;
};
