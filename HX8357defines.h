/*----------------------------------------------------------------------*
  Defines for Adafruit HX8357 TFT display
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

#define TFT_CS    7                     // HX8357 WHT wire to MKR1000 pin
#define TFT_DC    6                     // HX8357 GRN wire to MKR1000 pin
#define TFT_RST   5                     // HX8357 ORG wire to MKR1000 pin
#define TFT_MISO  5                     // HX8357 GRY wire to MKR1000 pin
#define SD_CS     2                     // HX8357 WHT wire to MKR1000 pin
#define FGROUND   HX8357_WHITE          // primary text color
#define BGROUND   HX8357_BLUE           // primary background color
#define GREEN     HX8357_GREEN
#define BLACK     HX8357_BLACK
#define RED       HX8357_RED
#define CYAN      HX8357_CYAN
#define MAGENTA   HX8357_MAGENTA
#define YELLOW    HX8357_YELLOW
int LineNu[80];                         // line structure
#define MARGIN    8                     // margin (pixels)
#define SLINE     16                    // base line height (pixels)
#define LINE      25                    // line height (pixels)
#define TAB       130                   // tab width (pixels)
