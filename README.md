<link rel="stylesheet" href="modest.css">

Console Project  
======================  
Runs out over WiFi or GSM to NOAA to grab forecast and current conditions weather reports. Displays Hi/Low/Current temperatures on 7-segment LED displays. Syncs to NTP servers and displays local time zone adjusted time in 12 or 24 hour format.  

Designed to use (see tested integrations below)

For WiFi connections  
1. ATMEL SoC (System on Chip) [ATSAMW25](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42618-SmartConnect-ATSAMW25-MR210PB_Datasheet.pdf) ARM 32bit ultra low power WiFi equipped MCU's composed of three main blocks:  
   + [SAMD21 Cortex-M0+ 32bit low power (<20ma) ARM MCU](http://www.atmel.com/Images/Atmel-42181-SAM-D21_Summary.pdf)  
   + [WINC1500 low power 2.4GHz IEEE® 802.11 b/g/n Wi-Fi](http://www.microchip.com/wwwproducts/en/ATWINC1500)
   + [ECC508 CryptoAuthentication](http://www.atmel.com/Images/Atmel-8923S-CryptoAuth-ATECC508A-Datasheet-Summary.pdf)  
  + in production examples  
    - Arduino [MKR1000](https://www.arduino.cc/en/Guide/MKR1000)
    - Adafruit [Feather M0](https://www.adafruit.com/product/3010)

2. Integrated STMicroelectronics [STM32F205 120Mhz 32bit ARM Cortex M3 microcontroller](http://www.st.com/content/ccc/resource/technical/document/datasheet/bc/21/42/43/b0/f3/4d/d3/CD00237391.pdf/files/CD00237391.pdf/jcr:content/translations/en.CD00237391.pdf) and Cypress [BCM43362 Single-Chip IEEE 802.11TM b/g/n MAC/Baseband/Radio](http://www.cypress.com/file/297991/download)   
  + in production examples  
    Particle [Photon](https://www.particle.io/products/hardware/photon-wifi-dev-kit)

For LTE/GSM connections  
1. Integrated Atmel [SAMD21 Cortex-M0+ 32bit low power (<20ma) ARM MCU](http://www.atmel.com/Images/Atmel-42181-SAM-D21_Summary.pdf) and u-blox [SARA-U2 series HSPA /  GSM Module](https://www.u-blox.com/sites/default/files/SARA-U2_DataSheet_%28UBX-13005287%29.pdf)
  + in production examples  
    Arduino [MKR GSM 1400](https://store.arduino.cc/usa/mkr-gsm-1400)

2. Combined STM32F205 120Mhz 32bit ARM Cortex M3 microcontroller U-Blox SARA U260/U270 for 3G GSM  
  + in production examples   
  Particle [Electron](https://www.particle.io/products/hardware/electron-cellular-dev-kit)

Written in C++ for the Arduino, Particle and Atmel Studio 7 ecosystems  
<img src="img/logoArduino.svg" width="10%"> https://www.arduino.cc  

<img src="img/logoParticle.png" width="20%">
https://www.particle.io  

<img src="img/logoAtmel.svg" width="15%">
http://www.atmel.com/microsite/atmel-studio/


These projects are open source under the following Licenses
<img src="img/mit.png" width="10%">
<img src="img/opensource.png" width="10%">  

Resources
----------------------------------------------  
### NOAA CURRENT Using aviation METAR pull
- documentation http://www.aviationweather.gov/dataserver  
- syntax for METAR get with XML return http://www.aviationweather.gov/adds/dataserver_current/httpparam?dataSource=metars&requestType=retrieve&format=xml&stationString=KPDK&hoursBeforeNow=1
    - ```stationString``` find station and coordinates[here] (https://forecast-v3.weather.gov/point/33.845,-84.3775) e.g. Peachtree Dekaulb, Atlanta, GA, USA (KPDK) Atlanta, GA 30305 shown
    - ```hoursBeforeNow``` limits age

### NOAA FORECAST using v3 JASON-LD pull
- documentation https://forecast-v3.weather.gov/documentation  
- HTTP get example parameters
```
https://api.weather.gov/points/33.8774,-84.3046/forecast
User-Agent: bob@bethanysciences.net/arduinowx01
Accept: application/ld+json
```

### Weather Underground  
- documentation http://api.wunderground.com/weather/api
- example pulls conditions and forecast at airport Peachtree Dekaulb, Atlanta, GA, USA
```
http://api.wunderground.com/api/<KEY>/conditions/ forecast/q/PDK.json
```
```<KEY>``` required key see documentation to obtain and view pull limits


Components used and tested integrations
--------------------------------------
- [Arduino MKR1000 for WiFi](https://store.arduino.cc/usa/arduino-mkr1000)
  - Substitutes include [Adafruit Feather M0 WIFI](https://www.adafruit.com/product/3010) or other combinations
- IN PROCESS Particle Electron GSM [Particle Electron](https://www.particle.io/products/hardware/electron-cellular-dev-kit)
- [Adafruit HXD8357D 3.5" TFT 320x480 + Touchscreen](https://www.adafruit.com/product/2050) Breakout Board w/MicroSD Socket for graphical weather display
- [Adafruit HT16K33 I2C multiplexer-based 0.56" 4-Digit 7-Segment Display Red](https://www.adafruit.com/product/878) for high temperature
- [Adafruit HT16K33 I2C multiplexer-based 0.56" 4-Digit 7-Segment Display Green](https://www.adafruit.com/product/880) for current temperature
- [Adafruit HT16K33 I2C multiplexer-based 0.56" 4-Digit 7-Segment Display Blue](https://www.adafruit.com/product/881) for low temperature
- [Adafruit HT16K33 I2C multiplexer-based 1.2" 4-Digit 7-Segment Display Yellow](https://www.adafruit.com/product/1269) for time display
- Li-Po single cell, 3.7V, 700mAh minimum [example link](https://www.adafruit.com/product/258)

File and Library Structure
----------------------  
- console.cpp (or console.ino for Arduino IDE) code base for most functions.
- [WiFi101.h](www.arduino.cc/en/Reference/WiFi101) for integrated ATMEL ATSAMW25 wifi functions  
- [RTCZero.h](arduino.cc/en/Reference/RTC) for ATMEL SAMD21 time functions  
- [ArduinoHttpClient.h](https://github.com/arduino-libraries/ArduinoHttpClient) ATMEL ARM library used for HTTP (GET, POST, PUT, DELETE) web server requests and WebSocket servers message exchange.
    - Returns parsed status code and Content-Length header (if present).
    - Expects Client type object useable with any networking class
    - Derived from Adrian McEwen's HttpClient [library](https://github.com/amcewen/HttpClient)
- [ArduinoJson.h](https://bblanchon.github.io/ArduinoJson/) Benoît Blanchon's library supports JSON serialization and deserialization.
  - parse directly from an input ```Stream``` or ```std::istream```
  - serial port ```JsonObject& root = jsonBuffer.parse(Serial)```
  - Ethernet connection ```JsonObject& root = jsonBuffer.parse(ethernetClient)```
  - Wifi connection ```JsonObject& root = jsonBuffer.parse(wifiClient)```
- [SimpleTimer.h](playground.arduino.cc/Code/SimpleTimer) for timer interrupts to make scheduled function calls - for weather gets  
- [Adafruit_HX8357.h](github.com/adafruit/Adafruit_HX8357_Library) for the spi HX8357 TFT screen low level functions  
- [Adafruit_LEDBackpack.h](github.com/adafruit/Adafruit-LED-Backpack-Library) for the HX285 low level i2c 7-segment led functions  
- [Adafruit_GFX.h](github.com/adafruit/Adafruit-GFX-Library) for universal display driver functions  
  - Fonts/FreeSans9pt7b.h font included with Adafruit-GFX-Library  
  - Fonts/FreeSans12pt7b.h font included with Adafruit-GFX-Library   
- WiFiCreds.h for credentials designating WiFi network SSID and password parameters, alter for your particulars  
  ```
    /*-----------------------------------------------------*
    PRIVATE WIFI Credentials
    for ATMEL series 32bit SAMD21 CPUs
    © 2017 Bob Smith https://github.com/bethanysciences
    MIT license
    *------------------------------------------------------*/
    char ssid[] = "iotworld";     // your network SSID (name)
    char pass[] = "iotworld";     // your network password
  ```
- convertTime.h timezone and 12/24 hr conversion  
  ```convertTime(24hour, TIME24, &hour, &AorP)```
  - 24hour as INT input hour in 24 hour format  
  - TIME24 as BOOL output type (y=24 hour, n= 12 hour)  
  - returns hour as INT output hour converted to 12 or 24 hour  
  - returns AorP as AM or PM (if 12 hour selected) 0 = AM, 1 = PM  
- dtostrf.h Convert float to string as avr-libc sprintf does not format floats  
    ```char *dtostrf  (val, width, prec, char *sout)```
    - val  double / float variable  
    - width  string length returned INCLUDING decimal point + sign  
    - prec  number of digits after the deimal point to print  
    - sout  destination of output buffer (must be large enough)  
- wxConversions.h library of useful weather conversion functions
  - Celc > Fahr ```double c2f(double [temp °celcius])``` returns temperature in °fahrenheit as double  
  - Fahr > Celc ```double f2c(double [temp °fahrenheit``` returns temperature in °celcius
  - Humidity ```double rh(double [dew point °celc], double [temp °celc])``` returns % relative humidity as double
  - Windchill ```float wc(double [temp °celcius], int [MPH windspeed])``` returns windchill in °celcius as float
  - Convert Barometric Pressure as Pascals (PA) to Inches Mercury("HG)  
    ```double p2h(double pa)```
  - Dew Point in °Celsius or °Fahrenheit  
    ```double dp(double temp, int rh)```
    - temp = °Celsius or °Fahrenheit],
    - rh = % relative humidity
  - Calc Heat Index temperature °Celsius or °Fahrenheit  
    ```double hi(double temp, int rh, bool c)```
    - temp = °Celsius or °Fahrenheit
    - rh = % relative humidity
    - c = °Celsius yes/no
    - returns heat index temperature °Celsius or °Fahrenheit as double
  - Calc minutes to Min Erythemal Dose (MED) or sunburn   
    ```double med(int uvi, int alt, bool water, bool snow, int fitz, int spf)```
    - uvi = [UV Index](http://www.nws.noaa.gov/om/heat/uv.shtml)
    - alt = altitude in meters
    - water = on water y/n
    - snow = on snow y/n
    - fitz = [Fitzpatrick skin type (0-32)](https://www.ncbi.nlm.nih.gov/pubmed/20682135)
    - spf =  Sun Protection Factor value of applied sunblock
    - returns minutes to Min Erythemal Dose (MED) sunburn as integer
  - Parse and extract elements from XML   
    ```string xmlTakeParam(String inStr, String needParam)```
    - inStr input string e.g. ```<temp_c>30.6</temp_c>```
    - needParam parameter sought e.g. ```temp_c```
    - returns value in string e.g. ```30.6```
