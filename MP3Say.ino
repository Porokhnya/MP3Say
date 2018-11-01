//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки работы с DFPlayer
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define DFPLAYER_SERIAL_SPEED 9600 // скорость работы с плеером DFPlayer mini, бод
#define DFPLAYER_SERIAL_RX_PIN A1 // пин RX для SoftwareSerial
#define DFPLAYER_SERIAL_TX_PIN A2 // пин TX для SoftwareSerial
#define DFPLAYER_INIT_DELAY 1000 // сколько миллисекунд ждать в setup инициализации плеера
#define DFPLAYER_INITIAL_VOLUME 30 // начальная громкость плеера (0-30)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// настройки Serial
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#define SERIAL_SPEED 57600 // скорость работы с монитором порта
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SoftwareSerial.h>
#include  "DFPlayer_Mini_Mp3.h"
#include  "DelayedEvents.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
SoftwareSerial playerSerial(DFPLAYER_SERIAL_RX_PIN,DFPLAYER_SERIAL_TX_PIN);
String receiveBuffer;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void playHandler(void* param)
{
  uint16_t melody = (uint16_t) param;
  Serial.print(F("Play delayed: "));
  Serial.println(melody);
  mp3_stop();
  mp3_play(melody);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void playMelodyDelayed(uint16_t melody, uint32_t delayMs)
{
  DelayedEvents.raise(delayMs, playHandler,(void*)melody);
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
uint32_t getMelodyDelay(uint16_t num) // возвращает задержку для мелодии
{
  switch(num)
  {
    case 20:
    case 40:
    case 50:
      return 900;

   case 60:
   case 70:
    return 950;
    
   case 80:
   case 90:
    return 1000;

   case 100:
   case 300:
    return 700;

   case 200:
   case 500:
   case 600:
    return 900;

   case 400:
   case 700:
   case 800:
   case 900:
    return 1000;
  }
  // пока тупо возвращаем секунду
  return 800; 
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void sayNumber(const String& str)
{
    Serial.print(F("You entered: "));
    Serial.println(str);

    uint32_t currentRaiseDelay = 0; // текущая задержка для мелодии

    uint16_t num = str.toInt();

    if(num < 0)
      return;

   if(num <= 20) // от 0 до 20 - отдельные файлы на числа
   {
      currentRaiseDelay += getMelodyDelay(num);
      playMelodyDelayed(num,0);
      return;
   }

   // тут уже идут составные числа, у нас максимум голосовых файлов - на тысячу.
   // просто втупую разбиваем на составные части

   uint16_t ones = num % 10;
   uint16_t tens = num/10 % 10;
   uint16_t hundreds = num/100 % 10;
   uint16_t thousands = num/1000 % 10;

   Serial.print(F("thousands: "));
   Serial.println(thousands);

   Serial.print(F("hundreds: "));
   Serial.println(hundreds);

   Serial.print(F("tens: "));
   Serial.println(tens);

   Serial.print(F("ones: "));
   Serial.println(ones);

   // теперь играем без склонения
   if(thousands > 0)
   {
      if(thousands > 1) // если две тысячи и больше - надо сперва произнести число, потом слово "тысяча"
      {
         Serial.println(String(F("PLAY THOUSANDS: ")) + thousands);

         // тут случай с "две тысячи", а не "два тысячи". файл со словов "две" имеет номер 0022.mp3
         int thisTh = thousands;
         if(thisTh == 2)
          thisTh = 22; // меняем "два" на "две"
          
         playMelodyDelayed(thisTh,currentRaiseDelay);         
         currentRaiseDelay += getMelodyDelay(thisTh);
         
         Serial.println(String(F("PLAY THOUSANDS: ")) + 1000);

         // вот тут можно со склонением поиграться, например, произносить "тысячи" (1001.mp3 - слово "тысяч", 1002.mp3 - слово "тысячи")
         int add = 0;
         if(thousands < 5) // две тысячи, три тысячи, четыре тысячи, но - пять тысяч
          add = 2;
         else
          add = 1;
          
         playMelodyDelayed(1000 + add,currentRaiseDelay);
         currentRaiseDelay += getMelodyDelay(1000 + add);

      }
      else
      {
        // просто произносим "тысяча", потому что префикс "одна" - не нужен
        Serial.println(String(F("PLAY THOUSANDS: ")) + 1000);
        playMelodyDelayed(1000,currentRaiseDelay);
        currentRaiseDelay += getMelodyDelay(1000);

      }
   }

   // тысячи произнесли, произносим сотни
   if(hundreds > 0)
   {
      Serial.println(String(F("PLAY HUNDREDS: ")) + hundreds*100);
      playMelodyDelayed(hundreds*100,currentRaiseDelay);
      currentRaiseDelay += getMelodyDelay(hundreds*100);
      
   }

   // сотни произнесли, произносим десятки.
   // тут есть один нюанс: если число десятков и единиц меньше 21 - произносим его напрямую, иначе - составное.
   int tensAndOnes = tens*10 + ones;

   if(tensAndOnes <= 20)
   {
      if(tensAndOnes > 0) // ноль произносить не надо
      {
        Serial.println(String(F("PLAY TENSANDONES: ")) + tensAndOnes);
        playMelodyDelayed(tensAndOnes,currentRaiseDelay);
        currentRaiseDelay += getMelodyDelay(tensAndOnes);
        
      }
   }
   else
   {   
     if(tens > 0)
     {
        Serial.println(String(F("PLAY TENS: ")) + tens*10);
        playMelodyDelayed(tens*10,currentRaiseDelay);
        currentRaiseDelay += getMelodyDelay(tens*10);
        
     }
  
     // остались единицы
     if(ones > 0)
     {
        Serial.println(String(F("PLAY ONES: ")) + ones);
        playMelodyDelayed(ones,currentRaiseDelay);
        currentRaiseDelay += getMelodyDelay(ones);
        
     }
     
   } // else
   

}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void processSerialInput()
{
  while(Serial.available())
  {
    if(receiveBuffer.length() > 100)
    {
      receiveBuffer = "";
      return;
    }
    char ch = (char) Serial.read();
    if(ch == '\r')
      continue;
    else
    if(ch == '\n')
    {
      sayNumber(receiveBuffer);
      receiveBuffer = "";
    }
    else
    if(ch >= '0' && ch <= '9')
    {
      receiveBuffer += ch;
    }
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void setup() 
{
  Serial.begin(SERIAL_SPEED);
  Serial.println(F("Init, please wait..."));

  
  // настраиваем плеер  
  playerSerial.begin(DFPLAYER_SERIAL_SPEED);
  mp3_set_serial(playerSerial); // устанавливаем, через какой Serial будет работать DFPlayer с нами

  delay(DFPLAYER_INIT_DELAY);// ждём, пока плеер прочухается
  mp3_set_volume(DFPLAYER_INITIAL_VOLUME); // устанавливаем громкость плеера

  Serial.println(F("Inited, enter number, ends with newline:"));
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void loop() 
{
  if(DelayedEvents.empty()) // только когда закончили играть предыдущую мелодию
    processSerialInput();
    
  DelayedEvents.update();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void yield()
{
  DelayedEvents.update();
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

