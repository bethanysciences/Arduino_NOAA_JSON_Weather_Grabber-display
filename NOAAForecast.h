/*----------------------------------------------------------------------*
  Fetch NOAA Forecasts and return JSON-LD

  // *** assumes WiFi connection up and running ***

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

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

#include <ArduinoJson.h>
#include "timeStamp.h"
#include "WXdefines.h"

String fcstServer  = "api.weather.gov";
int    fcstPort    = 443;
String fcstType    = "/points/";
String fcstProduct = "/forecast";
String fcstAgent   = "User-Agent: bob@bethanysciences.net/arduinowx01/";

void NOAAForecast() {
  if (connectServer(server)) {
    if (sendRequest() && skipResponseHeaders()) parse();
    client.stop();
    return;
  }
}
bool connectServer() {
  bool ok = client.connectSSL(FcstServer, FcstPort);
  return ok;
}
bool sendRequest() {
  client.print("GET ");
  client.print(type);
  client.print(FCSTLON);
  client.print(",");
  client.print(FCSTLON);
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
   Forecast.p0temp      = periods0["temperature"];
   Forecast.p0icon      = periods0["icon"];
   Forecast.p0short     = periods0["shortForecast"];
  JsonObject& periods1  = periods[1];
   Forecast.p1name      = periods1["name"];
   Forecast.p1temp      = periods1["temperature"];
   Forecast.p1icon      = periods1["icon"];
   forecast.p1short     = periods1["shortForecast"];
  JsonObject& periods2  = periods[2];
   Forecast.p2name      = periods2["name"];
   Forecast.p2temp      = periods2["temperature"];
   Forecast.p2icon      = periods2["icon"];
   Forecast.p2short     = periods2["shortForecast"];
  JsonObject& periods3  = periods[3];
   Forecast.p3name      = periods3["name"];
   forecast.p3temp      = periods3["temperature"];
   forecast.p3icon      = periods3["icon"];
   forecast.p3short     = periods3["shortForecast"];
  JsonObject& periods4  = periods[4];
   Forecast.p4name      = periods4["name"];
   Forecast.p4temp      = periods4["temperature"];
   Forecast.p4icon      = periods4["icon"];
   Forecast.p4short     = periods4["shortForecast"];
  JsonObject& periods5  = periods[5];
   Forecast.p5name      = periods5["name"];
   Forecast.p4temp      = periods5["temperature"];
   Forecast.p4icon      = periods5["icon"];
   Forecast.p4short     = periods5["shortForecast"];
  timeStamp(String timeStamp, bool hour24TS, int UTCoffsetTS,
            int &yearTS, int &monthTS, int &dateTS,
            int &hourTS, bool &pmTS, int &minuteTS)
   Forecast.year        = yearTS;
   Forecast.month       = monthTS;
   Forecast.date        = dateTS;
   Forecast.hour        = hourTS;
   Forecast.minute      = minuteTS;
   Forecast.pm          = pmTS;
}
