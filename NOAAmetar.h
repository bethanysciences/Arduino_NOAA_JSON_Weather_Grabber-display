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
#include <ArduinoHttpClient.h>
#include "timeStamp.h"
#include "WXdefines.h"
#include "xmlTakeParam.h"   // parse XML elements
                    // xmlTakeParam(String inStr, String needParam)
                    // input string       e.g. <temp_c>30.6</temp_c>
                    // input needParam    parameter e.g. temp_c
                    // returns            value as string
   int currPort           = 80;
String currServer         = "aviationweather.gov";
String currProduct        = "/adds/dataserver_current/";
String currDataSource     = "metars";
String currRequestType    = "retrieve";
String currFormat         = "xml";
String currHoursBeforeNow = "1";
String currRequest        = "";
String currResp           = "";
WiFiClient wifi;
HttpClient xml = HttpClient(wifi, currServer, currPort);

void NOAAmetar(String currStation, int *currCode) {
  String request = currProduct + "httpparam?" +
                   "dataSource=" + currDataSource + "&" +
                   "requestType=" + currRequestType + "&" +
                   "format=" + currFormat + "&" +
                   "hoursBeforeNow=" + currHoursBeforeNow + "&" +
                   "stationString=" + CURRSTATION;
  xml.get(request);

  *code = xml.responseStatusCode();
  int len = xml.contentLength();
  String response = xml.responseBody();
  Current.metar         = xmlTakeParam(respParse, "raw_text");
  Current.obstimeUTC    = xmlTakeParam(respParse, "observation_time");
  Current.tempC         = xmlTakeParam(respParse, "temp_c");
  Current.dewpointC     = xmlTakeParam(respParse, "dewpoint_c");
  Current.winddirDeg    = xmlTakeParam(respParse, "wind_dir_degrees");
  Current.windspeedKTS  = xmlTakeParam(respParse, "wind_speed_kt");
  Current.visibilitySM  = xmlTakeParam(respParse, "visibility_statute_mi");
  Current.altSettingHG  = xmlTakeParam(respParse, "altim_in_hg");
  timeStamp(String obstimeUTC, bool hour24, int UTCoffset,
            int &yearTS, int &monthTS, int &dateTS,
            int &hourTS, bool &pmTS, int &minuteTS)
  Current.year          = yearTS;
  Current.month         = monthTS;
  Current.date          = dateTS;
  Current.hour          = hourTS;
  Current.minute        = minuteTS;
  Current.pm            = pmTS;
}