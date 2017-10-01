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

String server     = "api.weather.gov";
int port          = 443;
String type       = "/points/"
String lat        = "33.8774"       // station KPDK
String lon        = "-84.3046";
String product    = "/forecast";

void NOAAForecast(String lat, Sting lon) {
  if (connectServer(server)) {
    if (sendRequest(lat, lon) && skipResponseHeaders()) parse();
    client.stop();
    return;
  }
}
bool connectServer() {
  bool ok = client.connectSSL(server, port);
  return ok;
}
bool sendRequest(String lat, String lon) {
  client.print("GET ");
  client.print(type);
  client.print(lat);
  client.print(",");
  client.print(lon);
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
   forecast.generated   = root["generatedAt"];
  JsonArray& periods    = root["periods"];
  JsonObject& periods0  = periods[0];
   forecast.p0name      = periods0["name"];
   forecast.p0temp      = periods0["temperature"];
   forecast.p0icon      = periods0["icon"];
   forecast.p0short     = periods0["shortForecast"];
  JsonObject& periods1  = periods[1];
   forecast.p1name      = periods1["name"];
   forecast.p1temp      = periods1["temperature"];
   forecast.p1icon      = periods1["icon"];
   forecast.p1short     = periods1["shortForecast"];
  JsonObject& periods2  = periods[2];
   forecast.p2name      = periods2["name"];
   forecast.p2temp      = periods2["temperature"];
   forecast.p2icon      = periods2["icon"];
   forecast.p2short     = periods2["shortForecast"];
  JsonObject& periods3  = periods[3];
   forecast.p3name      = periods3["name"];
   forecast.p3temp      = periods3["temperature"];
   forecast.p3icon      = periods3["icon"];
   forecast.p3short     = periods3["shortForecast"];
  JsonObject& periods4  = periods[4];
   forecast.p4name      = periods4["name"];
   forecast.p4temp      = periods4["temperature"];
   forecast.p4icon      = periods4["icon"];
   forecast.p4short     = periods4["shortForecast"];
  JsonObject& periods5  = periods[5];
   forecast.p5name      = periods5["name"];
   forecast.p4temp      = periods5["temperature"];
   forecast.p4icon      = periods5["icon"];
   forecast.p4short     = periods5["shortForecast"];
  timeStamp(String timeStamp, bool hour24TS, int UTCoffsetTS,
            int &yearTS, int &monthTS, int &dateTS,
            int &hourTS, bool &pmTS, int &minuteTS)
   forecast.year        = yearTS;
   forecast.month       = monthTS;
   forecast.date        = dateTS;
   forecast.hour        = hourTS;
   forecast.minute      = minuteTS;
   forecast.pm          = pmTS;
}
