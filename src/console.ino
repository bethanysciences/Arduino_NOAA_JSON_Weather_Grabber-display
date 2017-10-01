/*-------------------------------------------------------------------------*
  Weather and Information Console

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *-------------------------------------------------------------------------*/
#include <RTCZero.h>
#include <SimpleTimer.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include <Adafruit_HX8357.h>
#include <HX8357defines.h>
#include <Adafruit_LEDBackpack.h>
#include <LEDBackpackdefines.h>
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "NOAAmetar.h"              // Gets current METAR string
                // NOAAmetar(String station, int &respcode)
                // inputs
                //   station                 reporting station usually airport
                // returns
                //   &respcode               http response code e.g. 200 = ok
                //   struct NOAACurrent      current
                //     current.metar         formatted METAR strip
                //     current.obstimeUTC    observation time UTC
                //     current.tempC         temperature °Celcius
                //     current.dewpointC     dew point °Celcius
                //     current.winddirDeg    wind direction °0-359
                //     current.windspeedKTS  wind speed knots per hour
                //     current.visibilitySM  visibility statute miles
                //     current.altSettingHG  altimeter setting inches hg
                //     current.year          string forecast year
                //     current.month         string forecast month
                //     current.day           string forecast day
                //     current.hour          string forecast hour
                //     current.minute        string forecast minute
                //     current.pm            string forecast pm y/n;
#include "NOAAForecast.h"           // Gets current NOAA v3 Forcast
                // NOAAForecast(String latitude, Sting longitude)
                // inputs
                //   latitude           station longitude e.g. "33.8774"
                //   longitude          station longitude e.g. "-84.3046"
                // returns
                //   forecast.generated string forecast time 2017-09-23T19:53:00Z
                //   periods px (0-10)
                //   forecast.pxname    string period name
                //   forecast.pxtemp    string temperature °celcius
                //   forecast.pxicon    string icon ult
                //   forecast.pxshort   string shortForecast
                //   forecast.year      string forecast year
                //   forecast.month     string forecast month
                //   forecast.day       string forecast day
                //   forecast.hour      string forecast hour
                //   forecast.minute    string forecast minute
                //   forecast.pm        string forecast pm y/n;
#include "timestamp.h"              // breaks down timestamp string
                // timeStamp(String timeStamp, bool hour24TS, int UTCoffsetTS,
                //            int &yearTS, int &monthTS, int &dateTS,
                //            int &hourTS, bool &pmTS, int &minuteTS)
                // inputs
                //   timeStamp    string as 2017-09-23T19:53:00Z
                //   hour24TS     24 hour format as y/n select
                //   UTCoffsetTS) timezone offset in hours
                // returns
                //   &yearTS      int year as yyyy
                //   &monthTS     int month as mm
                //   &dateTS      int date timezone offset as dd
                //   &hourTS      int hour timezone offset and 12/24 hour as hh
                //   &pmTS        bool pm y/n
                //   &minuteTS    int minutes as mm
#include "WiFiCreds.h"              // WiFi credentials
              // char ssid[] = " " network SSID (name)
              // char pass[] = " " network password
#include "wxconversions.h"          // weather conversions
              // Celc > Fahr double c2f(double [temp °celcius])
              //             returns (double [temp °fahrenheit])
              // Fahr > Celc double f2c(double [temp °fahrenheit])
              //             retuns (double [temp °celcius])
              // Humidity    double rh(double [dew point °celcius], double [temp °celcius])
              //             returns (double [% rel humidity])
              // Windchill   float wc(double [temp °celcius], int [MPH windspeed]}
              //             returns (float [windchill °celcius])
              // PA -> "HG   double p2h(double pascals)
              //             returns double [presure in inches mercury])
              // Dew Point   double dp(double [temp °celcius or °fahrenheit],
              //               int [% rel humidity])
              //             returns double [dew point °celcius or °fahrenheit]
              // Heat Index  double hi(double [temp °celcius or °fahrenheit],
              //               int [% rel humidity], bool [°celcius input ?])
              //             returns heat index double [temp °celcius or °fahrenheit]
              // MED         double med(int [uvindex], int [altitude meters], bool [on water ?],
              //               bool [on snow ?], int [fitz skin type], int [spf applied])
              //             returns int [mins] to Min Erythemal Dose (MED) - sunburn
