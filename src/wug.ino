#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include "WiFi101.h"
#include "WiFiUdp.h"
#include "TimeLib.h"
#include "ArduinoJson.h"

const char ssid[]   = "pizgloria";
const char pass[]   = "HomeCall4";
char tStamp[10];

#define TFT_CS 7                    // WHT
#define TFT_DC 6                    // GRN
#define TFT_RST 5                   // ORG
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

unsigned int localPort = 2390;
IPAddress timeServer(129, 6, 15, 28);
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE ];
WiFiUDP Udp;
WiFiClient client;

const int timeZone = -4;

#define WUhost "api.wunderground.com"
#define WUkey  "712a91952a8b182e"
#define WUloc  "PDK"

char LastForecastFetch[30];
char LastConditionsFetch[30];
char ForecastDay0[30];
char TempHiLo[30];
char tempStr[30];
char LtempStr[30];
char presStr[30];
char humStr[30];
char windStr[30];
char conditions[30];
char condStr[30];
char NextFetch[20];
int fetchInt = 600;

void setup() {
  tft.begin(HX8357D);
  tft.fillScreen(HX8357_BLUE);
  tft.setRotation(3);
  tft.setCursor(0,15);
  tft.setTextColor(HX8357_WHITE, HX8357_BLUE); 
  if (WiFi.status() == WL_NO_SHIELD) { tft.println("No Shield"); while (true);}
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) { delay(8000); }
  printWifiStatus();
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  delay(10000);
  sprintf(tStamp, "%02d:%02d:%02d", hour(),minute(),second());
  tft.print("Started at "); tft.println(tStamp);
  tft.drawLine(20, 65, 460, 65, HX8357_WHITE);
  tft.setFont();
  tft.setTextSize(2);
}

static char respBuf[8192];

void loop() {
  tft.setCursor(40,75);
  sprintf(tStamp, "%02d:%02d:%02d", hour(),minute(),second());
  tft.print(tStamp);
  fetchWU("forecast");
  delay(5000);
  fetchWU("conditions");
  for (int i = fetchInt ; i > 0 ; i--){ 
    sprintf(NextFetch, "Next in %03d Secs", i);
    tft.setCursor(225,75);
    tft.print(NextFetch);
    delay(1000);
  }
}

void fetchWU(char *WUtype) {
  client.stop();
  delay(100);
  if (!client.connect(WUhost, 80)) { return; }
  client.print("GET /api/");
  client.print(WUkey);
  client.print("/");
  client.print(WUtype);
  client.print("/q/");
  client.print(WUloc);
  client.println(".json HTTP/1.1");
  client.print("Host: "); 
  client.println(WUhost);
  client.println("Connection: close");
  client.println();
  client.flush();
  int respLen = 0;
  bool skip_headers = true;
  while (client.connected() || client.available()) {
    if (skip_headers) { 
      String aLine = client.readStringUntil('\n'); 
      if (aLine.length() <= 1) { skip_headers = false; } 
    }
    else {
      int bytesIn;
      bytesIn = client.read((uint8_t *)&respBuf[respLen], sizeof(respBuf) - respLen);
      if (bytesIn > 0) {  respLen += bytesIn; if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf); }
      else if (bytesIn < 0) { }
    }
    delay(1);
  }
  client.stop();
  if (respLen >= sizeof(respBuf)) { return; }
  respBuf[respLen++] = '\0';
  if (WUtype == "conditions") {
    parseConditions(respBuf);
    if (true) { return; }
    if (false) { tft.println("Parse fail -conditions > \r\n"); return; }
  }
  if (WUtype == "forecast") {
    parseForecast(respBuf);
    if (true) { return; }
    if (false) { tft.println("Parse fail -forecast> \r\n"); return; }
  }   
  return; 
}

bool parseForecast(char *json) {
  StaticJsonBuffer<8192> jsonBuffer;
  char *jsonstart = strchr(json, '{');  
  if (jsonstart == NULL) { return false; }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json);
  JsonObject& current = root["forecast"]["simpleforecast"];
  if (!root.success()) { return false; }
  int DateDa = current["forecastday"][0]["date"]["day"];
  int DateMo = current["forecastday"][0]["date"]["month"];
  int DateYr = current["forecastday"][0]["date"]["year"];
  int Htemp =  current["forecastday"][0]["high"]["fahrenheit"];
  int Ltemp =  current["forecastday"][0]["low"]["fahrenheit"];
  sprintf(ForecastDay0, "Forecast for %02d/%02d/%02d at %02d:%02d", DateMo, DateDa, DateYr, hour(),minute());
  sprintf(TempHiLo, "Temps  High %02d | Low %02d", Htemp,Ltemp);
  tft.drawLine(20, 95, 460, 95, HX8357_WHITE);
  tft.setCursor(40,100);
  tft.print(ForecastDay0);
  tft.setCursor(40,120);
  tft.print(TempHiLo);
      
  return true;
}

bool parseConditions(char *json) {
  StaticJsonBuffer<8192> jsonBuffer;
  char *jsonstart = strchr(json, '{');
  if (jsonstart == NULL) { return false; }
  json = jsonstart;
  JsonObject& root = jsonBuffer.parseObject(json);
  JsonObject& current = root["current_observation"];
  if (!root.success()) { return false; }
  double temp =          current["temp_f"];
  const char *cond =     current["weather"];
  int visi =             current["visibility_mi"];
  const char *UV =       current["UV"];
  const char *humi =     current["relative_humidity"];
  const char *pres_in =  current["pressure_in"];
  const char *trend =    current["pressure_trend"];
  double wind_mph =      current["wind_mph"];
  int wind_mphR =        round(wind_mph);
  int wind_head =        current["wind_degrees"];
  const char *wind_dir = current["wind_dir"];
  int tempR = round(temp);
  sprintf(condStr, "Conditions at %02d:%02d %s", hour(), minute(), cond);
  sprintf(presStr, "Barometer %sinHG  Trending %s  |  Visibility %imi  | UV Index %s", pres_in, trend, cond, visi, UV);
  sprintf(humStr,  "Humidity %s  |  Wind %i MpH Direction %s", humi, wind_mphR, wind_dir);

  tft.drawLine(20, 185, 460, 185, HX8357_WHITE);
  tft.setCursor(40,190); 
  tft.print(condStr);
  tft.setCursor(40,195); 
  tft.println(presStr);
  tft.setCursor(40,205); 
  tft.println(humStr);
  return true;
}

time_t prevDisplay = 0;

time_t getNtpTime() {
  while (Udp.parsePacket() > 0);
  sendNTPpacket(timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Udp.read(packetBuffer, NTP_PACKET_SIZE);
      unsigned long secsSince1900;
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  return 0;
}

void sendNTPpacket(IPAddress &address) {
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  packetBuffer[0]   = 0b11100011;
  packetBuffer[1]   = 0;
  packetBuffer[2]   = 6;
  packetBuffer[3]   = 0xEC;
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;              
  Udp.beginPacket(address, 123);
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

void printWifiStatus() {
  IPAddress ip = WiFi.localIP(); long rssi = WiFi.RSSI();
  tft.print("SSID: "); tft.print(WiFi.SSID());
  tft.print(" @ IP Address: "); tft.println(ip);
  tft.print("signal strength (RSSI):"); tft.print(rssi); tft.println(" dBm");
}

