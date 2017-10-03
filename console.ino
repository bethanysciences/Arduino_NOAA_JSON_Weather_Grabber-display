/*-------------------------------------------------------------------------*
  Weather and Information Console

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *-------------------------------------------------------------------------*/
#include "RTCZero.h"
#include "SimpleTimer.h"
#include "WiFi101.h"
#include "Adafruit_HX8357.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "WiFiCreds.h"              // WiFi credentials
#include "convertTime.h"            // timezone and 12/24 hr conversion
#include "dtostrf.h"                // Convert float to string
#include "ArduinoHttpClient.h"
#include "xmlTakeParam.h"
#include "ArduinoJson.h"

// ------------  Defines for Adafruit HX8357 TFT display ------------------- //
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
int firstl = 1;
#define MARGIN    8                     // margin (pixels)
#define SLINE     16                    // base line height (pixels)
#define LINE      25                    // line height (pixels)
#define TAB       130                   // tab width (pixels)
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// -------------------  Defines for Time Displays ---------------------- //
#define UTCOFFSET       -4            // EDT 2nd sun mar | EST 1st sun NOV
int UTCEPOCH          = -4 * 3600;    // epoch timezone offset
const char* TIMEZONE  = "EDT";        // timezone lables
const char* AMPM[2]   = {"pm", "am"}; // AM/PM preferred lables
bool TIME24           = false;        // false = use 12 hour time
int MIN               = 60000;        // 1 minute microseconds
bool blinkColon       = false;
RTCZero rtc;
SimpleTimer timer;

// --------- Defines for Adafruit LED Backpack i2c LED displays ------- //
#define Clock_I2C   0x75    // yellow
#define hTemp_I2C   0x71    // red
#define cTemp_I2C   0x72    // grn
#define lTemp_I2C   0x70    // blu
Adafruit_7segment ClockDisp = Adafruit_7segment();
Adafruit_7segment HTempDisp = Adafruit_7segment();
Adafruit_7segment LTempDisp = Adafruit_7segment();
Adafruit_7segment CTempDisp = Adafruit_7segment();

// ----------------------  Defines NOAA METAR ------------------------- //
   int currPort           = 80;
String currServer         = "aviationweather.gov";
String currProduct        = "/adds/dataserver_current/";
String currDataSource     = "metars";
String currRequestType    = "retrieve";
String currFormat         = "xml";
String currHoursBeforeNow = "1";
String currRequest        = "";
String currResp           = "";
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

// -------------------  Defines NOAA FORECAST ------------------ //
const char* FCSTLAT = "33.8774";     // station KPDK NOAA forecast v3 pull
const char* FCSTLON = "-84.3046";
const char* FcstServer  = "api.weather.gov";
int    FcstPort    = 443;
const unsigned long HTTPTimeout = 10000;
struct f_cast {
const char* generated;
const char* p0;
const char* p1;
const char* p3;
const char* p4;
const char* p5;
const char* p0name;
const char* p1name;
const char* p2name;
const char* p3name;
const char* p4name;
const char* p5name;
double p0tempC;
double p1tempC;
double p2tempC;
double p3tempC;
double p4tempC;
double p5tempC;
const char* p0icon;
const char* p1icon;
const char* p2icon;
const char* p3icon;
const char* p4icon;
const char* p5icon;
const char* p0short;
const char* p1short;
const char* p2short;
const char* p3short;
const char* p4short;
const char* p5short;
   int year;
   int month;
   int date;
   int hour;
   int minute;
  bool pm;
};
typedef struct f_cast Fcst;
Fcst Forecast;

// -------------------  Defines for WIFI Operations ------------------- //
WiFiClient wifi;
HttpClient xml = HttpClient(wifi, currServer, currPort);
WiFiSSLClient client;           // client for NOAAForecast
int status  = WL_IDLE_STATUS;

void setup() {            // ================ SETUP ====================== //
  Serial.begin(115200);
  Serial.print("START UP");

  pinMode(LED_BUILTIN, OUTPUT);     // setup onboard LED

  // -------------------------------- LEDS SETUP ------------=------------ //
  ClockDisp.begin(Clock_I2C);
  HTempDisp.begin(hTemp_I2C);
  LTempDisp.begin(lTemp_I2C);
  CTempDisp.begin(cTemp_I2C);

  // -------------------------------- TFT SETUP -------------------------- //
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);

  int l = 0;      // counts display lines based on LINE height pixels
  for (int i = 0; i <= tft.width()/LINE; i++) {
    LineNu[i] = l; l += LINE;
  }

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

  epoch = epoch + UTCEPOCH;

  Serial.print(" Epoch timezone adjusted ");  Serial.println(epoch);

  rtc.setEpoch(epoch);

  Serial.print(" > time ");   Serial.print(rtc.getHours());
  Serial.print(":");          Serial.println(rtc.getMinutes());

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
  int hour;
  bool AorP;
  convertTime(rtc.getHours(), TIME24, &hour, &AorP);
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
  NOAAmetar();
  renderCurrent();
}

