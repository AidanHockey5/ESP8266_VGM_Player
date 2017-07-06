//16F690
//#include <usb_serial.h>
#include <main.h>
#USE SPI (MASTER, SPI1, MODE=0, BITS=16, STREAM=SPI_1, MSB_FIRST)

unsigned char SN_CLK = PIN_C6;
unsigned char YM_CLK = PIN_C5;

void SetFrqManual(unsigned int16 oct, unsigned int16 dac, unsigned char target)
{
  byte CNF = 0b00000000;
  unsigned int16 BitMap = (oct << 12) | (dac << 2) | CNF;
  output_low(target);
  spi_xfer(BitMap); 
  output_high(target);
}

void main()
{
   output_high(SN_CLK); output_high(YM_CLK);
   delay_ms(50); //Debug delay for instruments to start up.
   SetFrqManual(12, 912, YM_CLK); //3.58MHz
   delay_ms(50);
   SetFrqManual(11, 831, SN_CLK); //3.58MHz
}