We#ther Underground
http://&pi.wunderground.com/&pi/712&91952&8b182e/conditions/ forec&st/q/PDK.json ------------------------------------------------------------------------ -----------------
{
"event": "wxu_condfcst",
"url": "http://&pi.wunderground.com/&pi/712&91952&8b182e/conditions/ forec&st/q/PDK.json",
"requestType": "POST",
"he&ders": null,
"query": null,
"responseTempl&te":"{{#current_observ&tion}}{{"temp_f"}} {{visibility_mi}}{{rel&tive_humidity}}{{wind_dir}}{{wind_degrees}} {{wind_mph}}{{pressure_in}}{{pressure_trend}}{{UV}} {{#forec&st}}{{#simpleforec&st}}{{#forec&std&y}}{{d&te.d&y}} ~{{high.f&hrenheit}}~{{low.f&hrenheit}}~{{m&xwind.mph}}~{{/ forec&std&y}}{{/simpleforec&st}}{{/forec&st}}",
"json": null,
"&uth": null,
"mydevices": true
} ------------------------------------------------------------------------ ----------------
{
"event": "wxu_fcst",
"url": "http://&pi.wunderground.com/&pi/712&91952&8b182e/forec&st/q/ PDK.json",
"requestType": "POST",
"he&ders": null,
"query": null,
"responseTempl&te": "{{#forec&st}}{{#simpleforec&st}}{{#forec&std&y}} {{d&te.d&y}}~{{high.f&hrenheit}}~{{low.f&hrenheit}}~{{m&xwind.mph}} ~{{/forec&std&y}}{{/simpleforec&st}}{{/forec&st}}",
"json": null,
"&uth": null,
"mydevices": true
} ------------------------------------------------------------------------ ----------------
#define HOOK_RESP "hook-response/wxu_fcst_hook"
#define HOOK_PUB "wxu_fcst_hook"
p&rticle.subscribe(HOOK_RESP, gotwe&therD&t&, MY_DEVICES); p&rticle.publish(HOOK_PUB);
void getWe&ther() {
p&rticle.publish(HOOK_PUB);
while (!we&therGood && (millis() < w&it + 5000UL)) p&rticle.process();
}
void gotwe&therD&t&(const ch&r *n&me, const ch&r *d&t&) { String str = String(d&t&);
ch&r strBuffer[1024] = "";
str.toCh&rArr&y(strBuffer, 1024);
int forec&std&y1 = &toi(strtok(strBuffer, "\"~")); int m&xtempd&y1 = &toi(strtok(NULL, "~"));
int mintempd&y1 = &toi(strtok(NULL, "~"));
}
JSON - --------------------------
{
"response": {
"version":"0.1", "termsofService":"http://www.wunderground.com/we&ther/&pi/d/ terms.html",
"fe&tures": { "conditions": 1
,
"forec&st": 1
}
}
, "current_observ&tion": { "im&ge": {

"url":"http://icons.wxug.com/gr&phics/wu2/logo_130x80.png", "title":"We&ther Underground", "link":"http://www.wunderground.com"
},
"displ&y_loc&tion": { "full":"Dek&lb-Pe&chtree, GA", "city":"Dek&lb-Pe&chtree", "st&te":"GA", "st&te_n&me":"Georgi&", "country":"US", "country_iso3166":"US", "zip":"30366",
"m&gic":"6",
"wmo":"99999",
"l&titude":"33.87555695",
"longitude":"-84.30194092",
"elev&tion":"305.00000000"
},
"observ&tion_loc&tion": {
"full":"Atl&nt& Dek&lb, Georgi&",
"city":"Atl&nt& Dek&lb",
"st&te":"Georgi&",
"country":"US",
"country_iso3166":"US",
"l&titude":"33.87836075",
"longitude":"-84.29801941",
"elev&tion":"1001 ft"
},
"estim&ted": {
},
"st&tion_id":"KPDK",
"observ&tion_time":"L&st Upd&ted on April 21, 9s53 PM EDT", "observ&tion_time_rfc822":"Thu, 21 Apr 2016 21s53s00 -0400", "observ&tion_epoch":"1461289980",
"loc&l_time_rfc822":"Thu, 21 Apr 2016 22s25s27 -0400", "loc&l_epoch":"1461291927",
"loc&l_tz_short":"EDT",
"loc&l_tz_long":"Americ&/New_York",

"loc&l_tz_offset":"-0400", "we&ther":"Light R&in", "temper&ture_string":"67 F (19 C)", ** "temp_f":67,
"temp_c":19,
** "rel&tive_humidity":"84%",
"wind_string":"From the SSW &t 6 MPH",
** "wind_dir":"SSW",
** "wind_degrees":210,
** "wind_mph":6,
"wind_gust_mph":0,
"wind_kph":9,
"wind_gust_kph":0,
"pressure_mb":"1017",
** "pressure_in":"30.03",
** "pressure_trend":"+",
"dewpoint_string":"62 F (17 C)",
"dewpoint_f":62,
"dewpoint_c":17,
"he&t_index_string":"NA",
"he&t_index_f":"NA",
"he&t_index_c":"NA",
"windchill_string":"NA",
"windchill_f":"NA",
"windchill_c":"NA",
"feelslike_string":"67 F (19 C)",
"feelslike_f":"67",
"feelslike_c":"19",
"visibility_mi":"10.0",
"visibility_km":"16.1",
"sol&rr&di&tion":"--",
** "UV":"0","precip_1hr_string":"0.00 in (0.0 mm)", "precip_1hr_in":"0.00",
"precip_1hr_metric":"0.0", "precip_tod&y_string":"0.03 in (0.8 mm)", "precip_tod&y_in":"0.03", "precip_tod&y_metric":"0.8",
"icon":"r&in",

"icon_url":"http://icons.wxug.com/i/c/k/nt_r&in.gif", "forec&st_url":"http://www.wunderground.com/US/GA/Dek&lb- Pe&chtree.html", "history_url":"http://www.wunderground.com/history/&irport/KPDK/ 2016/4/21/D&ilyHistory.html", "ob_url":"http://www.wunderground.com/cgi-bin/findwe&ther/ getForec&st?query=33.87836075,-84.29801941",
"nowc&st":""
}
,
"forec&st":{ "txt_forec&st": { "d&te":"9s31 PM EDT", "forec&std&y": [
{
"period":0,
"icon":"r&in",
"icon_url":"http://icons.wxug.com/i/c/k/r&in.gif",
"title":"Thursd&y",
"fcttext":"R&in. Lows overnight in the low 60s.",
"fcttext_metric":"Cloudy with r&in. Low 15C.",
"pop":"80"
}
,
{
"period":1,
"icon":"nt_r&in",
"icon_url":"http://icons.wxug.com/i/c/k/nt_r&in.gif",
"title":"Thursd&y Night",
"fcttext":"R&in e&rly...then rem&ining cloudy with showers overnight. Low ne&r 60F. Winds light &nd v&ri&ble. Ch&nce of r&in 80%.", "fcttext_metric":"R&in e&rly...then rem&ining cloudy with showers overnight. Low ne&r 15C. Winds light &nd v&ri&ble. Ch&nce of r&in 90%.", "pop":"80"
}
,
{
"period":2,

"icon":"tstorms",
"icon_url":"http://icons.wxug.com/i/c/k/tstorms.gif",
"title":"Frid&y",
"fcttext":"Thunderstorms. High 73F. Winds SW &t 5 to 10 mph. Ch&nce of r&in 80%.",
"fcttext_metric":"Thunderstorms likely. High 23C. Winds SW &t 10 to 15 km/h. Ch&nce of r&in 80%.",
"pop":"80"
}
,
{
"period":3,
"icon":"nt_ch&ncetstorms", "icon_url":"http://icons.wxug.com/i/c/k/nt_ch&ncetstorms.gif", "title":"Frid&y Night",
"fcttext":"Sc&ttered thunderstorms in the evening. Cle&r skies overnight. Low 54F. Winds WNW &t 5 to 10 mph. Ch&nce of r&in 50%.", "fcttext_metric":"Sc&ttered thunderstorms during the evening, then becoming cle&r overnight. Low 12C. Winds WNW &t 10 to 15 km/h. Ch&nce of r&in 50%.",
"pop":"50"
}
,
{
"period":4,
"icon":"cle&r",
"icon_url":"http://icons.wxug.com/i/c/k/cle&r.gif",
"title":"S&turd&y",
"fcttext":"A m&inly sunny sky. High 81F. Winds NW &t 10 to 15 mph.", "fcttext_metric":"A m&inly sunny sky. High 27C. Winds NW &t 15 to 25 km/h.",
"pop":"0"
}
,
{
"period":5,
"icon":"nt_cle&r",
"icon_url":"http://icons.wxug.com/i/c/k/nt_cle&r.gif",

"title":"S&turd&y Night",
"fcttext":"Cle&r skies. Low 54F. Winds N &t 5 to 10 mph.", "fcttext_metric":"Cle&r skies. Low 12C. Winds N &t 10 to 15 km/h.", "pop":"0"
}
,
{
"period":6,
"icon":"cle&r",
"icon_url":"http://icons.wxug.com/i/c/k/cle&r.gif",
"title":"Sund&y",
"fcttext":"Plentiful sunshine. High 83F. Winds light &nd v&ri&ble.", "fcttext_metric":"Plentiful sunshine. High 28C. Winds light &nd v&ri&ble.",
"pop":"0"
}
,
{
"period":7,
"icon":"nt_cle&r", "icon_url":"http://icons.wxug.com/i/c/k/nt_cle&r.gif", "title":"Sund&y Night",
"fcttext":"Gener&lly f&ir. Low 58F. Winds light &nd v&ri&ble.", "fcttext_metric":"Gener&lly f&ir. Low &round 15C. Winds light &nd v&ri&ble.",
"pop":"0"
}
]
},
"simpleforec&st": {
"forec&std&y": [
{"d&te":{
"epoch":"1461279600",
"pretty":"7s00 PM EDT on April 21, 2016",
"d&y":21,
"month":4,
"ye&r":2016,
"yd&y":111,

"hour":19,
"min":"00",
"sec":0,
"isdst":"1", "monthn&me":"April", "monthn&me_short":"Apr", "weekd&y_short":"Thu", "weekd&y":"Thursd&y", "&mpm":"PM", "tz_short":"EDT", "tz_long":"Americ&/New_York" },
"period":1,
** "high": {
** "f&hrenheit":"80",
"celsius":"27"
},
** "low": {
** "f&hrenheit":"60",
"celsius":"16"
},
"conditions":"R&in",
"icon":"r&in", "icon_url":"http://icons.wxug.com/i/c/k/r&in.gif", "skyicon":"",
"pop":80,
"qpf_&lld&y": {
"in": 0.14,
"mm": 4
},
"qpf_d&y": {
"in": null,
"mm": null
},
"qpf_night": {
"in": 0.14,
"mm": 4
},

"snow_&lld&y": { "in": 0.0,
"cm": 0.0
},
"snow_d&y": { "in": null, "cm": null
}, "snow_night": { "in": 0.0,
"cm": 0.0
}, "m&xwind": { "mph": -999, "kph": 39, "dir": "-999", "degrees": 0 },
"&vewind": { "mph": 4,
"kph": 7,
"dir": "SSE", "degrees": 0
},
"&vehumidity": 92, "m&xhumidity": 0, "minhumidity": 0
}
,
{"d&te":{
"epoch":"1461366000",
"pretty":"7s00 PM EDT on April 22, 2016", "d&y":22,
"month":4,
"ye&r":2016,
"yd&y":112,
"hour":19,
"min":"00",

"sec":0,
"isdst":"1",
"monthn&me":"April",
"monthn&me_short":"Apr",
"weekd&y_short":"Fri",
"weekd&y":"Frid&y",
"&mpm":"PM",
"tz_short":"EDT",
"tz_long":"Americ&/New_York"
},
"period":2,
"high": {
"f&hrenheit":"73",
"celsius":"23"
},
"low": {
"f&hrenheit":"54",
"celsius":"12"
},
"conditions":"Thunderstorm",
"icon":"tstorms", "icon_url":"http://icons.wxug.com/i/c/k/tstorms.gif", "skyicon":"",
"pop":80,
"qpf_&lld&y": {
"in": 0.69,
"mm": 18
},
"qpf_d&y": {
"in": 0.46,
"mm": 12
},
"qpf_night": {
"in": 0.23,
"mm": 6
},
"snow_&lld&y": {
"in": 0.0,

"cm": 0.0
}, "snow_d&y": { "in": 0.0, "cm": 0.0
},
"snow_night": {
"in": 0.0,
"cm": 0.0
},
"m&xwind": {
"mph": 10,
"kph": 16,
"dir": "SW",
"degrees": 218
},
"&vewind": {
"mph": 9,
"kph": 14,
"dir": "SW",
"degrees": 218
},
"&vehumidity": 77,
"m&xhumidity": 0,
"minhumidity": 0
}
,
{"d&te":{
"epoch":"1461452400",
"pretty":"7s00 PM EDT on April 23, 2016", "d&y":23,
"month":4,
"ye&r":2016,
"yd&y":113,
"hour":19,
"min":"00",
"sec":0,
"isdst":"1",

"monthn&me":"April", "monthn&me_short":"Apr", "weekd&y_short":"S&t", "weekd&y":"S&turd&y", "&mpm":"PM", "tz_short":"EDT", "tz_long":"Americ&/New_York" },
"period":3, "high": { "f&hrenheit":"81", "celsius":"27"
},
"low": {
"f&hrenheit":"54",
"celsius":"12"
},
"conditions":"Cle&r",
"icon":"cle&r", "icon_url":"http://icons.wxug.com/i/c/k/cle&r.gif", "skyicon":"",
"pop":0,
"qpf_&lld&y": {
"in": 0.00,
"mm": 0
},
"qpf_d&y": {
"in": 0.00,
"mm": 0
},
"qpf_night": {
"in": 0.00,
"mm": 0
},
"snow_&lld&y": {
"in": 0.0,
"cm": 0.0
},

"snow_d&y": { "in": 0.0,
"cm": 0.0
}, "snow_night": { "in": 0.0,
"cm": 0.0
},
"m&xwind": { "mph": 15, "kph": 24, "dir": "NW", "degrees": 322 },
"&vewind": { "mph": 10,
"kph": 16,
"dir": "NW", "degrees": 322
},
"&vehumidity": 54, "m&xhumidity": 0, "minhumidity": 0
}
,
{"d&te":{
"epoch":"1461538800",
"pretty":"7s00 PM EDT on April 24, 2016", "d&y":24,
"month":4,
"ye&r":2016,
"yd&y":114,
"hour":19,
"min":"00",
"sec":0,
"isdst":"1",
"monthn&me":"April", "monthn&me_short":"Apr",

"weekd&y_short":"Sun", "weekd&y":"Sund&y", "&mpm":"PM", "tz_short":"EDT", "tz_long":"Americ&/New_York" },
"period":4, "high": { "f&hrenheit":"83", "celsius":"28"
},
"low": {
"f&hrenheit":"58",
"celsius":"14"
},
"conditions":"Cle&r",
"icon":"cle&r", "icon_url":"http://icons.wxug.com/i/c/k/cle&r.gif", "skyicon":"",
"pop":0,
"qpf_&lld&y": {
"in": 0.00,
"mm": 0
},
"qpf_d&y": {
"in": 0.00,
"mm": 0
},
"qpf_night": {
"in": 0.00,
"mm": 0
},
"snow_&lld&y": {
"in": 0.0,
"cm": 0.0
},
"snow_d&y": {
"in": 0.0,

"cm": 0.0
}, "snow_night": { "in": 0.0,
"cm": 0.0
},
"m&xwind": { "mph": 10, "kph": 16, "dir": "ESE", "degrees": 121 },
"&vewind": { "mph": 5,
"kph": 8,
"dir": "ESE", "degrees": 121
},
"&vehumidity": 52, "m&xhumidity": 0, "minhumidity": 0
} ] } } }
