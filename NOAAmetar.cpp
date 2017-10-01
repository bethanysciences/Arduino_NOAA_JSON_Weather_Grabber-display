/*----------------------------------------------------------------------*
  NOAAmetar.h - gets Weather METAR XML from aviationweather.gov
                parses and returns elements

  // *** assumes WiFi connection up and running ***

  #include "NOAAmetar.h"       // Gets and processes current NOAA METAR
           // http://www.aviationweather.gov/dataserver/example?datatype=metar
           // NOAAmetar(String station, int &respcode)
           // inputs
           //   station                 reporting station usually airport
           // returns
           //   &respcode               http response code e.g. 200 = ok
           //   struct NOAACurrent      current
           //     current.metar         formatted METAR strip
           //     current.obstimeUTC    observation time UTC
           //     current.tempC         temperature °Celcius
           //     current.dewpointC     dew point °Celcius
           //     current.winddirDeg    wind direction °0-359
           //     current.windspeedKTS  wind speed knots per hour
           //     current.visibilitySM  visibility statute miles
           //     current.altSettingHG  altimeter setting inches hg
           //     current.year          string forecast year
           //     current.month         string forecast month
           //     current.day           string forecast day
           //     current.hour          string forecast hour
           //     current.minute        string forecast minute
           //     current.pm            string forecast pm y/n;

  Example Trimmed METAR XML String
      <observation_time>2017-09-23T19:53:00Z</observation_time>
      <temp_c>30.6</temp_c>
      <dewpoint_c>15.6</dewpoint_c>
      <wind_dir_degrees>0</wind_dir_degrees>
      <wind_speed_kt>6</wind_speed_kt>
      <visibility_statute_mi>10.0</visibility_statute_mi>
      <altim_in_hg>30.02067</altim_in_hg>

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

#include "NOAAmetar.h"

#include <ArduinoHttpClient.h>
#include "timeStamp.h"
#include "xmlTakeParam.h"   // parse XML elements
                    // xmlTakeParam(String inStr, String needParam)
                    // input string       e.g. <temp_c>30.6</temp_c>
                    // input needParam    parameter e.g. temp_c
                    // returns            value as string

int port              = 80;
String server         = "aviationweather.gov";
String product        = "/adds/dataserver_current/";
String dataSource     = "metars";
String requestType    = "retrieve";
String format         = "xml";
String hoursBeforeNow = "1";
String request        = "";
String resp           = "";
WiFiClient wifi;
HttpClient xml = HttpClient(wifi, server, port);

void NOAAmetar(String station, int *code) {
  String request = product + "httpparam?" +
                   "dataSource=" + dataSource + "&" +
                   "requestType=" + requestType + "&" +
                   "format=" + format + "&" +
                   "hoursBeforeNow=" + hoursBeforeNow + "&" +
                   "stationString=" + station;
  xml.get(request);

  *code = xml.responseStatusCode();
  int len = xml.contentLength();
  String response = xml.responseBody();
  current.metar         = xmlTakeParam(respParse, "raw_text");
  current.obstimeUTC    = xmlTakeParam(respParse, "observation_time");
  current.tempC         = xmlTakeParam(respParse, "temp_c");
  current.dewpointC     = xmlTakeParam(respParse, "dewpoint_c");
  current.winddirDeg    = xmlTakeParam(respParse, "wind_dir_degrees");
  current.windspeedKTS  = xmlTakeParam(respParse, "wind_speed_kt");
  current.visibilitySM  = xmlTakeParam(respParse, "visibility_statute_mi");
  current.altSettingHG  = xmlTakeParam(respParse, "altim_in_hg");
  timeStamp(String obstimeUTC, bool hour24, int UTCoffset,
            int &yearTS, int &monthTS, int &dateTS,
            int &hourTS, bool &pmTS, int &minuteTS)
   forecast.year        = yearTS;
   forecast.month       = monthTS;
   forecast.date        = dateTS;
   forecast.hour        = hourTS;
   forecast.minute      = minuteTS;
   forecast.pm          = pmTS;
}
