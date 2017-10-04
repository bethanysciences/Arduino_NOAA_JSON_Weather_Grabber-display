/*----------------------------------------------------------------------*
  METARParse - library to deconstruct Weather METAR
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  Example Trimmed METAR String
    <observation_time>2017-09-23T19:53:00Z</observation_time>
    <temp_c>30.6</temp_c>
    <dewpoint_c>15.6</dewpoint_c>
    <wind_dir_degrees>0</wind_dir_degrees>
    <wind_speed_kt>6</wind_speed_kt>
    <visibility_statute_mi>10.0</visibility_statute_mi>
    <altim_in_hg>30.02067</altim_in_hg>

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

String xmlTakeParam(String inStr, String needParam) {
  if(inStr.indexOf("<"+needParam+">")>0){
    int CountChar=needParam.length();
    int indexStart=inStr.indexOf("<"+needParam+">");
    int indexStop= inStr.indexOf("</"+needParam+">");  
    return inStr.substring(indexStart+CountChar+2, indexStop);
  }
  return "not found";
}
  
void metarParse(String respParse, String &obs_time,
                String &temp_c,   String &dewpoint_c,
                String &wind_dir, String &wind_speed,
                String &visibility, String &altim,
                String &raw_text) {
  raw_text   = xmlTakeParam(respParse, "raw_text");
  obs_time   = xmlTakeParam(respParse, "observation_time");
  temp_c     = xmlTakeParam(respParse, "temp_c");
  dewpoint_c = xmlTakeParam(respParse, "dewpoint_c");
  wind_dir   = xmlTakeParam(respParse, "wind_dir_degrees");
  wind_speed = xmlTakeParam(respParse, "wind_speed_kt");
  visibility = xmlTakeParam(respParse, "visibility_statute_mi");
  altim      = xmlTakeParam(respParse, "altim_in_hg");
}