void renderCurrent() {    // ================ DISPLAY CURRENT ============ //
  char curr[50];  sprintf(curr, "Time Stamp %s:%s%s",
                          Current.hour, Current.minute, Current.pm);
  char tempC[20]; sprintf(tempC, "Temperature %s°C", Current.tempC);
  char dwptC[20]; sprintf(dwptC, "Dew Point %s°C", Current.dewpointC);
  char baro[40];  sprintf(baro, "%s""hg", Current.altSettingHG);
  char wind[40];  sprintf(wind, "Wind Speed %sKTS Blowing %deg",
                            Current.windspeedKTS, Current.winddirDeg);
  char visi[40];  sprintf(visi, "Visibility %s Miles", Current.visibilitySM);

  // -------------------------------- WRITE TFT -------------------------- //
  
  tft.setFont(&FreeSans12pt7b);
  tft.setTextColor(FGROUND);
  tft.setCursor(MARGIN, LineNu[firstl]);      tft.print(curr);
  tft.setCursor(MARGIN, LineNu[firstl + 1]);  tft.print(tempC);
  tft.setCursor(MARGIN, LineNu[firstl + 2]);  tft.print(dwptC);
  tft.setCursor(MARGIN, LineNu[firstl + 3]);  tft.print(baro);
  tft.setCursor(MARGIN, LineNu[firstl + 4]);  tft.print(wind);
  tft.setCursor(MARGIN, LineNu[firstl + 5]);  tft.print(visi);

  
  WriteLED(0, 0, Current.tempC);

  // -------------------------------- DEBUG ------------------------------ //
  int cchour;
  bool pm;
  convertTime(rtc.getHours(), TIME24, &cchour, &pm);
  char TimeStamp[50];
  sprintf(TimeStamp, "%02d:%02d%s", cchour, rtc.getMinutes(), AMPM[pm]);
  Serial.print(TimeStamp);              Serial.print("\t");
  Serial.print(curr);                   Serial.print("\t");
  Serial.print(Current.tempC);          Serial.print("\t");
  Serial.print(Current.dewpointC);      Serial.print("\t");
  Serial.print(Current.altSettingHG);   Serial.print("\t");
  Serial.print(Current.windspeedKTS);   Serial.print("\t");
  Serial.print(Current.winddirDeg);     Serial.print("\t");
  Serial.println(Current.visibilitySM);
}

void getForecast() {      // ================ GET FORECAST =============== //
  NOAAForecast();
  renderForecast();
}

