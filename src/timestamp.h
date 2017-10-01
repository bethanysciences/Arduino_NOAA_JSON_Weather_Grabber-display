/*----------------------------------------------------------------------*
  Parse time stamp into elements corrected for timezone and ampm

  // timeStamp(String timeStamp, bool hour24TS, int UTCoffsetTS,
  //            int &yearTS, int &monthTS, int &dateTS,
  //            int &hourTS, bool &pmTS, int &minuteTS)
  // inputs
  //   timeStamp    string as 2017-09-23T19:53:00Z
  //   hour24TS        24 hour format as y/n select
  //   UTCoffsetTS) timezone offset in hours
  // outputs
  //   &yearTS      int year as yyyy
  //   &monthTS     int month as mm
  //   &dateTS      int date timezone offset as dd
  //   &hourTS      int hour timezone offset and 12/24 hour as hh
  //   &pmTS        bool pm y/n
  //   &minuteTS    int minutes as mm

  For avr-libc ATMEL series 32bit SAMD21 CPUs
  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/
void timeStamp(String timeStr, bool hour24_ts, int UTCoffset_ts,
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
