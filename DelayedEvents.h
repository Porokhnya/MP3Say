#pragma once
//--------------------------------------------------------------------------------------------------------------------------------------
#include <Arduino.h>
#include "TinyVector.h"
//--------------------------------------------------------------------------------------------------------------------------------------
typedef void (*DelayedEventHandler)(void* param);
//--------------------------------------------------------------------------------------------------------------------------------------
typedef struct
{
  uint32_t timer;
  uint32_t duration;
  void* param;
  DelayedEventHandler handler;
  
} DelayedEventData;
//--------------------------------------------------------------------------------------------------------------------------------------
typedef Vector<DelayedEventData> DelayedEventsList;
//--------------------------------------------------------------------------------------------------------------------------------------
class DelayedEventsClass
{
  public:
    DelayedEventsClass();

    void update();
    void raise(uint32_t raiseDelayMilliseconds,DelayedEventHandler handler, void* param);
    bool empty() { return signals.size() < 1; }

  private:

    DelayedEventsList signals;
  
};
//--------------------------------------------------------------------------------------------------------------------------------------
extern DelayedEventsClass DelayedEvents;
//--------------------------------------------------------------------------------------------------------------------------------------

