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

#include "ArduinoJson.h"

String fcstServer  = "api.weather.gov";
int    fcstPort    = 443;
String fcstType    = "/points/";
String fcstProduct = "/forecast";
String fcstAgent   = "User-Agent: bob@bethanysciences.net/arduinowx01/";
struct f_cast {
  char generated[22];
  char p0[50];
  char p1[50];
  char p3[50];
  char p4[50];
  char p5[50];
  char p0name[50];
  char p1name[50];
  char p2name[50];
  char p3name[50];
  char p4name[50];
  char p5name[50];
double p0tempC;
double p1tempC;
double p2tempC;
double p3tempC;
double p4tempC;
double p5tempC;
  char p0icon[50];
  char p1icon[50];
  char p2icon[50];
  char p3icon[50];
  char p4icon[50];
  char p5icon[50];
  char p0short[50];
  char p1short[50];
  char p2short[50];
  char p3short[50];
  char p4short[50];
  char p5short[50];
   int year;
   int month;
   int day;
   int hour;
   int minute;
  bool pm;
};
typedef struct f_cast Fcst;
Fcst Forecast;

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
   Forecast.p0tempC      = periods0["temperature"];
   Forecast.p0icon      = periods0["icon"];
   Forecast.p0short     = periods0["shortForecast"];
  JsonObject& periods1  = periods[1];
   Forecast.p1name      = periods1["name"];
   Forecast.p1tempC      = periods1["temperature"];
   Forecast.p1icon      = periods1["icon"];
   forecast.p1short     = periods1["shortForecast"];
  JsonObject& periods2  = periods[2];
   Forecast.p2name      = periods2["name"];
   Forecast.p2tempC      = periods2["temperature"];
   Forecast.p2icon      = periods2["icon"];
   Forecast.p2short     = periods2["shortForecast"];
  JsonObject& periods3  = periods[3];
   Forecast.p3name      = periods3["name"];
   forecast.p3tempC      = periods3["temperature"];
   forecast.p3icon      = periods3["icon"];
   forecast.p3short     = periods3["shortForecast"];
  JsonObject& periods4  = periods[4];
   Forecast.p4name      = periods4["name"];
   Forecast.p4tempC      = periods4["temperature"];
   Forecast.p4icon      = periods4["icon"];
   Forecast.p4short     = periods4["shortForecast"];
  JsonObject& periods5  = periods[5];
   Forecast.p5name      = periods5["name"];
   Forecast.p5tempC      = periods5["temperature"];
   Forecast.p5icon      = periods5["icon"];
   Forecast.p5short     = periods5["shortForecast"];

  // ---- timeStamp parse
  int firstDash    = Forecast.generated.indexOf("-");
  int secondDash   = Forecast.generated..lastIndexOf("-");
  int firstT       = Forecast.generated..indexOf("T");
  int firstColon   = Forecast.generated..indexOf(":");
  int secondColon  = Forecast.generated..lastIndexOf(":");
  String yearStr   = Forecast.generated..substring(0, firstDash);
  String monthStr  = Forecast.generated..substring(firstDash + 1);
  String dateStr   = Forecast.generated..substring(secondDash + 1);
  String hourStr   = Forecast.generated..substring(firstT + 1);
  String minuteStr = Forecast.generated..substring(firstColon + 1);
  Forecast.year    = yearStr.toInt();
  Forecast.month   = monthStr.toInt();
  Forecast.minute  = minuteStr.toInt();
  int date_utc = dateStr.toInt();
  int hour_utc = hourStr.toInt();
  hour_utc     += UTCOFFSET;                // TZ offset adjust
  if (hour_utc < 0) { hour_utc += 24; date_utc -= 1; }
  else if (hour_utc > 23) { hour_utc -= 24; date_utc -= 1; }
  else if (hour_utc == 0) { hour_utc += 12; }
  
  Forecast.pm = false;
  if (!hour24_ts) {                            // 12/24 hour adjust
    if (hour_utc >= 12) { hour_utc -= 12; Forecast.pm = true; }
  }
  Forecast.hour   = hour_utc;
  Forecast.date   = date_utc;
}


