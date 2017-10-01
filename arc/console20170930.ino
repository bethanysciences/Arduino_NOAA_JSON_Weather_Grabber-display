#include <SPI.h>
#include <SD.h>
#include "SDU.h"
#include "RTCZero.h"
#include "SimpleTimer.h"
#include "WiFi101.h"
#include "WiFiUdp.h"
#include "ArduinoJson.h"
#include "Adafruit_HX8357.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/FreeSans12pt7b.h"

RTCZero rtc;
SimpleTimer timer;
int UTCOffset       = -4;  // EDT 2nd sun mar | EST 1st sun NOV  
long UTCOffsetEpoch = UTCOffset * 3600;
char timeZone[]     = "EDT";
char ampm[2][3]     = {"am", "pm"};
#define Time24      false
#define MIN         60000         // 1 minute microseconds

const char ssid[]     = "iotworld";
const char pass[]     = "iotworld";
int status  = WL_IDLE_STATUS;

WiFiSSLClient client;
char server[]       = "api.weather.gov";
char locForecast[]  = "/points/33.8774,-84.3046";
char prodForecast[] = "/forecast";
char locCurrent[]   = "/stations/KPDK";
char prodCurrent[]  = "/observations/current";
char METARserver[]  = "w1.weather.gov";
char METARrequest[] = "/data/METAR/KPDK.1.txt";

const unsigned long HTTPTimeout = 10000;

#define TFT_CS    7                       // HX8357 WHT
#define TFT_DC    6                       // HX8357 GRN
#define TFT_RST   5                       // HX8357 ORG
#define TFT_MISO  5                       // HX8357 GRY
#define SD_CS     2                       // HX8357 WHT
#define FGROUND   HX8357_WHITE
#define BGROUND   HX8357_BLUE
#define GREEN     HX8357_GREEN
#define BLACK     HX8357_BLACK
#define RED       HX8357_RED
#define CYAN      HX8357_CYAN
#define MAGENTA   HX8357_MAGENTA
#define YELLOW    HX8357_YELLOW
int LineNu[80];                           // max number of lines
#define MARGIN    8                       // pixels
#define SLINE     16                      // base line height (pixels)
#define LINE      25                      // line height (pixels)
#define TAB       130                     // tab width (pixels)
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

Adafruit_7segment ClockDisp = Adafruit_7segment();
Adafruit_7segment hTempDisp = Adafruit_7segment();
Adafruit_7segment lTempDisp = Adafruit_7segment();
Adafruit_7segment cTempDisp = Adafruit_7segment();
#define Clock_I2C   0x75    // yellow
#define hTemp_I2C   0x71    // red
#define cTemp_I2C   0x72    // grn
#define lTemp_I2C   0x70    // blu
bool blinkColon = false;

void convertTime(int Hour24, int *Hour12, int *AorP) {
  *AorP = 0;                              // set as am
  *Hour12 = Hour24;
  if (!Time24) {                          // 24 hour = false
    if (*Hour12 >= 12) {
      *Hour12 -= 12;                      // 1300 > 100
      *AorP = 1;                          // pm
    }
  }
}
double c2f(double celsius) {
    return ((celsius * 9) / 5) + 32;
}
double pa2hg(double pascals) {
    return (pascals * 0.000295333727);
}
char *dtostrf (double v, signed char w, unsigned char p, char *s) {
  // convert floats to fixed string
  // v  Your float variable;
  // w  Length of the string that will be created INCL decimal pt
  // p  Number of digits after the deimal point to print;
  // s  Destination of output buffer
  char fmt[20];
  sprintf(fmt, "%%%d.%df", w, p);
  sprintf(s, fmt, v);
  return s;
}