#include "convertTime.h"            // timezone and 12/24 hr conversion
              // convertTime(int Hour24, bool Time24, int &Hour12, bool &pm)
              // inputs
              //   Hour24   int hour in 24 hour format
              //   Time24   bool 24 hour time y/n
              // returns
              //   &Hour12  int hour in 12 hour format (return)
              //   &pm      bool pm y/n
#include "dtostrf.h"                // Convert float to string
              // char *dtostrf(double val, signed char width, unsigned char prec, char *sout)
              // inputs
              //   val      float variable
              //   width    string length returned INCLUDING decimal point
              //   prec     number of digits after the deimal point to print
              // returns
              //   sout     destination of output buffer (must be large enough)


  // -------------------------------- Declare Structs -------------------- //
RTCZero rtc;
SimpleTimer timer;
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
WiFiSSLClient client;           // client for NOAAForecast
HttpClient xml;                 // client for NOAACurrent
NOAAForecast forecast;          // NOAA forecast api.weather.gov v3 get
NOAACurrent current;            // NOAA current METAR get aviationweather.gov
int status  = WL_IDLE_STATUS;

void setup() {            // ================ SETUP ====================== //
  Serial.begin(115200);
  Serial.print("START UP");

  pinMode(LED_BUILTIN, OUTPUT);     // setup onboard LED

  // -------------------------------- LEDS SETUP ------------------------ //
  ClockDisp.begin(Clock_I2C);
  hTempDisp.begin(hTemp_I2C);
  lTempDisp.begin(lTemp_I2C);
  cTempDisp.begin(cTemp_I2C);

  // -------------------------------- TFT SETUP ------------------------ //
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);

  int l = 0;      // counts display lines based on LINE height pixels
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l; l += LINE;
  }

  int firstl = 1;
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);

  tft.print("TFT width ");     tft.println(tft.width());
  tft.print("TFT height ");    tft.println(tft.height());
  tft.print("TFT Lines ");     tft.println(tft.height() / LINE);

  Serial.print("TFT width ");  Serial.println(tft.width());
  Serial.print("TFT height "); Serial.println(tft.height());
  Serial.print("TFT Lines ");  Serial.println(tft.height() / LINE);

  // -------------------------------- WIFI SETUP ------------------------- //

  tft.print("connect to >");    tft.print(ssid);

  Serial.print("connect to >"); Serial.print(ssid);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    tft.print(">");
    delay(500);
  }
  tft.print("> connected ");    tft.println();
  Serial.println("> connected ");
  digitalWrite(LED_BUILTIN, HIGH);      // lite onboard LED

  // -------------------------------- SET CLOCK -------------------------- //
  rtc.begin();
  unsigned long epoch;

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.print("Time (epoch) ");       tft.println(epoch);
  Serial.print("Time (epoch) ");    Serial.println(epoch);

  while (epoch == 0) {
    epoch = WiFi.getTime();
    tft.print(".");
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print(" NTP epoch acquired  ");      Serial.println(epoch);

  epoch = epoch + UTCOffsetEpoch;

  Serial.print(" Epoch timezone adjusted ");  Serial.println(epoch);

  rtc.setEpoch(epoch);

  Serial.print(" > time ");   Serial.print(rtc.getHours());
  Serial.print(":");          Serial.println(rtc.getMinutes());
  bool blinkColon = false;

  // -------------------------------- HEADER FOR DEBUG REEDINGS ---------- //
  Serial.print("pulled"); Serial.print("\t");
  Serial.print("update"); Serial.print("\t");
  Serial.print(" temp"); Serial.print("\t");
  Serial.print(" rHum"); Serial.print("\t");
  Serial.print(" htIx"); Serial.print("\t");
  Serial.print(" dwpt"); Serial.print("\t");
  Serial.print(" baro"); Serial.print("\t");
  Serial.print("  wind"); Serial.print("\t");
  Serial.println("conditions ");

  // -------------------------------- SET TIMERS ------------------------- //
  timer.setInterval(1000, cycleColon);      // every second
  renderClock(); timer.setInterval(MIN, renderClock);      // every minute
  statusLine();  timer.setInterval(MIN/10, statusLine);    // every 6 secs
  getCurrent();  timer.setInterval(MIN * 10, getCurrent);  // every 10 mins
  getForecast(); timer.setInterval(MIN * 60, getForecast); // 1x / hour
}

void loop() {             // ================ LOOP ======================= //
  timer.run();
}

void cycleColon() {       // ================ COLON ====================== //
  blinkColon = !blinkColon;
  ClockDisp.drawColon(blinkColon);
  ClockDisp.writeDisplay();
}

