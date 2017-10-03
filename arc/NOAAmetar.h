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
#include "ArduinoHttpClient.h"
#include "xmlTakeParam.h"
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
struct c_rent {
String metar;
String obstimeUTC;
double tempC;
double dewpointC;
   int winddirDeg;
   int windspeedKTS;
   int visibilitySM;
double altSettingHG;
   int year;
   int month;
   int date;
   int hour;
   int minute;
  bool pm;
};
typedef struct c_rent Curr;
Curr Current;

void NOAAmetar() {
  String request = currProduct + "httpparam?" +
                   "dataSource=" + currDataSource + "&" +
                   "requestType=" + currRequestType + "&" +
                   "format=" + currFormat + "&" +
                   "hoursBeforeNow=" + currHoursBeforeNow + "&" +
                   "stationString=KPDK";
  xml.get(request);
  // currCode = xml.responseStatusCode();
  int len = xml.contentLength();
  String response = xml.responseBody();
  Current.metar         = xmlTakeParam(response, "raw_text");
  Current.obstimeUTC    = xmlTakeParam(response, "observation_time");
  Current.tempC         = xmlTakeParam(response, "temp_c").toDouble();
  Current.dewpointC     = xmlTakeParam(response, "dewpoint_c").toDouble();
  Current.winddirDeg    = xmlTakeParam(response, "wind_dir_degrees").toInt();
  Current.windspeedKTS  = xmlTakeParam(response, "wind_speed_kt").toInt();
  Current.visibilitySM  = xmlTakeParam(response, "visibility_statute_mi").toInt();
  Current.altSettingHG  = xmlTakeParam(response, "altim_in_hg").toDouble();

  // ---- timeStamp parse
  int firstDash    = Current.obstimeUTC .indexOf("-");
  int secondDash   = Current.obstimeUTC .lastIndexOf("-");
  int firstT       = Current.obstimeUTC .indexOf("T");
  int firstColon   = Current.obstimeUTC .indexOf(":");
  int secondColon  = Current.obstimeUTC .lastIndexOf(":");

  String yearStr   = Current.obstimeUTC .substring(0, firstDash);
  String monthStr  = Current.obstimeUTC .substring(firstDash + 1);
  String dateStr   = Current.obstimeUTC .substring(secondDash + 1);
  String hourStr   = Current.obstimeUTC .substring(firstT + 1);
  String minuteStr = Current.obstimeUTC .substring(firstColon + 1);

  Current.year     = yearStr.toInt();
  Current.month    = monthStr.toInt();
  Current.minute   = minuteStr.toInt();
  int date_utc     = dateStr.toInt();
  int hour_utc     = hourStr.toInt();
  hour_utc        += UTCOFFSET;        // TZ offset adjust
  if (hour_utc < 0) { hour_utc += 24; date_utc -= 1; }
  else if (hour_utc > 23) { hour_utc -= 24; date_utc -= 1; }
  else if (hour_utc == 0) { hour_utc += 12; }
  
  Current.pm = false;
  if (!hour24_ts) {                       // 12/24 hour adjust
    if (hour_utc >= 12) { 
      hour_utc -= 12; 
      Current.pm = true; 
    }
  }
  Current.hour    = hour_utc;
  Current.date    = date_utc;
}