void renderForecast() {   // ================ DISPLAY FORECAST =========== //
  double templ, temph;
  char stamp[50];
  sprintf(stamp, "%d:%02d%s", Forecast.hour, Forecast.minute, AMPM[Forecast.pm]);
  char p0[100];
  sprintf(p0,"%s %02d %s", Forecast.p0name,Forecast.p0tempC,Forecast.p0short);
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
  tft.print(stamp);
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
  Serial.print("Updated  "); Serial.println(stamp);
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

void WriteLED(int LTemp, int HTemp, int CTemp) { // ==== WRITE LEDS ======  //
  if (LTemp > 99) {
    LTempDisp.writeDigitNum(0, (LTemp / 100));
    LTempDisp.writeDigitNum(1, (LTemp / 10));
    LTempDisp.writeDigitNum(3, (LTemp % 10));
    LTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    LTempDisp.writeDigitNum(0, (LTemp / 10));
    LTempDisp.writeDigitNum(1, (LTemp % 10));
    LTempDisp.writeDigitRaw(3, B01110001);
    LTempDisp.writeDigitRaw(4, B00000000);
  }
  if (HTemp > 99) {
    HTempDisp.writeDigitNum(0, (HTemp / 100));
    HTempDisp.writeDigitNum(1, (HTemp / 10));
    HTempDisp.writeDigitNum(3, (HTemp % 10));
    HTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    HTempDisp.writeDigitNum(0, (HTemp / 10));
    HTempDisp.writeDigitNum(1, (HTemp % 10));
    HTempDisp.writeDigitRaw(3, B01110001);
    HTempDisp.writeDigitRaw(4, B00000000);
  }
  if (CTemp > 99) {
    CTempDisp.writeDigitNum(0, (CTemp / 100));
    CTempDisp.writeDigitNum(1, (CTemp / 10));
    CTempDisp.writeDigitNum(3, (CTemp % 10));
    CTempDisp.writeDigitRaw(4, B01110001);
  }
  else {
    CTempDisp.writeDigitNum(0, (CTemp / 10));
    CTempDisp.writeDigitNum(1, (CTemp % 10));
    CTempDisp.writeDigitRaw(3, B01110001);
    CTempDisp.writeDigitRaw(4, B00000000);
  }
  CTempDisp.writeDisplay();
  HTempDisp.writeDisplay();
  LTempDisp.writeDisplay();
}

void timeStamp(String timeStr, bool hour24_ts, int UTCoffset_ts,
               int *year_ts, int *month_ts, int *date_ts,
               int *hour_ts, bool *pm_ts, int *minute_ts) {
  int firstDash    = timeStr.indexOf("-");
  int secondDash   = timeStr.lastIndexOf("-");
  int firstT       = timeStr.indexOf("T");
  int firstColon   = timeStr.indexOf(":");
  int secondColon  = timeStr.lastIndexOf(":");

  String yearStr   = timeStr.substring(0, firstDash);
  String monthStr  = timeStr.substring(firstDash + 1);
  String dateStr   = timeStr.substring(secondDash + 1);
  String hourStr   = timeStr.substring(firstT + 1);
  String minuteStr = timeStr.substring(firstColon + 1);

  *year_ts     = yearStr.toInt();
  *month_ts    = monthStr.toInt();
  *minute_ts   = minuteStr.toInt();

  int date_utc = dateStr.toInt();
  int hour_utc = hourStr.toInt();
  hour_utc     += UTCOFFSET;                        // TZ offset adjust
  if (hour_utc < 0) { hour_utc += 24; date_utc -= 1; }
  else if (hour_utc > 23) { hour_utc -= 24; date_utc -= 1; }
  else if (hour_utc == 0) { hour_utc += 12; }

  if (!hour24_ts) {                                   // 12/24 hour adjust
    if (hour_utc >= 12) { hour_utc -= 12; *pm_ts = true; }
  }
  *hour_ts = hour_utc;
  *date_ts = date_utc;
}

void NOAAForecast() { // ================ FORECAST ================ //
  if (connectServer(FcstServer, FcstPort)) {
    if (sendRequest() && skipResponseHeaders()) parse();
    client.stop();
    return;
  }
}
bool connectServer(const char* SSLServer, int SSLPort) {
  bool ok = client.connectSSL(SSLServer, SSLPort);
  return ok;
}

bool sendRequest() {
  client.print("GET /points/");
  client.print(FCSTLON);
  client.print(",");
  client.print(FCSTLON);
  client.println("/forecast HTTP/1.1");
  client.println("Host: api.weather.gov");
  client.print("User-Agent: bob@bethanysciences.net/arduinowx01/");
  client.println("/forecast");
  client.println("Accept: application/ld+json");
  client.println("Connection: close");
  client.println();
}
bool skipResponseHeaders() {
  char endOfHeaders[] = "\r\n\r\n";
  client.setTimeout(HTTPTimeout);
  bool ok = client.find(endOfHeaders);
  return ok;
}
bool parse() {
  const size_t bufferSize = JSON_ARRAY_SIZE(14) +
              JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) +
              14*JSON_OBJECT_SIZE(13) + 8350;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);

  if (!root.success()) {
    Serial.println("Forecast parse fail");
    return false;
  }
  JsonObject& _context  = root["@context"];
   Forecast.generated   = root["generatedAt"];
  JsonArray& periods    = root["periods"];
  JsonObject& periods0  = periods[0];
   Forecast.p0name      = periods0["name"];
   Forecast.p0tempC      = periods0["temperature"];
   Forecast.p0icon      = periods0["icon"];
   Forecast.p0short     = periods0["shortForecast"];
  JsonObject& periods1  = periods[1];
   Forecast.p1name      = periods1["name"];
   Forecast.p1tempC      = periods1["temperature"];
   Forecast.p1icon      = periods1["icon"];
   Forecast.p1short     = periods1["shortForecast"];
  JsonObject& periods2  = periods[2];
   Forecast.p2name      = periods2["name"];
   Forecast.p2tempC      = periods2["temperature"];
   Forecast.p2icon      = periods2["icon"];
   Forecast.p2short     = periods2["shortForecast"];
  JsonObject& periods3  = periods[3];
   Forecast.p3name      = periods3["name"];
   Forecast.p3tempC      = periods3["temperature"];
   Forecast.p3icon      = periods3["icon"];
   Forecast.p3short     = periods3["shortForecast"];
  JsonObject& periods4  = periods[4];
   Forecast.p4name      = periods4["name"];
   Forecast.p4tempC      = periods4["temperature"];
   Forecast.p4icon      = periods4["icon"];
   Forecast.p4short     = periods4["shortForecast"];
  JsonObject& periods5  = periods[5];
   Forecast.p5name      = periods5["name"];
   Forecast.p5tempC     = periods5["temperature"];
   Forecast.p5icon      = periods5["icon"];
   Forecast.p5short     = periods5["shortForecast"];

  // ---- timeStamp parse
  String stamp(Forecast.generated);
  int firstDash    = stamp.indexOf("-");
  int secondDash   = stamp.lastIndexOf("-");
  int firstT       = stamp.indexOf("T");
  int firstColon   = stamp.indexOf(":");
  int secondColon  = stamp.lastIndexOf(":");
  String yearStr   = stamp.substring(0, firstDash);
  String monthStr  = stamp.substring(firstDash + 1);
  String dateStr   = stamp.substring(secondDash + 1);
  String hourStr   = stamp.substring(firstT + 1);
  String minuteStr = stamp.substring(firstColon + 1);
  Forecast.year    = yearStr.toInt();
  Forecast.month   = monthStr.toInt();
  Forecast.minute  = minuteStr.toInt();
  int date_utc = dateStr.toInt();
  int hour_utc = hourStr.toInt();
  hour_utc     += UTCOFFSET;                // TZ offset adjust
  if (hour_utc < 0) { 
    hour_utc += 24; 
    date_utc -= 1; 
  }
  else if (hour_utc > 23) { 
    hour_utc -= 24; 
    date_utc -= 1; 
  }
  else if (hour_utc == 0) hour_utc += 12;
  
  Forecast.pm = false;
  if (!TIME24) {                            // 12/24 hour adjust
    if (hour_utc >= 12) { hour_utc -= 12; Forecast.pm = true; }
  }
  Forecast.hour   = hour_utc;
  Forecast.date    = date_utc;
}

