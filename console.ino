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
#include "HX8357defines.h"
#include <Adafruit_LEDBackpack.h>
#include "LEDBackpackdefines.h"
#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include "NOAAmetar.h"              // Gets current METAR string
#include "NOAAForecast.h"           // Gets current NOAA v3 Forcast
#include "timestamp.h"              // breaks down timestamp string
#include "WiFiCreds.h"              // WiFi credentials
#include "wxconversions.h"          // weather conversions
#include "convertTime.h"            // timezone and 12/24 hr conversion
#include "dtostrf.h"                // Convert float to string
#include "ClockDefines.h"

RTCZero rtc;
SimpleTimer timer;
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
WiFiSSLClient client;            // client for NOAAForecast
HttpClient xml;                  // client for NOAACurrent
int status  = WL_IDLE_STATUS;
String CURRSTATION = "KPDK";     // station KPDK for NOAA METAR pull
String FCSTLAT     = "33.8774";  // station KPDK NOAA forecast v3 pull
String FCSTLON     = "-84.3046";

void setup() {            // ================ SETUP ====================== //
  Serial.begin(115200);
  Serial.print("START UP");

  pinMode(LED_BUILTIN, OUTPUT);     // setup onboard LED

  // -------------------------------- LEDS SETUP ------------=------------ //
  ClockDisp.begin(Clock_I2C);
  hTempDisp.begin(hTemp_I2C);
  lTempDisp.begin(lTemp_I2C);
  cTempDisp.begin(cTemp_I2C);

  // -------------------------------- TFT SETUP -------------------------- //
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
  char curr[50];  sprintf(curr, "Time Stamp %s:%s%s",
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

  WriteLED(0, 0, tempC);

  // -------------------------------- DEBUG ------------------------------ //
  int cchour;
  bool pm;
  convertTime(rtc.getHours(), TIME24, &cchour, &pm);
  char TimeStamp[50];
  sprintf(TimeStamp, "%02d:%02d%s", cchour, rtc.getMinutes(), AMPM[pm]);
  Serial.print(TimeStamp);              Serial.print("\t");
  Serial.print(curr);                   Serial.print("\t");
  Serial.print(current.tempC);          Serial.print("\t");
  Serial.print(current.dewpointC);      Serial.print("\t");
  Serial.print(current.altSettingHG);   Serial.print("\t");
  Serial.print(current.windspeedKTS);   Serial.print("\t");
  Serial.print(current.winddirDeg);     Serial.print("\t");
  Serial.println(current.visibilitySM);
}

void getForecast() {      // ================ GET FORECAST =============== //
  NOAAForecast();
  renderForecast();
}

void renderForecast() {   // ================ DISPLAY FORECAST =========== //
  double templ, temph;
  int yearGen, monthGen, dateGen, hourGen, minuteGen;
  bool pmGen;
   timeStamp(Forecast.generated, TIME24, UTCOFFSET,
            &yearGen, &monthGen, &dateGen, &hourGen, &pmGen, &minuteGen);
  char agePretty[50];
  sprintf(agePretty, "%d:%02d%s", hourGen, minuteGen, pmGen);
  char p0[100];
  sprintf(p0,"%s %02d %s",Forecast.p0name,Forecast.p0tempC,Forecast.p0short);
  char p1[100];
  sprintf(p1,"%s %02d %s",Forecast.p1name,Forecast.p1tempC,Forecast.p1short);
  char p2[100];
  sprintf(p2,"%s %02d %s",Forecast.p2name,Forecast.p2tempC,Forecast.p2short);
  char p3[100];
  sprintf(p3,"%s %02d %s",Forecast.p3name,Forecast.p3tempC,Forecast.p3short);
  char p4[100];
  sprintf(p4,"%s %02d %s",Forecast.p4name,Forecast.p4tempC,Forecast.p4short);
  char p5[100];
  sprintf(p5,"%s %02d %s",Forecast.p5name,Forecast.p5tempC,Forecast.p5short);
  
  int firstl = 5;     int lastl = 11;     // display line limits
  tft.fillRect(0, LineNu[firstl]-LINE, tft.width(), LineNu[lastl], BGROUND);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);
  tft.print("Last Forecast: ");
  tft.print(agePretty);
  tft.fillRect(tft.width()/2, LineNu[firstl]-10, tft.width()/2, 10, GREEN);
  tft.setCursor(MARGIN, LineNu[firstl+1]);  tft.print(p0);
  tft.setCursor(MARGIN, LineNu[firstl+2]);  tft.print(p1);
  tft.setCursor(MARGIN, LineNu[firstl+3]);  tft.print(p2);
  tft.setCursor(MARGIN, LineNu[firstl+4]);  tft.print(p3);
  tft.setCursor(MARGIN, LineNu[firstl+5]);  tft.print(p4);
  tft.setCursor(MARGIN, LineNu[firstl+6]);  tft.print(p5);

  // -------------------------------- WRITE LEDS ------------------------- //
  if (Forecast.p0tempC <= Forecast.p1tempC) {
    templ = Forecast.p0tempC;
    temph = Forecast.p1tempC;
  }
  else {
    templ = Forecast.p1tempC;
    temph = Forecast.p0tempC;
  }
  WriteLED(templ, temph, 0);

  // -------------------------------- DEBUG ------------------------------ //
  char CurrentTime[50];
  sprintf(CurrentTime, "%02d:%02d", rtc.getHours(), rtc.getMinutes());
  Serial.print("Pulled   "); Serial.println(CurrentTime);
  Serial.print("Updated  "); Serial.println(agePretty);
  Serial.print("   ");       Serial.println(p0);
  Serial.print("   ");       Serial.println(p1);
  Serial.print("   ");       Serial.println(p2);
  Serial.print("   ");       Serial.println(p3);
  Serial.print("   ");       Serial.println(p4);
  Serial.print("   ");       Serial.println(p5);
}

void statusLine() {       // ================ STATUS LINE ================ //
  int firstl = 12;
  int lastl = 12;
  // -------------------------------- CLEAR AREA ------------------------- //
  tft.fillRect(0,LineNu[firstl]-LINE,tft.width(),LineNu[lastl],BGROUND);

  int hour12;
  bool pm;
  convertTime(rtc.getHours(), TIME24, &hour12, &pm);
  char stat[80];
  IPAddress ip = WiFi.localIP();
  sprintf(stat, "%s %d.%d.%d.%d %ddBm %02d/%02d/%02d %d:%02d:%02d%s %s",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), rtc.getDay(), rtc.getYear(),
          hour12, rtc.getMinutes(), rtc.getSeconds(),
          AMPM[pm], TIMEZONE);

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

void WriteLED(int templ, int temph, int Ctemp) { // ==== WRITE LEDS ======  //
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
  if (tempC > 99) {
    cTempDisp.writeDigitNum(0, (Ctemp / 100));
    cTempDisp.writeDigitNum(1, (Ctemp / 10));
    cTempDisp.writeDigitNum(3, (Ctemp % 10));
    cTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    cTempDisp.writeDigitNum(0, (Ctemp / 10));
    cTempDisp.writeDigitNum(1, (Ctemp % 10));
    cTempDisp.writeDigitRaw(3, B01110001);
    cTempDisp.writeDigitRaw(4, B00000000);
  }
  cTempDisp.writeDisplay();
  hTempDisp.writeDisplay();
  lTempDisp.writeDisplay();
}
