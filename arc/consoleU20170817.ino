#include <SPI.h>
#include <SD.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
// #include "Adafruit_ILI9341.h"
#include "WiFi101.h"
#include "WiFiUdp.h"
#include "ArduinoJson.h"
#include "RTCZero.h"
#include "SimpleTimer.h"
#include "WiFi101OTA.h"
#include "Adafruit_LEDBackpack.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans24pt7b.h"

const char ssid[]   = "iotworld";
const char pass[]   = "iotworld";
int status  = WL_IDLE_STATUS;

#define TFT_CS    7     // HX8357 WHT  9 Feather ILI9341
#define TFT_DC    6     // HX8357 GRN 10 Feather ILI9341
#define TFT_RST   5     // HX8357 ORG
#define TFT_MISO  5     // HX8357 GRY
// #define SD_CS     4     // 5 Feather ILI9341
// #define TFTTYPE ILI9341
// #define FGROUND ILI9341_WHITE
// #define BGROUND ILI9341_BLUE
#define FGROUND   HX8357_WHITE
#define BGROUND   HX8357_BLUE
#define GREEN     HX8357_GREEN
#define BLACK     HX8357_BLACK
#define RED       HX8357_RED
#define CYAN      HX8357_CYAN
#define MAGENTA   HX8357_MAGENTA
#define YELLOW    HX8357_YELLOW

#define MARGIN    8
#define XLENGTH   480
#define YLENGTH   320
#define MINS      60000
int LineNu[80];
#define LINE      15

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST); // 320x480 40 lines
// Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);     // 240x320 30 lines

Adafruit_7segment ClockDisp = Adafruit_7segment();
Adafruit_7segment hTempDisp = Adafruit_7segment();
Adafruit_7segment lTempDisp = Adafruit_7segment();
Adafruit_7segment cTempDisp = Adafruit_7segment();
#define Clock_I2C   0x73
#define hTemp_I2C   0x71
#define cTemp_I2C   0x72
#define lTemp_I2C   0x70
bool blinkColon = false;

WiFiSSLClient client;
char server[]       = "api.weather.gov";
char locForecast[]  = "/points/33.8774,-84.3046";
char prodForecast[] = "/forecast";
char locCurrent[]   = "/stations/KPDK";
char prodCurrent[]  = "/observations/current";
const unsigned long HTTPTimeout = 10000;

RTCZero rtc;
int UTCOffset    = -4;
char timeZone[]  = "EDT";
char ampm[2][3]  = {"am", "pm"};
#define Time24   false

SimpleTimer timer;

void convertTime(int utcHour, int utcDay, int *locHour, int *locDay, int *AorP) {
  *AorP = 0;
  *locHour = utcHour + UTCOffset;
  if (*locHour < 0) {
    *locHour += 24;
    *locDay = utcDay - 1;
  }
  else if (*locHour > 23) {
    *locHour -= 24;
    *locDay = utcDay - 1;
  }
  if (*locHour >= 12) {
    *AorP = 1;
  }
  if (!Time24) {
    if (*locHour > 12) *locHour -= 12;
    else if (*locHour == 0) *locHour += 12;
  }
}
double c2f(double celsius) {          // convert Celcius to Fahrenheit
    return ((celsius * 9) / 5) + 32;
}
double pa2hg(double pascals) {        // convert Pascals to in. mercury
    return (pascals * 0.000295333727);
}
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  // convert floats to fixed string
  // val      Your float variable;
  // width    Length of the string that will be created INCLUDING decimal point;
  // prec     Number of digits after the deimal point to print;
  // sout     Destination of output buffer
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  // WiFi.setPins(8,7,4,2);          // for Adafruit Feather M0 WiFi
  // while (!Serial);
    
  ClockDisp.begin(Clock_I2C);
  hTempDisp.begin(hTemp_I2C);
  lTempDisp.begin(lTemp_I2C);
  cTempDisp.begin(cTemp_I2C);    

  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(BGROUND);
  tft.setTextColor(FGROUND, BGROUND);

  Serial.println("-------------------------------------"); 
  Serial.print("SET TFT "); Serial.println(tft.width() / LINE);
  int l = 0;
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l;
    l += LINE;
    Serial.print(l);        Serial.print(",");
  }

  tft.fillRect(0, tft.height() - 8, tft.width(), 8, MAGENTA);
  
  Serial.println(); 
  Serial.println("-------------------------------------"); 
  Serial.println("connect ssid >");
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    tft.print(">");
    delay(500);
  }
  
  WiFiOTA.begin("Arduino", "password", InternalStorage);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wifi Connected "); Serial.println(ssid);
  
  rtc.begin();
  unsigned long epoch;
  Serial.println("-------------------------------------"); 
  Serial.print("Time (epoch) "); Serial.println(epoch);
  
  while (epoch == 0) {
    epoch = WiFi.getTime();
    tft.print(".");
    delay(500);
  }
  
  Serial.print("NTP Time (epoch) "); Serial.println(epoch);
  rtc.setEpoch(epoch);
  Serial.println("-------------------------------------"); 

  tft.fillRect(0, tft.height() - 8, tft.width(), 8, GREEN);
  tft.setTextColor(BLACK, GREEN);
  
  statusLine();
  getCurrent();
  getForecast();

  timer.setInterval(1000, RenderClock);
  timer.setInterval(MINS/10, statusLine);
  timer.setInterval(MINS, getCurrent);
  timer.setInterval(MINS * 5, getForecast);
}

