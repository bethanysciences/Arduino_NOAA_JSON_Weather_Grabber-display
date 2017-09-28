#include "ArduinoJson.h"
#include "WiFi101.h"
#include "WiFi101OTA.h"
#include "WiFiUdp.h"
#include "RTCZero.h"
#include "SimpleTimer.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Adafruit_LEDBackpack.h"

Adafruit_7segment ClockDisp = Adafruit_7segment();
Adafruit_7segment hTempDisp = Adafruit_7segment();
Adafruit_7segment lTempDisp = Adafruit_7segment();
Adafruit_7segment cTempDisp = Adafruit_7segment();
#define Clock_I2C   0x73
#define hTemp_I2C   0x72
#define lTemp_I2C   0x71
#define cTemp_I2C   0x70
bool blinkColon = false;

#define TFT_CS  9
#define TFT_DC  10
#define SD_CS   5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);    // 240x320 TFT
#define FGROUND ILI9341_WHITE
#define BGROUND ILI9341_BLUE

char ssid[] = "iotworld";
char pass[] = "iotworld";
int status  = WL_IDLE_STATUS;

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
double c2f(double celsius) {                      // convert Celcius to Fahrenheit
    return ((celsius * 9) / 5) + 32;
}
double pa2hg(double pascals) {                    // convert Pascals to in. mercury
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
  WiFi.setPins(8,7,4,2);          // for Adafruit Feather M0 WiFi
  while (!Serial);

  ClockDisp.begin(Clock_I2C);
  hTempDisp.begin(hTemp_I2C);
  lTempDisp.begin(lTemp_I2C);
  cTempDisp.begin(cTemp_I2C);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(BGROUND);
  tft.fillRect(0, 230, 320, 240, ILI9341_ORANGE);

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    Serial.print(">");
    delay(500);
  }
  Serial.println();
  WiFiOTA.begin("Arduino", "password", InternalStorage);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Wifi Connected");
  rtc.begin();
  unsigned long epoch;
  Serial.print("epoch ");   Serial.println(epoch);
  while (epoch == 0) {
    epoch = WiFi.getTime();
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("post epoch ");   Serial.println(epoch);
  rtc.setEpoch(epoch);

  tft.fillRect(0, 230, 320, 240, ILI9341_GREEN);
  tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);

  statusLine();
  getForecast();
  getCurrent();
  timer.setInterval(1000*10, statusLine);
  timer.setInterval(1000*60*60*10, getForecast);    // 10 mins
  timer.setInterval(1000*60*60*10, getCurrent);     // 10 mins
}

void loop() {
  WiFiOTA.poll();
  timer.run();
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);
  int displayVal = (locHour * 100) + rtc.getMinutes();
  ClockDisp.print(displayVal);
  blinkColon = !blinkColon;
  ClockDisp.drawColon(blinkColon);
  ClockDisp.writeDisplay();

  tft.setTextColor(FGROUND, BGROUND);
  tft.setTextSize(3);
  tft.setTextColor(FGROUND, BGROUND);

  tft.setCursor(0,0);
  char tStamp[10];
  sprintf(tStamp, "%02d:%02d:%02d%s", locHour,
                              rtc.getMinutes(),
                              rtc.getSeconds(),
                              AorP);
  tft.setCursor(0,25);
  tft.print(tStamp);

  char dStamp[10];
  sprintf(dStamp, "%02d/%02d/%02d", rtc.getMonth(),
                                      locDay,
                                      rtc.getYear());
  tft.setCursor(0,50);
  tft.print(dStamp);
  delay(1000);
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
    Serial.println("Disconnect");
    Serial.println("-----------------");
    client.stop();
  }
}
bool connect(const char* hostName) {
  bool ok = client.connectSSL(hostName, 443);
  Serial.println(ok ? "Connect" : "Connect Failed!");
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
  if (!ok) Serial.println("No response or invalid response!");
  return ok;
}

