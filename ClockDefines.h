/*----------------------------------------------------------------------*
  Time defines
  
  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
int UTCOFFSET  = -4;                // EDT 2nd sun mar | EST 1st sun NOV
int UTCEPOCH    = UTCOOFFSET * 3600; // epoch timezone offset
char TIMEZONE   = "EDT";             // timezone lables
char AMPM[2]    = {"pm", "am"};      // AM/PM preferred lables
bool TIME24     = false;             // false = use 12 hour time
int MIN         = 60000;             // 1 minute microseconds