void loop() {
  WiFiOTA.poll();
  timer.run();
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);
  int displayVal = (locHour * 100) + rtc.getMinutes();
  ClockDisp.print(displayVal);
}

void RenderClock() {
  blinkColon = !blinkColon;
  ClockDisp.drawColon(blinkColon);
  ClockDisp.writeDisplay();
}
void getForecast() {
  fetchNOAA(server, locForecast, prodForecast);
}
void getCurrent() {
  fetchNOAA(server, locCurrent, prodCurrent);
}
void fetchNOAA(const char* server, const char* location, const char* product) {  
  if (connect(server)) {
    if (sendRequest(location, product) && skipResponseHeaders()) {
      if (product == prodForecast) parseForecast();
      if (product == prodCurrent) parseCurrent();
    }
    // Serial.println("Disconnect");
    // Serial.println("-----------------");
    client.stop();
  }
}
bool connect(const char* hostName) {
  bool ok = client.connectSSL(hostName, 443);
  // Serial.println(ok ? "Connect" : "Connect Failed!");
  return ok;
}
bool sendRequest(const char* location, const char* product) {
  client.print("GET ");
  client.print(location);
  client.print(product);
  client.println(" HTTP/1.1");
  client.println("Host: api.weather.gov");
  client.print("User-Agent: bob@bethanysciences.net/arduinowx01/");
  client.println(product);
  client.println("Accept: application/ld+json");
  client.println("Connection: close");
  client.println();
}
bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTPTimeout);
  bool ok = client.find(endOfHeaders);
  // if (!ok) Serial.println("No response or invalid response!");
  return ok;
}

