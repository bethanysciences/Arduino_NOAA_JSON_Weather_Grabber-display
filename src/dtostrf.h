/*----------------------------------------------------------------------*
  dtostrf.h  Emulation for dtostrf function to convert floats to fixed string
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  "dtostrf.h"   // Convert float to string needed because avr-libc sprintf does not format floats
                // char *dtostrf(double val, signed char width, unsigned char prec, char *sout)
                // val      float variable
                // width    string length returned INCLUDING decimal point
                // prec     number of digits after the deimal point to print
                // sout     destination of output buffer (must be large enough)

  2017 Bob Smith https://github.com/bethanysciences
  Orgin by Cristian Maglie https://github.com/cmaglie
 *----------------------------------------------------------------------*/
char *dtostrf (double val, signed char width,
               unsigned char prec, char *sout) {
  char fmt[20];
  sprintf(fmt, "%%%d.%df", width, prec);
  sprintf(sout, fmt, val);
  return sout;
}
