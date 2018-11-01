#include "DelayedEvents.h"
//--------------------------------------------------------------------------------------------------------------------------------------
DelayedEventsClass DelayedEvents;
//--------------------------------------------------------------------------------------------------------------------------------------
DelayedEventsClass::DelayedEventsClass()
{
  
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DelayedEventsClass::raise(uint32_t raiseDelay,DelayedEventHandler handler, void* param)
{
  DelayedEventData rec;
  rec.timer = millis();
  rec.duration = raiseDelay;
  rec.handler = handler;
  rec.param = param;
  
  signals.push_back(rec);
}
//--------------------------------------------------------------------------------------------------------------------------------------
void DelayedEventsClass::update()
{
    for(size_t i=0;i<signals.size();)
    {
        if(millis() - signals[i].timer > signals[i].duration)
        {
          // сигнал сработал
          DelayedEventHandler handler = signals[i].handler;
          void* param = signals[i].param;

          // сначала мы убираем его из очереди
          for(size_t j=i+1; j< signals.size(); j++)
          {
            signals[j-1] = signals[j];
          }

          signals.pop();

          // и только потом вызываем обработчик сигнала
          if(handler)
            handler(param);
            
        } // if
        else
        {
          i++;
        }
    } // for
}
//--------------------------------------------------------------------------------------------------------------------------------------