bool parseCurrent() {
  const size_t bufferSize = JSON_ARRAY_SIZE(0) + JSON_ARRAY_SIZE(1) + 
                          6*JSON_OBJECT_SIZE(1) + 6*JSON_OBJECT_SIZE(2) + 
                          16*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(16) + 
                          JSON_OBJECT_SIZE(28) + 2410;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) {
    Serial.println("Forecast parse fail");
    Serial.println("--------------------------");
    tft.setTextColor(BLACK, MAGENTA);
    tft.setTextSize(1);
    tft.fillRect(tft.width() / 2, tft.height() - 16, tft.width() / 2, 8, MAGENTA);
    tft.setCursor((tft.width() / 2) + MARGIN, tft.height() - 16);  
    tft.print("Current: parse fail");
    return false;
  }
  JsonObject& _context           = root["@context"];
  JsonObject& temperature        = root["temperature"];
  JsonObject& dewpoint           = root["dewpoint"];
  JsonObject& windDirection      = root["windDirection"];
  JsonObject& windSpeed          = root["windSpeed"];
  JsonObject& barometricPressure = root["barometricPressure"];
  JsonObject& seaLevelPressure   = root["seaLevelPressure"];
  JsonObject& relativeHumidity   = root["relativeHumidity"];
  JsonObject& heatIndex          = root["heatIndex"];

  const char* timestamp = root["timestamp"];
  const char* text      = root["textDescription"];
  const char* icon      = root["icon"];
  int temp    = temperature["value"];
  float dwpt  = dewpoint["value"];
  int wDir    = windDirection["value"];
  float wind  = windSpeed["value"];
  double baro = barometricPressure["value"];
  double sLvl = seaLevelPressure["value"];
  float rHum  = relativeHumidity["value"];
  float htIx  = heatIndex["value"];

  String ageString = String(timestamp);         // 2017-08-04T02:30:03+00:00
  int firstDash    = ageString.indexOf("-");
  int secondDash   = ageString.lastIndexOf("-");
  int firstT       = ageString.indexOf("T");
  int firstColon   = ageString.indexOf(":");
  int secondColon  = ageString.lastIndexOf(":");
  
  String yearStr   = ageString.substring(0, firstDash);
  String monthStr  = ageString.substring(firstDash + 1);
  String dayStr    = ageString.substring(secondDash + 1);
  String hourStr   = ageString.substring(firstT + 1);
  String minuteStr = ageString.substring(firstColon + 1);

  int cyear        = yearStr.toInt();
  int cmonth       = monthStr.toInt();
  int cday         = dayStr.toInt();
  int chour        = hourStr.toInt();
  int cminute      = minuteStr.toInt();
  
  char agePretty[50];
  sprintf(agePretty, "%02d-%02d %02d:%02d",
                      cmonth, cday, chour, cminute);
  temp = c2f(temp);
  dwpt = c2f(dwpt);
  htIx = c2f(htIx);
  baro = pa2hg(baro);
  
  char temp_d[7]; dtostrf(temp, 3, 0, temp_d);
  char dwpt_d[7]; dtostrf(dwpt, 3, 0, dwpt_d);
  char wind_d[7]; dtostrf(wind, 3, 0, wind_d);
  char baro_d[9]; dtostrf(baro, 2, 2, baro_d);
  char sLvl_d[9]; dtostrf(sLvl, 3, 2, sLvl_d);
  char rHum_d[7]; dtostrf(rHum, 3, 0, rHum_d);
  char htIx_d[7]; dtostrf(htIx, 3, 0, htIx_d);

  char textStr[80]; sprintf(textStr, "Conditions %s at %s", text, agePretty);
  char tempStr[40]; sprintf(tempStr, "Temperature %sF",   temp_d);
  char rHumStr[40]; sprintf(rHumStr, "Humidity    %s%%",  rHum_d);
  char dwptStr[40]; sprintf(dwptStr, "Dew Point   %sF",   dwpt_d);
  char htIxStr[40]; sprintf(htIxStr, "Heat Index  %sF",   htIx_d);
  char baroStr[40]; sprintf(baroStr, "Barometer    %shg", baro_d);
  char windStr[40]; sprintf(windStr, "Wind       %smph",  wind_d);
  
  tft.fillRect(0, LineNu[0], tft.width(), LINE * 4, BGROUND);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[1]);                     tft.print(textStr);
  tft.setCursor(MARGIN, LineNu[2]);                     tft.print(tempStr);
  tft.setCursor((tft.width() / 2) + MARGIN, LineNu[2]); tft.print(rHumStr);
  tft.setCursor(MARGIN, LineNu[3]);                     tft.print(dwptStr); 
  tft.setCursor((tft.width() / 2) + MARGIN, LineNu[3]); tft.print(htIxStr);
  tft.setCursor(MARGIN, LineNu[4]);                     tft.print(baroStr);
  tft.setCursor((tft.width() / 2) + MARGIN, LineNu[4]); tft.print(windStr);

  Serial.println("CURRENT---------------------"); 
  Serial.print("Readings "); Serial.println(agePretty);
  Serial.println(textStr);
  Serial.println(tempStr);
  Serial.println(rHumStr);
  Serial.println(dwptStr);
  Serial.println(htIxStr);
  Serial.println(baroStr);
  Serial.println(windStr);
  Serial.println("--------------------------");

  tft.setFont();
  tft.setTextColor(BLACK, GREEN);
  tft.setTextSize(1);
  tft.fillRect(tft.width() / 2, tft.height() - 16, tft.width() / 2, 8, GREEN);
  tft.setCursor((tft.width() / 2) + MARGIN, tft.height() - 16);
  tft.print(agePretty);
  
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

  return true;
}

