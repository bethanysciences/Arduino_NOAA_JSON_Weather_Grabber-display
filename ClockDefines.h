/*----------------------------------------------------------------------*
  Time defines
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
int UTCOffset       = -4;               // EDT 2nd sun mar | EST 1st sun NOV
long UTCOffsetEpoch = UTCOffset * 3600; // epoch timezone offset
char timeZone[]     = "EDT";            // timezone lables
char ampm[2][3]     = {"am", "pm"};     // AM/PM preferred lables
#define TIME24      false               // false = use 12 hour time
#define MIN         60000               // 1 minute microseconds
