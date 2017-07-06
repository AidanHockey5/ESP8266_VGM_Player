//Hey, this one actually works!
#include "music.h"
#define PROGMEM  ICACHE_RODATA_ATTR
//SN
const int WE = D3;

//Shift Register 595
const int latchPin = D0;
const int clockPin = D1;
const int dataPin = D2;
const int clearShift = D4;

float singleSampleWait = 0;
const float WAIT60TH = 1000 / (44100/735);
const float WAIT50TH = 1000 / (44100/882);

void setup() 
{
  Serial.begin(115200);
  //Setup 595
  pinMode(clearShift, OUTPUT);
  digitalWrite(clearShift, LOW);
  delay(50);
  digitalWrite(clearShift, HIGH);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  //Setup SN
  pinMode(WE, OUTPUT);
  digitalWrite(WE, HIGH);
  delay(500);
  SilenceAllChannels();
  delay(500);
}

void SilenceAllChannels()
{
  SendSNByte(0x9f);
  SendSNByte(0xbf);
  SendSNByte(0xdf);
  SendSNByte(0xff);
}

void SendSNByte(byte b)
{
  digitalWrite(WE, HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, b);   
  digitalWrite(latchPin, HIGH);
  digitalWrite(WE, LOW);
  delay(1);
  digitalWrite(WE, HIGH);
}

//Read music data from flash memory
uint8 ICACHE_FLASH_ATTR read_rom_uint8(const uint8* addr)
{
    uint32 bytes;
    bytes = *(uint32*)((uint32)addr & ~3);
    return ((uint8*)&bytes)[(uint32)addr & 3];
}

unsigned long parseLocation = 64;
uint32_t lastWaitData = 0;
float cachedWaitTime = 0;
void ICACHE_FLASH_ATTR loop(void) 
{
  switch(read_rom_uint8(&music_data[parseLocation]))
  {
    case 0x50:
    parseLocation++;
    SendSNByte(read_rom_uint8(&music_data[parseLocation]));
    break;
    case 0x61: 
    {
    uint32_t wait = 0;
    parseLocation++;
    for ( int i = 0; i < 2; i++ ) 
    {
      wait += ( uint32_t( read_rom_uint8(&music_data[parseLocation]) ) << ( 8 * i ));
      parseLocation++;
    }
    if(lastWaitData != wait) //Avoid doing lots of unnecessary division.
    {
      lastWaitData = wait;
      cachedWaitTime = 1000/(44100/wait);
    }
    delay(cachedWaitTime);
    break;
    }
    case 0x62:
    delay(WAIT60TH); //Actual time is 16.67ms (1/60 of a second)
    break;
    case 0x63:
    delay(WAIT50TH); //Actual time is 20ms (1/50 of a second)
    break;
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F: 
    {
      uint32_t wait = read_rom_uint8(&music_data[parseLocation]) & 0x0F;
      delay(1000/(44100/wait));
    break;
    }
    case 0x66:
    parseLocation = 64;
    break;
  }
  parseLocation++;
  if (parseLocation == music_length)
  {
    parseLocation = 64;
  }
}