bool parseForecast() {   
  const size_t bufferSize = JSON_ARRAY_SIZE(14) + 
              JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) + 
              14*JSON_OBJECT_SIZE(13) + 8350;              
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);
  
  if (!root.success()) {
    Serial.println("Forecast parse fail");
    tft.setTextColor(BLACK, MAGENTA);
    tft.setFont();
    tft.setTextSize(1);
    tft.fillRect(0, tft.height() - 16, tft.width() / 2, 8, MAGENTA);
    tft.setCursor(MARGIN, tft.height() - 16);  
    tft.print("Forecast: parse fail");
    return false;
  }

  JsonObject& _context = root["@context"];
   const char* generatedAt    = root["generatedAt"]; // 2017-08-04T02:30:03+00:00
  JsonArray& periods          = root["periods"];
  JsonObject& periods0        = periods[0];
   const char* periods0_name  = periods0["name"];
   int periods0_temp          = periods0["temperature"];
   const char* periods0_icon  = periods0["icon"];
   const char* periods0_short = periods0["shortForecast"];
  JsonObject& periods1        = periods[1];
   const char* periods1_name  = periods1["name"];
   int periods1_temp          = periods1["temperature"];
   const char* periods1_icon  = periods1["icon"];
   const char* periods1_short = periods1["shortForecast"];
  JsonObject& periods2        = periods[2];
   const char* periods2_name  = periods2["name"];
   int periods2_temp          = periods2["temperature"];
   const char* periods2_icon  = periods2["icon"];
   const char* periods2_short = periods2["shortForecast"];
  JsonObject& periods3        = periods[3];
   const char* periods3_name  = periods3["name"];
   int periods3_temp          = periods3["temperature"];
   const char* periods3_icon  = periods3["icon"];
   const char* periods3_short = periods3["shortForecast"];
  JsonObject& periods4        = periods[4];
   const char* periods4_name  = periods4["name"];
   int periods4_temp          = periods4["temperature"];
   const char* periods4_icon  = periods4["icon"];
   const char* periods4_short = periods4["shortForecast"];
  JsonObject& periods5        = periods[5];
   const char* periods5_name  = periods5["name"];
   int periods5_temp          = periods5["temperature"];
   const char* periods5_icon  = periods5["icon"];
   const char* periods5_short = periods5["shortForecast"];

  String ageString  = String(generatedAt);    // 2017-08-04T02:30:03+00:00
   int firstDash    = ageString.indexOf("-");
   int secondDash   = ageString.lastIndexOf("-");
   int firstT       = ageString.indexOf("T");
   int firstColon   = ageString.indexOf(":");
   int secondColon  = ageString.lastIndexOf(":");
   
   String yearStr   = ageString.substring(0, firstDash);
   String monthStr  = ageString.substring(firstDash + 1);
   String dayStr    = ageString.substring(secondDash + 1);
   String hourStr   = ageString.substring(firstT + 1);
   String minuteStr = ageString.substring(firstColon + 1);
   
   int fyear        = yearStr.toInt();
   int fmonth       = monthStr.toInt();
   int fday         = dayStr.toInt();
   int fhour        = hourStr.toInt();
   int fminute      = minuteStr.toInt();

  int flocHour, flocDay, fAorP;
  convertTime(fhour, fday, &flocHour, &flocDay, &fAorP);
  char agePretty[50];
  sprintf(agePretty, "Forecast:%02d-%02d %02d:%02d%s",
                     fmonth, flocDay, flocHour, fminute, ampm[fAorP]);

  Serial.println("FORECAST---------------------"); 
  Serial.print("--ageString "); Serial.println(ageString); 
  Serial.print("generatedAt "); Serial.println(generatedAt);
  Serial.print("-----dayStr "); Serial.println(dayStr);
  Serial.print("-------fday "); Serial.println(fday);
  Serial.print("----flocDay "); Serial.println(flocDay);
  Serial.println(agePretty);
  Serial.println("------------------------------"); 
  
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);
  char fetchedAt[50];
  sprintf(fetchedAt, "Fetched:%02d-%02d %02d:%02d%s",
          rtc.getMonth(), locDay, locHour, rtc.getMinutes(), ampm[AorP]);
  
  char per0[100];
  char per1[100];
  char per2[100];
  char per3[100];
  char per4[100]; 
  char per5[100];
  sprintf(per0, "%s %02d %s", periods0_name, periods0_temp, periods0_short);
  sprintf(per1, "%s %02d %s", periods1_name, periods1_temp, periods1_short);
  sprintf(per2, "%s %02d %s", periods2_name, periods2_temp, periods2_short);
  sprintf(per3, "%s %02d %s", periods3_name, periods3_temp, periods3_short);
  sprintf(per4, "%s %02d %s", periods4_name, periods4_temp, periods4_short);
  sprintf(per5, "%s %02d %s", periods5_name, periods5_temp, periods5_short);
  
  int firstline = 6;
  tft.fillRect(0, LineNu[firstline], tft.width(), LineNu[firstline] - 16, BGROUND);
  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND); 


  tft.setCursor(0, LineNu[firstline]);