void setup() {            // ================ SETUP ======================= //
  Serial.begin(115200);
  Serial.print("START UP");
  
  pinMode(LED_BUILTIN, OUTPUT);

  
                    // ---------------------- LEDS SETUP ----------------- //
  ClockDisp.begin(Clock_I2C);
  hTempDisp.begin(hTemp_I2C);
  lTempDisp.begin(lTemp_I2C);
  cTempDisp.begin(cTemp_I2C);


                   // ----------------------- TFT SETUP ------------------ //
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.setTextWrap(0);
  tft.fillScreen(BGROUND);

  int l = 0;
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
  
  
                    // ------------------- WIFI SETUP ------------------- //
  
  tft.print("connect to >");  tft.print(ssid);
  
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass); 
    tft.print(">");
    delay(500);
  }
  tft.print("> connected ");    tft.println();

  digitalWrite(LED_BUILTIN, HIGH);

                    // ---------------------- SET CLOCK ------------------ //
  rtc.begin();
  unsigned long epoch;

  tft.setFont(&FreeSans9pt7b);
  tft.setTextColor(FGROUND);
  tft.print("Time (epoch) ");       tft.println(epoch);
  
  while (epoch == 0) { 
    epoch = WiFi.getTime(); 
    tft.print("."); 
    delay(500); 
  }
  Serial.println();
  Serial.print(" NTP epoch acquired  "); Serial.println(epoch);

  epoch = epoch + UTCOffsetEpoch;

  Serial.print(" Epoch timezone adjusted "); Serial.println(epoch);
  
  rtc.setEpoch(epoch);
  
  Serial.print(" > time "); Serial.print(rtc.getHours()); 
  Serial.print(":"); Serial.println(rtc.getMinutes()); 


                    // ---------------------- SD SETUP -------------------- //
  if (!SD.begin(SD_CS)) {
    tft.setFont(&FreeSans9pt7b);
    tft.setTextColor(MAGENTA); 
    tft.println("SD card fail");
  }
//  bmpDraw("nra40.bmp", 0, 200);
  delay (1000);

                    // ---------------------- HEADER ---------------------- //
  Serial.print("pulled"); Serial.print("\t");
  Serial.print("update"); Serial.print("\t");
  Serial.print(" temp"); Serial.print("\t");
  Serial.print(" rHum"); Serial.print("\t");
  Serial.print(" htIx"); Serial.print("\t");
  Serial.print(" dwpt"); Serial.print("\t");
  Serial.print(" baro"); Serial.print("\t");
  Serial.print("  wind"); Serial.print("\t");
  Serial.println("conditions ");

                    // ---------------------- SET TIMERS ------------------ //
  cycleColon();   timer.setInterval(1000, cycleColon);
  renderClock();  timer.setInterval(MIN, renderClock);
//  statusLine();   timer.setInterval(MIN/10, statusLine);
//  getCurrent();   timer.setInterval(MIN * 10, getCurrent);
//  getForecast();  timer.setInterval(MIN * 60, getForecast);
}

void loop() {             // ================ LOOP ======================== //
  timer.run();
}
void cycleColon() {       // ================ COLON ======================= //
  blinkColon = !blinkColon;
  ClockDisp.drawColon(blinkColon);
  ClockDisp.writeDisplay();
}
void renderClock() {      // ================ CLOCK ======================= //
  int hour, AorP;
  convertTime(rtc.getHours(), &hour, &AorP);
  if ((hour / 10) > 0) {
    ClockDisp.writeDigitNum(0, (hour / 10));  // drop lead 0
  }
  else ClockDisp.writeDigitRaw(0, 0x0);
  ClockDisp.writeDigitNum(1, (hour % 10));
  ClockDisp.writeDigitNum(3, rtc.getMinutes() / 10);
  ClockDisp.writeDigitNum(4, rtc.getMinutes() % 10);
  ClockDisp.writeDisplay();
}

void getForecast() {      // ================ GET FORECAST ================ //
  fetchNOAA(server, locForecast, prodForecast);
}

void fetchNOAA(const char* server, const char* location, const char* product) {
  if (connectServer(server)) {
    if (sendRequest(location, product) && skipResponseHeaders()) {
      if (product == prodForecast) parseForecast();
      // if (product == prodCurrent) parseCurrent();
    }
    client.stop();
  }
}

bool connectServer(const char* hostName) { 
  bool ok = client.connectSSL(hostName, 443); 
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
  return ok;
}

