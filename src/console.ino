#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include "WiFi101/WiFi101.h"
#include "WiFi101/WiFiUdp.h"
#include "RTCZero/RTCZero.h"
#include "Adafruit_GFX/Adafruit_GFX.h"
#include "Adafruit_ILI9341/Adafruit_ILI9341.h"

RTCZero rtc;

int tzOffset = -4;                                          // Time zone -4 EDY
#define TIME_24_HOUR      false

#define TFT_CS  9
#define TFT_DC  10
#define SD_CS   5
#define FGROUND ILI9341_WHITE
#define BGROUND ILI9341_BLUE

char ssid[] = "iotworld";
char pass[] = "iotworld";
int status = WL_IDLE_STATUS;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);    // 240x320 TFT 

void setup() {
    Serial.begin(115200);
    WiFi.setPins(8,7,4,2);
//    while (!Serial) { ; }
    
    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(BGROUND);

    rtc.begin();

    while ( status != WL_CONNECTED) {
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    // setNTPTime();
    WifiConfig();
}

void loop() {
    int thours = rtc.getHours();
    int tday = rtc.getDay();
    const char* ampm = "am";
    thours = thours + tzOffset;                 // time zone adustment
    if (thours < 0) {
        thours += 24;
        tday -= 1;
    }
    else if (thours > 23) {
        thours -= 24;
        tday -= 1;
    }
    if (thours >= 12) ampm = "pm";    
    if (!TIME_24_HOUR) {                        // 24hr adjustment
        if (thours > 12) thours -= 12;
        else if (thours == 0) {                 // hour 0 (midnight) show as 12
            thours += 12;
        }
    }
    tft.setTextColor(FGROUND, BGROUND);
    tft.setTextSize(3);
    tft.setCursor(0,0);
    tft.setTextColor(FGROUND, BGROUND);    
    char tStamp[10];
    sprintf(tStamp, "%02d:%02d:%02d%s", thours, 
                                        rtc.getMinutes(),
                                        rtc.getSeconds(),
                                        ampm);
    tft.print(tStamp);
    char dStamp[10];
    sprintf(dStamp, "%02d/%02d/%02d", rtc.getMonth(), 
                                      tday,
                                      rtc.getYear());
    tft.setCursor(0,25);
    tft.print(dStamp);
    // WifiConfig();
}

void setNTPTime() {
    unsigned long epoch;
    int numberOfTries = 0, maxTries = 6;
    do {
        epoch = WiFi.getTime();
        numberOfTries++;
    }
    while ((epoch == 0) || (numberOfTries > maxTries));
    if (numberOfTries > maxTries) {
        Serial.print("NTP unreachable");
        while (1);
    }
    else {
        Serial.print("Epoch "); Serial.println(epoch);
        rtc.setEpoch(epoch);
    }
}

void WifiConfig() {
    char wifistatus[80];
  //  sprintf(wifistatus, "SSID %s | address  %s | %s signal  dBm", WiFi.SSID(),        // 
  //                                  WiFi.localIP(),     // IPAddress ip = WiFi.localIP()
  //                                  WiFi.RSSI());       // long rssi = WiFi.RSSI();   
    sprintf(wifistatus, "SSID %s", WiFi.SSID());
    tft.setCursor(10,232);
    tft.fillRect(0, 230, 320, 240, ILI9341_GREEN);
    tft.setTextSize(1);
    tft.setTextColor(ILI9341_BLACK, ILI9341_GREEN);    
    tft.print(wifistatus);
}