//  tft.print(periods0_name);
//  tft.setCursor(100, LineNu[firstline]);  tft.print(periods0_temp); tft.print("°F"); 
//  tft.setCursor(125, LineNu[firstline]);  tft.println(periods0_short);

//  tft.print(periods1_name);
//  tft.setCursor(100, LineNu[firstline] + 2);  tft.print(periods1_temp); tft.print("°F"); 
//  tft.setCursor(125, LineNu[firstline] + 2);  tft.println(periods1_short);

  
  sprintf(per1, "%s %02d %s", periods1_name, periods1_temp, periods1_short);
  sprintf(per2, "%s %02d %s", periods2_name, periods2_temp, periods2_short);
  sprintf(per3, "%s %02d %s", periods3_name, periods3_temp, periods3_short);
  sprintf(per4, "%s %02d %s", periods4_name, periods4_temp, periods4_short);
  sprintf(per5, "%s %02d %s", periods5_name, periods5_temp, periods5_short);
  
tft.println(per0);
tft.println(per1);
tft.println(per2);
tft.println(per3);
tft.println(per4);
tft.println(per5);  

  tft.setFont();
  tft.setTextColor(BLACK, GREEN);
  tft.setTextSize(1);
  tft.fillRect(0, tft.height() - 16, tft.width() / 2, 8, GREEN);
  tft.setCursor(MARGIN, tft.height() - 16);
  tft.print(agePretty);

  int templ, temph;
  if (periods0_temp <= periods1_temp) {
    templ = periods0_temp;
    temph = periods1_temp; 
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
  }
  else {
    templ = periods1_temp;
    temph = periods0_temp;
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
  }
  hTempDisp.writeDisplay();
  lTempDisp.writeDisplay();

  return true;
}

void statusLine() {
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);
  char stat[80];
  IPAddress ip = WiFi.localIP();
  sprintf(stat, "%s %d.%d.%d.%d %ddBm %02d/%02d/%02d %d:%02d:%02d%s EDT",
          WiFi.SSID(), ip[0], ip[1], ip[2], ip[3], WiFi.RSSI(),
          rtc.getMonth(), locDay, rtc.getYear(), locHour,
          rtc.getMinutes(), rtc.getSeconds(), ampm[AorP]);

  tft.setFont();
  tft.setTextSize(1);
  if (WiFi.RSSI() < -70) {
    tft.setTextColor(BLACK, MAGENTA);
    tft.setCursor(MARGIN, tft.height() - 8);
    tft.print(stat);
    return; 
  }
  tft.setTextColor(BLACK, GREEN);
  tft.setCursor(MARGIN, tft.height() - 8);
  tft.print(stat);
}

/*
BITMAPS
To make new bitmaps, make sure they are less than 320 by 480 pixels 
and save them in 24-bit BMP format! They must be in 24-bit format, 
even if they are not 24-bit color as that is the easiest format for 
the Arduino. You can rotate images using the setRotation() procedure

You can draw as many images as you want - dont forget the names must 
be less than 8 characters long. Just copy the BMP drawing routines 
below loop() and call
bmpDraw(bmpfilename, x, y);
*/