bool parseForecast() {    // ================== FORECAST ================  //
  int firstl = 5;
  int lastl = 11;

                          // ---------------- Clear Area ----------------- //
  tft.fillRect(0, LineNu[firstl]-LINE, tft.width(), LineNu[lastl], BGROUND);

  const size_t bufferSize = JSON_ARRAY_SIZE(14) + 
              JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(7) + 
              14*JSON_OBJECT_SIZE(13) + 8350;              
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);
  
  if (!root.success()) {
    tft.setFont(&FreeSans9pt7b);
    tft.setTextColor(MAGENTA);
    tft.setCursor(MARGIN, LineNu[firstl]);
    tft.println("Forecast parse fail");
    return false;
  }

  JsonObject& _context = root["@context"];
   const char* generatedAt    = root["generatedAt"];
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

                          // ------------- ADJUST TIMEZONE  AM/PM -------- //
  int cfhour, AorP;
  fhour += UTCOffset;
  if (fhour < 0)        { fhour += 24; fday -= 1; }
  else if (fhour > 23)  { fhour -= 24; fday -= 1; }
  else if (fhour == 0)  { fhour += 12; }

  convertTime(fhour, &cfhour, &AorP);
  char agePretty[50];
  sprintf(agePretty, "%d:%02d%s", cfhour, fminute, ampm[AorP]);

  char per0[100]; sprintf(per0, "%s %02d %s", 
    periods0_name, periods0_temp, periods0_short);
  char per1[100]; sprintf(per1, "%s %02d %s", 
    periods1_name, periods1_temp, periods1_short);
  char per2[100]; sprintf(per2, "%s %02d %s", 
    periods2_name, periods2_temp, periods2_short);
  char per3[100]; sprintf(per3, "%s %02d %s", 
    periods3_name, periods3_temp, periods3_short);
  char per4[100]; sprintf(per4, "%s %02d %s", 
    periods4_name, periods4_temp, periods4_short);
  char per5[100]; sprintf(per5, "%s %02d %s", 
    periods5_name, periods5_temp, periods5_short);

                            // -------------- WRITE FORECAST -------------- //
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

                           // ------------------ WRITE LEDS --------------- //
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

                               // ------------------ DEBUG ---------------- //
  convertTime(rtc.getHours(), &cfhour, &AorP);  
  char TimeStamp[50];
  sprintf(TimeStamp, "%d:%02d%s", cfhour, rtc.getMinutes(), ampm[AorP]);

  Serial.print("Forecast pulled at ");  Serial.print(TimeStamp);  
  Serial.print(" and last updated at ");   Serial.println(agePretty); 
  
  Serial.print("   ");    Serial.println(per0); 
  Serial.print("   ");    Serial.println(per1); 
  Serial.print("   ");    Serial.println(per2); 
  Serial.print("   ");    Serial.println(per3); 
  Serial.print("   ");    Serial.println(per4); 
  Serial.print("   ");    Serial.println(per5);

  return true;
}

void statusLine() {       // ================== STATUS LINE =============  //
  int firstl = 12;
  int lastl = 12;
                          // ------------------ Clear Area --------------- //
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

#define BUFFPIXEL 20      // ================= DRAW BMP =================  //
void bmpDraw(char *filename, uint8_t x, uint16_t y) { 
  File     bmpFile;
  int      bmpWidth, bmpHeight;
  uint8_t  bmpDepth;
  uint32_t bmpImageoffset;
  uint32_t rowSize;
  uint8_t  sdbuffer[3*BUFFPIXEL];
  uint8_t  buffidx = sizeof(sdbuffer);
  boolean  goodBmp = false;
  boolean  flip    = true;
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  if((x >= tft.width()) || (y >= tft.height())) return;
  Serial.print("loading ");             
  Serial.println(filename);
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println("file not found");
    return;
  }
  if(read16(bmpFile) == 0x4D42) {
    Serial.print("file size:    ");     
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);
    bmpImageoffset = read32(bmpFile);
    Serial.print("image offset: ");     
    Serial.println(bmpImageoffset, DEC);
    Serial.print("header size:  ");     
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);  
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) {
      bmpDepth = read16(bmpFile);
      Serial.print("bit depth:  ");     
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) {
        goodBmp = true;
        Serial.print("image size: ");   
        Serial.println(bmpDepth);
        Serial.print(bmpWidth); 
        Serial.print('x'); 
        Serial.println(bmpHeight);
        rowSize = (bmpWidth * 3 + 3) & ~3;
        if(bmpHeight < 0) { 
          bmpHeight = -bmpHeight; flip = false; 
        }
        w = bmpWidth; h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;
        tft.setAddrWindow(x, y, x+w-1, y+h-1);
        for (row=0; row<h; row++) {
          if(flip) pos = bmpImageoffset + 
              (bmpHeight - 1 - row) * rowSize;
          else pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { 
            bmpFile.seek(pos); 
            buffidx = sizeof(sdbuffer); 
          }
          for (col=0; col<w; col++) {
            if (buffidx >= sizeof(sdbuffer)) {
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0;
            }
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          }
        }
        Serial.print("loaded in "); 
        Serial.print(millis() - startTime); 
        Serial.println(" ms");
      }
    }
  }
  bmpFile.close();
  if(!goodBmp) Serial.println("format not recognized");
}
uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}
uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

