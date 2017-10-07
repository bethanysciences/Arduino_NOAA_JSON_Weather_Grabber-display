/*-------------------------------------------------------------------------*
  Weather defines

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *-------------------------------------------------------------------------*/

// ---------------- NOAA METAR API settings -------------------------------*/
// for NOAACurrent.h
struct c_rent {
String metar;
String obstimeUTC;
double tempC;
double dewpointC;
   int winddirDeg;
   int windspeedKTS;
   int visibilitySM;
double altSettingHG;
   int year;
   int month;
   int date;
   int hour;
   int minute;
  bool pm;
};
typedef struct c_rent Curr;
Curr Current;

// ---------------- NOAA Forecast API v3 settings -------------------------*/
// for NOAAForecast.h

struct f_cast {
  char generated[22];
  char p0[50];
  char p1[50];
  char p3[50];
  char p4[50];
  char p5[50];
  char p0name[50];
  char p1name[50];
  char p2name[50];
  char p3name[50];
  char p4name[50];
  char p5name[50];
double p0tempC;
double p1tempC;
double p2tempC;
double p3tempC;
double p4tempC;
double p5tempC;
  char p0icon[50];
  char p1icon[50];
  char p2icon[50];
  char p3icon[50];
  char p4icon[50];
  char p5icon[50];
  char p0short[50];
  char p1short[50];
  char p2short[50];
  char p3short[50];
  char p4short[50];
  char p5short[50];
   int year;
   int month;
   int day;
   int hour;
   int minute;
  bool pm;
};
typedef struct f_cast Fcst;
Fcst Forecast;