void renderClock() {      // ================ CLOCK DISPLAY ============== //
  int hour, AorP;
  convertTime(rtc.getHours(), &hour, &AorP);
  if ((hour / 10) > 0) {
    ClockDisp.writeDigitNum(0, (hour / 10));                   // drop lead 0
  }
  else ClockDisp.writeDigitRaw(0, 0x0);
  ClockDisp.writeDigitNum(1, (hour % 10));
  ClockDisp.writeDigitNum(3, rtc.getMinutes() / 10);
  ClockDisp.writeDigitNum(4, rtc.getMinutes() % 10);
  ClockDisp.writeDisplay();
}

void getCurrent() {       // ================ GET CURRENT ================ //
  NOAAmetar(station);
  renderCurrent();
}

void renderCurrent() {    // ================ DISPLAY CURRENT ============ //
  char curr[50];  sprintf(current, "Time Stamp %s:%s%s",
                            current.hour, current.minute, current.pm);
  char tempC[20]; sprintf(tempC, "Temperature %s°C", current.tempC);
  char dwptC[20]; sprintf(dwptC, "Dew Point %s°C", current.dewpointC);
  char baro[40];  sprintf(baro, "%s""hg", current.altSettingHG);
  char wind[40];  sprintf(wind, "Wind Speed %sKTS Blowing %deg",
                            current.windspeedKTS, current.winddirDeg);
  char visi[40];  sprintf(visi, "Visibility %s Miles", current.visibilitySM);

  // -------------------------------- WRITE TFT -------------------------- //
  tft.fillRect(tft.width()/2, LineNu[firstl]-10, tft.width()/2, 10, GREEN)
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);      tft.print(curr);
  tft.setCursor(MARGIN, LineNu[firstl + 1]);  tft.print(tempC);
  tft.setCursor(MARGIN, LineNu[firstl + 2]);  tft.print(dwptC);
  tft.setCursor(MARGIN, LineNu[firstl + 3]);  tft.print(baro);
  tft.setCursor(MARGIN, LineNu[firstl + 4]);  tft.print(wind);
  tft.setCursor(MARGIN, LineNu[firstl + 5]);  tft.print(visi);

  WriteLED([], [], int temp);

  // -------------------------------- DEBUG ------------------------------ //
  convertTime(rtc.getHours(), &cchour, &AorP);
  char TimeStamp[50];
  sprintf(TimeStamp, "%02d:%02d%s", cchour, rtc.getMinutes(), ampm[AorP]);
  Serial.print(TimeStamp);              Serial.print("\t");
  Serial.print(curr);                   Serial.print("\t");
  Serial.print(current.tempC);          Serial.print("\t");
  Serial.print(current.dewpointC);      Serial.print("\t");
  Serial.print(current.altSettingHG);   Serial.print("\t");
  Serial.print(current.windspeedKTS);   Serial.print("\t");
  Serial.print(current.winddirDeg);     Serial.print("\t");
  Serial.println(current.visibilitySM);

  return true;
}

void getForecast() {      // ================ GET FORECAST =============== //
  NOAAForecast(lat, lon);
  renderForecast();
}

