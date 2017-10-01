/*----------------------------------------------------------------------*
  xmlTakeParam.h - parse XML elements

  #include xmlTakeParam.h - parse XML elements
    // xmlTakeParam(String inStr, String needParam)
    // input string       e.g. <temp_c>30.6</temp_c>
    // input needParam    parameter e.g. temp_c
    // returns value in string

    Example XML String
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
String xmlTakeParam(String inStr, String needParam) {
  if(inStr.indexOf("<"+needParam+">")>0){
    int CountChar=needParam.length();
    int indexStart=inStr.indexOf("<"+needParam+">");
    int indexStop= inStr.indexOf("</"+needParam+">");
    return inStr.substring(indexStart+CountChar+2, indexStop);
  }
  return "not found";
}