void NOAAmetar() {    // ================ CURRENT ================ //
  String request = currProduct + "httpparam?" +
                   "dataSource=" + currDataSource + "&" +
                   "requestType=" + currRequestType + "&" +
                   "format=" + currFormat + "&" +
                   "hoursBeforeNow=" + currHoursBeforeNow + "&" +
                   "stationString=KPDK";
  xml.get(request);
  // currCode = xml.responseStatusCode();
  int len = xml.contentLength();
  String response = xml.responseBody();
  Current.metar         = xmlTakeParam(response, "raw_text");
  Current.obstimeUTC    = xmlTakeParam(response, "observation_time");
  Current.tempC         = xmlTakeParam(response, "temp_c").toDouble();
  Current.dewpointC     = xmlTakeParam(response, "dewpoint_c").toDouble();
  Current.winddirDeg    = xmlTakeParam(response, "wind_dir_degrees").toInt();
  Current.windspeedKTS  = xmlTakeParam(response, "wind_speed_kt").toInt();
  Current.visibilitySM  = xmlTakeParam(response, "visibility_statute_mi").toInt();
  Current.altSettingHG  = xmlTakeParam(response, "altim_in_hg").toDouble();

  // ---- timeStamp parse
  int firstDash    = Current.obstimeUTC .indexOf("-");
  int secondDash   = Current.obstimeUTC .lastIndexOf("-");
  int firstT       = Current.obstimeUTC .indexOf("T");
  int firstColon   = Current.obstimeUTC .indexOf(":");
  int secondColon  = Current.obstimeUTC .lastIndexOf(":");

  String yearStr   = Current.obstimeUTC .substring(0, firstDash);
  String monthStr  = Current.obstimeUTC .substring(firstDash + 1);
  String dateStr   = Current.obstimeUTC .substring(secondDash + 1);
  String hourStr   = Current.obstimeUTC .substring(firstT + 1);
  String minuteStr = Current.obstimeUTC .substring(firstColon + 1);

  Current.year     = yearStr.toInt();
  Current.month    = monthStr.toInt();
  Current.minute   = minuteStr.toInt();
  int date_utc     = dateStr.toInt();
  int hour_utc     = hourStr.toInt();
  hour_utc        += UTCOFFSET;        // TZ offset adjust
  if (hour_utc < 0) { 
    hour_utc += 24; 
    date_utc -= 1; 
  }
  else if (hour_utc > 23) { 
    hour_utc -= 24; 
    date_utc -= 1; 
  }
  else if (hour_utc == 0) hour_utc += 12;
  
  Current.pm = false;
  if (!TIME24) {                       // 12/24 hour adjust
    if (hour_utc >= 12) { 
      hour_utc -= 12; 
      Current.pm = true; 
    }
  }
  Current.hour    = hour_utc;
  Current.date    = date_utc;
}

