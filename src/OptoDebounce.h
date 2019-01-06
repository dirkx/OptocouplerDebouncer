#ifndef _H_OPTO_DEBOUNCE
#define _H_OPTO_DEBOUNCE

#ifndef FAXE_TEST
#include <Ticker.h>
#endif
  
#include <algorithm>
#include <list>
#include <functional>

class OptoDebounce {
 public:
   OptoDebounce(uint8_t pin, uint16_t msTrigger = 250 /* mSec */, uint16_t sampleFrequency = 20 /* Hz */ );
   ~OptoDebounce();

   typedef std::function<void()> THandlerFunction_Callback;

   OptoDebounce& onOptoOn(THandlerFunction_Callback fn);
   OptoDebounce& onOptoOff(THandlerFunction_Callback fn);

   typedef enum { OFF, ON, UNKNOWN } state_t;
   typedef std::function<void(state_t state)> THandlerFunction_CallbackWithState;
   OptoDebounce& onOptoChange(THandlerFunction_CallbackWithState fn);

   bool state();
   void sample();
   void loop();
   double s();

   uint16_t _interruptCounter;
   void _handleInt();
   void _process(bool newState);

 private:
   Ticker * _ticker;

   uint8_t _pin;
   uint16_t _msTrigger;
   uint16_t _hz;
   float _sum;
   bool _state;
   unsigned long last;

   THandlerFunction_Callback _callbackOn,  _callbackOff;
   THandlerFunction_CallbackWithState _callback;
};
#endif