bool parseForecast() {
  const size_t bufferSize = JSON_ARRAY_SIZE(14) +
              JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) +
              14*JSON_OBJECT_SIZE(13) + 8350;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);

  if (!root.success()) {
    Serial.println("Forecast parse fail");
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
  JsonObject& periods6        = periods[6];
   const char* periods6_name  = periods6["name"];
   int periods6_temp          = periods6["temperature"];
   const char* periods6_icon  = periods6["icon"];
   const char* periods6_short = periods6["shortForecast"];
  JsonObject& periods7        = periods[7];
   const char* periods7_name  = periods7["name"];
   int periods7_temp          = periods7["temperature"];
   const char* periods7_icon  = periods7["icon"];
   const char* periods7_short = periods7["shortForecast"];
  JsonObject& periods8        = periods[8];
   const char* periods8_name  = periods8["name"];
   int periods8_temp          = periods8["temperature"];
   const char* periods8_icon  = periods8["icon"];
   const char* periods8_short = periods8["shortForecast"];
  JsonObject& periods9        = periods[9];
   const char* periods9_name  = periods9["name"];
   int periods9_temp          = periods9["temperature"];
   const char* periods9_icon  = periods9["icon"];
   const char* periods9_short = periods9["shortForecast"];

  String ageString  = String(generatedAt);
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

  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);

  char agePretty[50];
  char fetchedAt[50];
  char per0[100];
  char per1[100];
  char per2[100];
  char per3[100];
  char per4[100];
  char per5[100];
  char per6[100];
  char per7[100];
  char per8[100];
  char per9[100];

  sprintf(agePretty, "Forecast issued: %02d/%02d/%04d %02d:%02d%s %s",
    fmonth, flocDay, fyear, flocHour, fminute, fAorP, timeZone);


  sprintf(fetchedAt, "Fetched at:      %02d/%02d/%04d %02d:%02d%s %s",
    rtc.getMonth(), locDay, rtc.getYear(), locHour, rtc.getMinutes(), AorP, timeZone);

  sprintf(per0, "%s %02d %s", periods0_name, periods0_temp, periods0_short);
  sprintf(per1, "%s %02d %s", periods1_name, periods1_temp, periods1_short);
  sprintf(per2, "%s %02d %s", periods2_name, periods2_temp, periods2_short);
  sprintf(per3, "%s %02d %s", periods3_name, periods3_temp, periods3_short);
  sprintf(per4, "%s %02d %s", periods4_name, periods4_temp, periods4_short);
  sprintf(per5, "%s %02d %s", periods5_name, periods5_temp, periods5_short);
  sprintf(per6, "%s %02d %s", periods6_name, periods6_temp, periods6_short);
  sprintf(per7, "%s %02d %s", periods7_name, periods7_temp, periods7_short);
  sprintf(per8, "%s %02d %s", periods8_name, periods8_temp, periods8_short);
  sprintf(per9, "%s %02d %s", periods9_name, periods9_temp, periods9_short);

  Serial.println(per0);
  Serial.print("Icon "); Serial.println(periods0_icon);
  Serial.println(per1);
  Serial.print("Icon "); Serial.println(periods1_icon);
  Serial.println(per2);
  Serial.print("Icon "); Serial.println(periods2_icon);
  Serial.println(per3);
  Serial.print("Icon "); Serial.println(periods3_icon);
  Serial.println(per4);
  Serial.print("Icon "); Serial.println(periods4_icon);
  Serial.println(per5);
  Serial.print("Icon "); Serial.println(periods5_icon);
  Serial.println(per6);
  Serial.print("Icon "); Serial.println(periods6_icon);
  Serial.println(per7);
  Serial.print("Icon "); Serial.println(periods7_icon);
  Serial.println(per8);
  Serial.print("Icon "); Serial.println(periods8_icon);
  Serial.println(per9);
  Serial.print("Icon "); Serial.println(periods9_icon);
  Serial.println(generatedAt);
  Serial.print("Icon "); Serial.println(periods0_icon);
  Serial.println(agePretty);
  Serial.println(fetchedAt);

  int templ, temph;
  if (periods0_temp < periods1_temp) {
    templ = periods0_temp;
    temph = periods1_temp;
  }
  if (periods0_temp > periods1_temp) {
    templ = periods1_temp;
    temph = periods0_temp;
  }
  hTempDisp.print(templ);
  hTempDisp.writeDisplay();
  lTempDisp.print(temph);
  lTempDisp.writeDisplay();

  return true;
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
  float temp  = temperature["value"];
  float dwpt  = dewpoint["value"];
  int wDir    = windDirection["value"];
  float wind  = windSpeed["value"];
  long baro   = barometricPressure["value"];
  long sLvl   = seaLevelPressure["value"];
  float rHum  = relativeHumidity["value"];
  float htIx  = heatIndex["value"];

  String ageString = String(timestamp);
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

  int clocHour, clocDay, cAorP;
  convertTime(chour, cday, &clocHour, &clocDay, &cAorP);

  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);

  char agePretty[50];
  sprintf(agePretty, "Current issued: %02d/%02d/%04d %02d:%02d%s %s",
    cmonth, clocDay, cyear, clocHour, cminute, cAorP, timeZone);

  char fetchedAt[50];
  sprintf(fetchedAt, "Fetched at:     %02d/%02d/%04d %02d:%02d%s %s",
    rtc.getMonth(), locDay, rtc.getYear(), locHour, rtc.getMinutes(), AorP, timeZone);

  temp = c2f(temp);
  dwpt = c2f(dwpt);
  htIx = c2f(htIx);

  char temp_d[3]; dtostrf(temp, 3, 0, temp_d);
  char dwpt_d[3]; dtostrf(dwpt, 3, 0, dwpt_d);
  char wind_d[3]; dtostrf(wind, 3, 0, wind_d);
  char baro_d[6]; dtostrf(baro/10, 3, 0, baro_d);
  char sLvl_d[6]; dtostrf(sLvl/10, 3, 0, sLvl_d);
  char rHum_d[6]; dtostrf(rHum, 3, 0, rHum_d);
  char htIx_d[6]; dtostrf(htIx, 3, 0, htIx_d);
  char curr0[100];
  sprintf(curr0, "Current: %s%sF  Dew Point%sF  Humidity%s%%  Heat Index%sF",
                            text, temp_d, dwpt_d, rHum_d, htIx_d);
  char curr1[100];
  sprintf(curr1, "Wind%smph at %idegrees  Barometer %smb at sea level setting %smb",
                            wind_d, wDir, baro_d, sLvl_d);
  Serial.println(curr0);
  Serial.println(curr1);
  Serial.print("Icon "); Serial.println(icon);
  Serial.println(timestamp);
  Serial.println(agePretty);
  Serial.println(fetchedAt);

  cTempDisp.writeDigitAscii(0, (temp / 10 + 48 ));
  cTempDisp.writeDigitAscii(1, (temp % 10 + 48));
  cTempDisp.writeDigitAscii(2, 'F');
  cTempDisp.writeDigitAscii(3, ' ');
  cTempDisp.writeDisplay();


  return true;
}

void statusLine() {
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay, &AorP);
  Serial.print("StatusLine: ssid ");
  char stat[80];
  IPAddress ip = WiFi.localIP();
  sprintf(stat, "%s %d.%d.%d.%d %ddBm set %02d/%02d/%02d %d:%02d:%02d%s EDT",
                            WiFi.SSID(),
                            ip[0], ip[1], ip[2], ip[3],
                            WiFi.RSSI(),
                            rtc.getMonth(),
                            locDay,
                            rtc.getYear(),
                            locHour,
                            rtc.getMinutes(),
                            rtc.getSeconds(),
                            ampm[AorP]);
  tft.setCursor(10,232);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);
  Serial.println(stat);
  tft.print(stat);
}
