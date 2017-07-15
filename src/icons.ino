#include "SPI.h"
#include "RTCZero.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "Fonts/FreeSansBold18pt7b.h"
#include "cloudy.h"                           // 100x90
#include "clr.h"                      
#include "partlycloudy.h"
#include "rain.h"
#include "snow.h"
#include "thunderstorm.h"
#include "warning.h"
#include "windy.h"
#define ICON_WIDTH          100
#define ICON_HEIGHT         90

const char conditions[8] = {'cloudy', 'clr', 'partlycloudy', 'rain', 'snow', 'thunderstorm', 'warning', 'windy'};
int i = 0;


#define TFT_DC 9
#define TFT_CS 10
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);    // 240x320
#define SD_CS 4
#define BUFFPIXEL 20
#define SCREEN_BACK         ILI9341_BLACK
#define SCREEN_TEXT         ILI9341_WHITE
#define SCREEN_TEXT_RVSE    ILI9341_BLACK
#define SCREEN_BACK_RVSE    ILI9341_WHITE

RTCZero rtc;
const byte seconds = 0;
const byte minutes = 0;
const byte hours = 16;
const byte day = 15;
const byte month = 6;
const byte year = 15;

void setup() {
    rtc.begin();
    rtc.setTime(hours, minutes, seconds);
    rtc.setDate(day, month, year);
    tft.begin();
    tft.fillScreen(SCREEN_BACK);
    tft.setRotation(3);
    DrawScreen();
}

void loop(void) {
    tft.setTextColor(SCREEN_BACK, SCREEN_TEXT); tft.setCursor(125, 18);
    char tStamp[8]; sprintf(tStamp, "%02d:%02d:%02d", rtc.getHours(),rtc.getMinutes(),rtc.getSeconds());
    tft.print(tStamp);

    tft.setTextColor(SCREEN_BACK_RVSE, SCREEN_TEXT_RVSE); tft.setCursor(125, 81);
    char dStamp[10]; sprintf(dStamp, "%02d/%02d/%02d", rtc.getDay(),rtc.getMonth(),rtc.getYear());
    tft.print(dStamp);

    DrawWx(i);

    delay(3000);
    if (i<9) i++ ;
    else i = 0;
}

void DrawWx(int i) {
    tft.setTextColor(SCREEN_BACK_RVSE, SCREEN_TEXT_RVSE); tft.setCursor(0, 150);
    switch (i) {
        case 0:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)cloudy);
            tft.println("Cloudy    ");
            tft.print  ("          ");
            break;
        case 1:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)clr);
            tft.println("Clear     ");
            tft.print  ("          ");
            break;
        case 2:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)partlycloudy);
            tft.println("Partly    ");
            tft.print  ("Cloudy    ");
            break;
        case 3:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)rain);
            tft.println("Rain Heavy");
            tft.print  ("At Times  ");
            break;
        case 4:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)snow);
            tft.println("Snow      ");
            tft.print  ("          ");
            break;
        case 5:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)thunderstorm);
            tft.println("Thunder   ");
            tft.print  ("Storms    ");
            break;
        case 6:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)warning);
            tft.println("ALERT 1  ");
            tft.print  ("WARNING  ");
            break;
        case 7:
            tft.drawBitmap(220, 150, ICON_WIDTH, ICON_HEIGHT, (uint16_t*)windy);
            tft.println("Windy    ");
            tft.print  ("         ");
            break;
        default:  break;
    }
}
    
void DrawScreen () {
    tft.setTextColor(SCREEN_TEXT); 
    tft.setFont(&FreeSansBold18pt7b); 
    tft.drawRoundRect(10, 5, 100, 40, 4, SCREEN_TEXT);
    tft.setTextColor(SCREEN_TEXT);
    tft.setCursor(18, 38);
    tft.print("Time");
    tft.drawRoundRect(120, 70, 190, 40, 4, SCREEN_TEXT);
        
    tft.fillRoundRect(10, 70, 100, 40, 4, SCREEN_BACK_RVSE);
    tft.setTextColor(SCREEN_TEXT_RVSE); 
    tft.setCursor(18, 100);
    tft.print("Date");
    tft.fillRoundRect(120, 5, 190, 40, 4, SCREEN_BACK_RVSE);

    tft.setFont();
    tft.setTextSize(3);
}

