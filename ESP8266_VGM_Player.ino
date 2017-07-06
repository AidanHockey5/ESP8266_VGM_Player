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

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

unsigned long parseLocation = 64;
float sampleWait = 1;
void loop() 
{
  //sampleWait = mapfloat(analogRead(A0), 0, 1023, 0.5, 5);
  //Serial.println(sampleWait);

  switch(music_data[parseLocation])
  {
    case 0x50:
    parseLocation++;
    SendSNByte(music_data[parseLocation]);
    break;
    case 0x61: //There seems to be issues with how these delays are timed. Sample rate doesn't match
    {
    Serial.print("Hit 0x61. Wait: ");
    uint32_t wait = 0;
    parseLocation++;
    for ( int i = 0; i < 2; i++ ) 
    {
      wait += ( uint32_t( music_data[parseLocation] ) << ( 8 * i ));
      parseLocation++;
    }
    Serial.println(wait);
//    parseLocation++;
//    wait << music_data[parseLocation];
//    parseLocation++;
//    wait << music_data[parseLocation];
    while(wait-- > 0) delayMicroseconds(23);
    //for(int i=0; i < wait; i++) delay(0.023);
    break;
    }
    case 0x62:
    delay(16.67*0.75); //Actual time is 16.67 (1/60 of a second). 0.75 is just to speed songs up a little;
    break;
    case 0x63:
    delay(20*0.75);
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
    case 0x7F: //There seems to be issues with how these delays are timed. Sample rate doesn't match
    {
      //parseLocation++;
      Serial.print("Hit "); Serial.println(music_data[parseLocation]);
      uint32_t s = music_data[parseLocation] & 0x0F;
      while ( s-- > 0 ) delayMicroseconds(23);
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






