void renderForecast() {   // ================ DISPLAY FORECAST =========== //
  timeStamp(forecast.generated, hour24, UTCOffset,
                &yearGen, &monthGen, &dateGen, &hourGen, &pmGen, &minuteGen)
  char agePretty[50];
  sprintf(agePretty, "%d:%02d%s", hourGen, minuteGen, pmGen);
  char per0[100];
  sprintf(per0, "%s %02d %s",forecast.p0name,forecast.p0temp,forecast.p0short);
  char per1[100];
  sprintf(per1, "%s %02d %s",forecast.p1name,forecast.p1temp,forecast.p1short);
  char per2[100];
  sprintf(per2, "%s %02d %s",forecast.p2name,forecast.p2temp,forecast.p2short);
  char per3[100];
  sprintf(per3, "%s %02d %s",forecast.p3name,forecast.p3temp,forecast.p3short);
  char per4[100];
  sprintf(per4, "%s %02d %s",forecast.p4name,forecast.p4temp,forecast.p4short);
  char per5[100];
  sprintf(per5, "%s %02d %s",forecast.p5name,forecast.p5temp,forecast.p5short);
  int firstl = 5;     int lastl = 11;     // display line limits
  tft.fillRect(0, LineNu[firstl]-LINE, tft.width(), LineNu[lastl], BGROUND);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);
  tft.print("Last Forecast: ");
  tft.print(agePretty);
  tft.fillRect(tft.width()/2, LineNu[firstl]-10, tft.width()/2, 10, GREEN);
  tft.setCursor(MARGIN, LineNu[firstl+1]);  tft.print(per0);
  tft.setCursor(MARGIN, LineNu[firstl+2]);  tft.print(per1);
  tft.setCursor(MARGIN, LineNu[firstl+3]);  tft.print(per2);
  tft.setCursor(MARGIN, LineNu[firstl+4]);  tft.print(per3);
  tft.setCursor(MARGIN, LineNu[firstl+5]);  tft.print(per4);
  tft.setCursor(MARGIN, LineNu[firstl+6]);  tft.print(per5);

  // -------------------------------- WRITE LEDS ------------------------- //
  if (periods0_temp <= periods1_temp) {
    templ = forecast.p0temp;
    temph = forecast.p1temp;
  }
  else {
    templ = forecast.p1temp;
    temph = forecast.p0temp;
  }
  WriteLED(int templ, int temph, []);

  // -------------------------------- DEBUG ------------------------------ //
  char CurrentTime[50];
  sprintf(CurrentTime, "%02d:%02d", rtc.getHour(), rtc.getMinutes());
  Serial.print("Pulled   "); Serial.println(CurrentTime);
  Serial.print("Updated  "); Serial.println(agePretty);
  Serial.print("   ");       Serial.println(per0);
  Serial.print("   ");       Serial.println(per1);
  Serial.print("   ");       Serial.println(per2);
  Serial.print("   ");       Serial.println(per3);
  Serial.print("   ");       Serial.println(per4);
  Serial.print("   ");       Serial.println(per5);
  return true;
}

void statusLine() {       // ================ STATUS LINE ================ //
  int firstl = 12;
  int lastl = 12;
  // -------------------------------- CLEAR AREA ------------------------- //
  tft.fillRect(0, LineNu[firstl] - LINE, tft.width(), LineNu[lastl], BGROUND);

  int hour, AorP;
  convertTime(rtc.getHours(), &hour, &AorP);
  char stat[80];
  IPAddress ip = WiFi.localIP();
  sprintf(stat, "%s %d.%d.%d.%d %ddBm %02d/%02d/%02d %d:%02d:%02d%s %s",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          hour, rtc.getMinutes(), rtc.getSeconds(),
          ampm[AorP], timeZone);

  if (WiFi.RSSI() < -70) {
    tft.setFont(&FreeSans9pt7b);
    tft.setTextColor(MAGENTA);
    tft.setCursor(MARGIN, LineNu[firstl]);
    tft.print(stat);
    return;
  }
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);
  tft.print(stat);
}

void WriteLED(int templ, int temph, int temp) { // ==== WRITE LEDS ======  //
  if (templ > 99) {
    lTempDisp.writeDigitNum(0, (templ / 100));
    lTempDisp.writeDigitNum(1, (templ / 10));
    lTempDisp.writeDigitNum(3, (templ % 10));
    lTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    lTempDisp.writeDigitNum(0, (templ / 10));
    lTempDisp.writeDigitNum(1, (templ % 10));
    lTempDisp.writeDigitRaw(3, B01110001);
    lTempDisp.writeDigitRaw(4, B00000000);
  }
  if (temph > 99) {
    hTempDisp.writeDigitNum(0, (temph / 100));
    hTempDisp.writeDigitNum(1, (temph / 10));
    hTempDisp.writeDigitNum(3, (temph % 10));
    hTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    hTempDisp.writeDigitNum(0, (temph / 10));
    hTempDisp.writeDigitNum(1, (temph % 10));
    hTempDisp.writeDigitRaw(3, B01110001);
    hTempDisp.writeDigitRaw(4, B00000000);
  }
  if (temp > 99) {
    cTempDisp.writeDigitNum(0, (temp / 100));
    cTempDisp.writeDigitNum(1, (temp / 10));
    cTempDisp.writeDigitNum(3, (temp % 10));
    cTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    cTempDisp.writeDigitNum(0, (temp / 10));
    cTempDisp.writeDigitNum(1, (temp % 10));
    cTempDisp.writeDigitRaw(3, B01110001);
    cTempDisp.writeDigitRaw(4, B00000000);
  }
  cTempDisp.writeDisplay();
  hTempDisp.writeDisplay();
  lTempDisp.writeDisplay();
}
