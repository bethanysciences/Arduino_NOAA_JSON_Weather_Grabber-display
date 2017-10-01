/*----------------------------------------------------------------------*
  Defines for Adafruit LED Backpack i2c LED displays
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
Adafruit_7segment ClockDisp = Adafruit_7segment();
Adafruit_7segment hTempDisp = Adafruit_7segment();
Adafruit_7segment lTempDisp = Adafruit_7segment();
Adafruit_7segment cTempDisp = Adafruit_7segment();
#define Clock_I2C   0x75    // yellow
#define hTemp_I2C   0x71    // red
#define cTemp_I2C   0x72    // grn
#define lTemp_I2C   0x70    // blu
