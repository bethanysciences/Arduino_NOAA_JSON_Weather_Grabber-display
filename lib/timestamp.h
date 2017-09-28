/*----------------------------------------------------------------------*
  Parse time stamp into elements corrected for timezone and ampm
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  inputs   (timeStr)      timestamp string as 2017-09-23T19:53:00Z
           (hour24_ts)    24 hour format as y/n select 
           (UTCoffset_ts) timezone offset in hours
  outputs  (year_ts)      year as yyyy
           (month_ts)     month as mm
           (date_ts)      date timezone offset as dd
           (hour_ts)      hour timezone offset and 12/24 hour as hh
           (pm)           pm as y/n select
           (minute_ts)    minutes as mm

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
void timestamp(String timeStr, bool hour24_ts, int UTCoffset_ts, 
               int *year_ts, int *month_ts, int *date_ts, 
               int *hour_ts, bool *pm_ts, int *minute_ts) {
  int firstDash    = timeStr.indexOf("-");
  int secondDash   = timeStr.lastIndexOf("-");
  int firstT       = timeStr.indexOf("T");
  int firstColon   = timeStr.indexOf(":");
  int secondColon  = timeStr.lastIndexOf(":");
  
  String yearStr   = timeStr.substring(0, firstDash);
  String monthStr  = timeStr.substring(firstDash + 1);
  String dateStr   = timeStr.substring(secondDash + 1);
  String hourStr   = timeStr.substring(firstT + 1);
  String minuteStr = timeStr.substring(firstColon + 1);

  *year_ts     = yearStr.toInt();
  *month_ts    = monthStr.toInt();
  *minute_ts   = minuteStr.toInt();

  int date_utc = dateStr.toInt();
  int hour_utc = hourStr.toInt();
  hour_utc     += UTCoffset_ts;                        // TZ offset adjust
  if (hour_utc < 0) { hour_utc += 24; date_utc -= 1; }
  else if (hour_utc > 23) { hour_utc -= 24; date_utc -= 1; }
  else if (hour_utc == 0) { hour_utc += 12; }
  
  if (!hour24_ts) {                                   // 12/24 hour adjust
    if (hour_utc >= 12) { hour_utc -= 12; *pm_ts = true; }
  }
  *hour_ts = hour_utc;
  *date_ts = date_utc;
}
