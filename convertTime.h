/*----------------------------------------------------------------------*
  Convert time function
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  #include "convertTime.h"    // timezone and 12/24 hr conversion
                // convertTime(int Hour24, bool Time24, int &Hour12, bool &pm)
                // Hour24   hour in 24 hour format
                // &Hour12   hour in 12 hour format (return)
                // Time24   bool 24 hour time y/n
                // &pm      bool pm y/n

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
void convertTime(int Hour24, bool TIME24, int *Hour12, bool *pm) {
  *pm = 0;                                // set as am
  *Hour12 = Hour24;
  if (!Time24) {                          // 24 hour = false
    if (*Hour12 >= 12) {
      *Hour12 -= 12;                      // 1300 > 100
      *pm = 1;                            // set as pm
    }
  }
}
