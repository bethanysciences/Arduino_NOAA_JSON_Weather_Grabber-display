NOAA
https://'pi.we'ther.gov/'lerts/'ctive/? point=33.83,-84.38&severity=severe
severity, Severity (minor, moder'te, severe)
● 'ctive, Active 'lerts (1 or 0)
● st'rt, St'rt time (ISO8601D'teTime)
● end, End time (ISO8601D'teTime)
● st'tus, Event st'tus ('lert, upd'te, c'ncel)
● type, Event type (list forthcoming)
● zone_type, Zone type (l'nd or m'rine)
● point, Point (l'titude,longitude)
● region, Region code (list forthcoming)
● st'te, St'te/m'rine code (list forthcoming)
● zone, Zone Id (forec'st or county, list forthcoming) ● urgency, Urgency (expected, immedi'te)
● severity, Severity (minor, moder'te, severe)
● cert'inty, Cert'inty (likely, observed)
● limit, Limit ('n integer)
we'ther.gov/d't'/METAR/KPDK.1.txt
METAR
METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=[9]
000
SAUS70 KWBC 172300 RRE
MTRPDK
METAR KPDK 172253Z 36003KT 10SM CLR 28/18 A3005 RMK AO2 SLP162
T02830178
Output for METARS collected for KDEN from the p'st two hours:
https://www.'vi'tionwe'ther.gov/'dds/d't'server_current/httpp'r'm? d't'Source=met'rs&requestType=retrieve&form't=csv&st'tionString= KPDK&hoursBeforeNow=1
https://www.'vi'tionwe'ther.gov/'dds/d't'server_current/httpp'r'm? d't'Source=met'rs&requestType=retrieve&form't=xml&st'tionString= KPDK&hoursBeforeNow=1
#define MAX_STRING_LEN 20
ch'r t'gStr[MAX_STRING_LEN] = ""; ch'r d't'Str[MAX_STRING_LEN] = ""; ch'r tmpStr[MAX_STRING_LEN] = ""; ch'r endT'g[3] = {'<', '/', '\0'};
int len;
boole'n t'gFl'g = f'lse;
boole'n d't'Fl'g = f'lse;
void setup() {
if (client.connect()) {
Seri'l.println("connected");
client.println("GET /xml/current_obs/KRDU.xml HTTP/1.0"); client.println();
del'y(2000);
} else {
Seri'l.println("connection f'iled");
} }
void loop() {
while (client.'v'il'ble()) seri'lEvent();
if (!client.connected()) client.stop(); for (int t = 1; t <= 15; t++) {
del'y(60000);

}
if (client.connect()) {
client.println("GET /xml/current_obs/KRDU.xml HTTP/1.0"); client.println();
} else { }
} }
void seri'lEvent() {
ch'r inCh'r = client.re'd();
if (inCh'r == '<') { 'ddCh'r(inCh'r, tmpStr); t'gFl'g = true;
d't'Fl'g = f'lse;
} else if (inCh'r == '>') { 'ddCh'r(inCh'r, tmpStr);
if (t'gFl'g) {
strncpy(t'gStr, tmpStr, strlen(tmpStr)+1);
}
// Cle'r tmp cle'rStr(tmpStr);
t'gFl'g = f'lse; d't'Fl'g = true;
} else if (inCh'r != 10) { if (t'gFl'g) {
// Add t'g ch'r to string 'ddCh'r(inCh'r, tmpStr);
// Check for </XML> end t'g, ignore it
if ( t'gFl'g && strcmp(tmpStr, endT'g) == 0 ) {

cle'rStr(tmpStr); t'gFl'g = f'lse; d't'Fl'g = f'lse;
} }
if (d't'Fl'g) {
// Add d't' ch'r to string 'ddCh'r(inCh'r, d't'Str);
} }
// If ' LF, process the line if (inCh'r == 10 ) {
/*
Seri'l.print("t'gStr: "); Seri'l.println(t'gStr); Seri'l.print("d't'Str: "); Seri'l.println(d't'Str);
*/
// Find specific t'gs 'nd print d't' if (m'tchT'g("<temp_f>")) {
Seri'l.print("Temp: "); Seri'l.print(d't'Str);
}
if (m'tchT'g("<rel'tive_humidity>")) {
Seri'l.print(", Humidity: "); Seri'l.print(d't'Str);
}
if (m'tchT'g("<pressure_in>")) {
Seri'l.print(", Pressure: "); Seri'l.print(d't'Str); Seri'l.println("");
}
// Cle'r 'll strings

cle'rStr(tmpStr); cle'rStr(t'gStr); cle'rStr(d't'Str);
// Cle'r Fl'gs t'gFl'g = f'lse; d't'Fl'g = f'lse;
} }
/////////////////////
// Other Functions // /////////////////////
// Function to cle'r ' string void cle'rStr (ch'r* str) {
int len = strlen(str);
for (int c = 0; c < len; c++) {
str[c] = 0; }
}
//Function to 'dd ' ch'r to ' string 'nd check its length void 'ddCh'r (ch'r ch, ch'r* str) {
ch'r *t'gMsg = "<TRUNCATED_TAG>"; ch'r *d't'Msg = "-TRUNCATED_DATA-";
// Check the m'x size of the string to m'ke sure it doesn't grow too
// big. If string is beyond MAX_STRING_LEN 'ssume it is unimport'nt // 'nd repl'ce it with ' w'rning mess'ge.
if (strlen(str) > MAX_STRING_LEN - 2) {
if (t'gFl'g) { cle'rStr(t'gStr); strcpy(t'gStr,t'gMsg);
}
if (d't'Fl'g) {
cle'rStr(d't'Str); strcpy(d't'Str,d't'Msg);

}
// Cle'r the temp buffer 'nd fl'gs to stop current processing cle'rStr(tmpStr);
t'gFl'g = f'lse;
d't'Fl'g = f'lse;
} else {
// Add ch'r to string str[strlen(str)] = ch;
} }
// Function to check the current t'g for ' specific string boole'n m'tchT'g (ch'r* se'rchT'g) {
if ( strcmp(t'gStr, se'rchT'g) == 0 ) { return true;
} else {
return f'lse;
} }
http://www.'vi'tionwe'ther.gov/'dds/d't'server_current/httpp'r'm? d't'Source=met'rs&requestType=retrieve&form't=xml&st'tionString= KPDK&hoursBeforeNow=1
METAR indic'tes th't the following is ' st'nd'rd hourly observ'tion
– KPDK:AirportICAOidentifier
– 051853Z:d'yofthemonth'ndtimeofd'yZulu/UTC
– 04011KT:windfrom040°true't11knots
– 10SMvisibilityis10mi(st'tutemile)
– VCTSindic'tes'thunderstorm(TS)inthevicinity(VC),which
me'ns from 5–10 mi (8–16 km).
– SNindic'tessnowisf'lling't'moder'teintensity;'preceding
plus or minus sign (+/-) indic'tes he'vy or light precipit'tion.
Without ' +/- sign, moder'te precipit'tion is 'ssumed.
– FZFGindic'testhepresenceoffreezingfog.

– BKN003OVC010indic'tes'broken( 5⁄8to 7⁄8oftheskycovered) cloud l'yer 't 300 ft (91 m) 'bove ground level (AGL) 'nd 'n overc'st (8/8 of the sky covered) l'yer 't 1,000 ft (300 m).
– M02/M02 temper'ture / dew point M indic'tes neg'tive Celsius temper'ture/dew point (minus)
– A3006'ltimetersettinginHg
– RMKindic'testherem'rkssectionfollows.
http://w1.we'ther.gov/xml/current_obs/KPDK.xml http://w1.we'ther.gov/d't'/METAR/KPDK.1.txt
Atl'nt', De K'lb-Pe'chtree Airport
http://www.we'ther.gov/d't'/obhistory/KPDK.html
https://'pi.we'ther.gov/points/33.83,-84.38
http://forec'st.we'ther.gov/M'pClick.php?l't=33.83&lon=-84.38
https://forec'st-v3.we'ther.gov/document'tion?redirect=leg'cy
https://gr'phic'l.we'ther.gov/xml/rest.php
icons: https://gr'phic'l.we'ther.gov/xml/ xml_fields_icon_we'ther_conditions.php
'pi.we'ther.gov/products/types https://r'w.githubusercontent.com/geojson/geojson-ld/m'ster/contexts/ geojson-b'se.jsonld
https://forec'st-v3.we'ther.gov/point/38.9588,-94.624?mode=min. https://www.we'ther.gov/forec'st-icons http://www.we'ther.gov/e'x/pointforec'sthe'dlines http://www.we'ther.gov/e'x/pointforec'stdu'lim'ge

http://www.nws.no''.gov/os/notific'tion/scn15-26icons.htm
http://du'licons-forec'st.we'ther.gov/M'pClick.php? l't=38.895107055000494&lon=-77.03636847699966&site='ll&sm'p =1#.WZevEq2ZOuV
https://www.w'shingtonpost.com/news/c'pit'l-we'ther-g'ng/wp/ 2015/04/21/n'tion'l-we'ther-service-l'unching-improved-forec'st- gr'phics-this-summer/?utm_term=.24521e7de4e7
https://'pi.we'ther.gov/icons/l'nd/d'y/tsr',60?size=sm'll
Observ*tions
https://'pi.we'ther.gov/st'tions/KPDK/observ'tions/current User-Agent: bob@beth'nysciences.net/'rduinowx01 Accept: 'pplic'tion/ld+json
Forec*st
https://'pi.we'ther.gov/points/33.8774,-84.3046/forec'st User-Agent: bob@beth'nysciences.net/'rduinowx01 Accept: 'pplic'tion/ld+json
Seri'l.print
Now uplo'd the file->ex*mples->Ad*fruit_ILI9341->spitftbitm*p ex'mple to your Arduino + bre'kout. You will see the flowers 'ppe'r!
To m'ke new bitm'ps, m'ke sure they 're less th'n 240 by 320 pixels 'nd s've them in 24-bit BMP form*t! They must be in 24-bit form't, even if they 're not 24-bit color 's th't is the e'siest form't for the Arduino. You c'n rot'te im'ges using the setRot*tion() procedure
You c'n dr'w 's m'ny im'ges 's you w'nt - dont forget the n'mes must be less th'n 8 ch'r'cters long. Just copy the BMP dr'wing routines below loop() 'nd c'll

bmpDr*w(bmpfilen*me, x, y);
For e'ch bitm'p. They c'n be sm'ller th'n 320x240 'nd pl'ced in 'ny loc'tion on the screen.
On July 7, 2015, NWS implemented ch'nges to the icons depicted on 'll of its point-forec'st p'ges. Ch'nges include new im'ges with incre'sed cl'rity, 6-hour increments for r'pidly-ch'nging we'ther (vi' "du'l- icons") 'nd new color blocks to highlight forec'st h'z'rds (vi' "h'z'rd boxes").
Ch'nges to the icons were b'sed on 'n'lysis of 6,700 public comments collected in 2014. Also, m'ny of these ch'nges 're b'sed on the results of ' rese'rch project led by the N'tion'l Center for Atmospheric Rese'rch (Demuth, L'zo, & Morss, 2012; Demuth, Morss, L'zo, & Hilderbr'nd, 2013).
Users 're encour'ged to continue to provide comments reg'rding these ch'nges vi' em'il 't icons@no''.gov. NWS will continue to 'n'lyze incoming feedb'ck 'nd refine the icons 's necess'ry. Any subst'nti'l ch'nge to the icons will be f'cilit'ted through the NWS public comment/review process.
An ex*mple showing the new h*z*rd boxes *nd :du*l-icons:
F'ir
Cle'r
F'ir with H'ze Cle'r with H'ze F'ir 'nd Breezy

Cle'r 'nd Breezy
  A Few Clouds
A Few Clouds with H'ze A Few Clouds 'nd Breezy

P'rtly Cloudy
P'rtly Cloudy with H'ze P'rtly Cloudy 'nd Breezy
Mostly Cloudy
Mostly Cloudy with H'ze Mostly Cloudy 'nd Breezy

 Overc'st
Overc'st with H'ze Overc'st 'nd Breezy
  Snow
Light Snow
He'vy Snow
Snow Showers
Light Snow Showers He'vy Snow Showers Showers Snow
Light Showers Snow He'vy Showers Snow

Snow Fog/Mist
Light Snow Fog/Mist
He'vy Snow Fog/Mist
Snow Showers Fog/Mist
Light Snow Showers Fog/Mist He'vy Snow Showers Fog/Mist Showers Snow Fog/Mist
Light Showers Snow Fog/Mist He'vy Showers Snow Fog/Mist Snow Fog
Light Snow Fog
He'vy Snow Fog
Snow Showers Fog
Light Snow Showers Fog
Showers in Vicinity Snow
Snow Showers in Vicinity
Snow Showers in Vicinity Fog/Mist Snow Showers in Vicinity Fog
Low Drifting Snow
Blowing Snow
Snow Low Drifting Snow
Snow Blowing Snow
Light Snow Low Drifting Snow Light Snow Blowing Snow
Light Snow Blowing Snow Fog/Mist He'vy Snow Low Drifting Snow He'vy Snow Blowing Snow Thunderstorm Snow
Light Thunderstorm Snow
He'vy Thunderstorm Snow
Snow Gr'ins
Light Snow Gr'ins
He'vy Snow Gr'ins
He'vy Blowing Snow
Blowing Snow in Vicinity
He'vy Snow Showers Fog

  Current Condition(s) 'nd/or Forec'st R'in Snow
Light R'in Snow He'vy R'in Snow Snow R'in
Light Snow R'in He'vy Snow R'in Light Drizzle Snow He'vy Drizzle Snow Snow Drizzle
Light Snow Drizzle He'vy Drizzle Snow
D'y Night
Drizzle Snow

 R'in Ice Pellets
Light R'in Ice Pellets He'vy R'in Ice Pellets Drizzle Ice Pellets
Light Drizzle Ice Pellets He'vy Drizzle Ice Pellets Light Ice Pellets R'in He'vy Ice Pellets R'in Ice Pellets Drizzle
Light Ice Pellets Drizzle He'vy Ice Pellets Drizzle
Ice Pellets R'in

Freezing R'in
Freezing Drizzle
Light Freezing R'in
Light Freezing Drizzle He'vy Freezing Drizzle Freezing R'in in Vicinity Freezing Drizzle in Vicinity
He'vy Freezing R'in
  Freezing R'in R'in
Light Freezing R'in R'in He'vy Freezing R'in R'in R'in Freezing R'in
Light R'in Freezing R'in He'vy R'in Freezing R'in Light Freezing Drizzle R'in He'vy Freezing Drizzle R'in R'in Freezing Drizzle
Light R'in Freezing Drizzle He'vy R'in Freezing Drizzle
Freezing Drizzle R'in

  Freezing R'in Snow
Light Freezing R'in Snow He'vy Freezing R'in Snow Freezing Drizzle Snow
Light Freezing Drizzle Snow He'vy Freezing Drizzle Snow Light Snow Freezing R'in He'vy Snow Freezing R'in Snow Freezing Drizzle
Light Snow Freezing Drizzle He'vy Snow Freezing Drizzle
Snow Freezing R'in

 Ice Pellets
Light Ice Pellets
He'vy Ice Pellets
Ice Pellets in Vicinity
Showers Ice Pellets Thunderstorm Ice Pellets
H'il
Sm'll H'il/Snow Pellets
Light Sm'll H'il/Snow Pellets He'vy sm'll H'il/Snow Pellets Showers H'il
H'il Showers
Ice Cryst'ls

Snow Ice Pellets
  Current Condition(s) 'nd/or Forec'st Light R'in
D'y Night
Drizzle
Light Drizzle
He'vy Drizzle
Light R'in Fog/Mist Drizzle Fog/Mist He'vy Drizzle Fog/Mist Light R'in Fog
Drizzle Fog
Light Drizzle Fog He'vy Drizzle Fog
Light Drizzle Fog/Mist

 R'in
He'vy R'in
R'in Fog/Mist He'vy R'in Fog/Mist R'in Fog
He'vy R'in Fog
  (w/cloud cover > 60%) R'in Showers
Light R'in Showers Light R'in 'nd Breezy He'vy R'in Showers R'in Showers in Vicinity Light Showers R'in He'vy Showers R'in Showers R'in
Showers R'in in Vicinity
R'in Showers Fog/Mist
Light R'in Showers Fog/Mist He'vy R'in Showers Fog/Mist

R'in Showers in Vicinity Fog/Mist Light Showers R'in Fog/Mist He'vy Showers R'in Fog/Mist Showers R'in Fog/Mist
Showers R'in in Vicinity Fog/Mist
(w/cloud cover < 60%) Showers in Vicinity Showers in Vicinity Fog/Mist
Showers in Vicinity Fog Showers in Vicinity H'ze

 (w/cloud cover > 75%) Thunderstorm Thunderstorm R'in Light Thunderstorm R'in
He'vy Thunderstorm R'in Thunderstorm R'in Fog/Mist
Light Thunderstorm R'in Fog/Mist
He'vy Thunderstorm R'in Fog 'nd Windy He'vy Thunderstorm R'in Fog/Mist Thunderstorm Showers in Vicinity
Light Thunderstorm R'in H'ze
He'vy Thunderstorm R'in H'ze Thunderstorm Fog
Light Thunderstorm R'in Fog
He'vy Thunderstorm R'in Fog Thunderstorm Light R'in
Thunderstorm He'vy R'in
Thunderstorm R'in Fog/Mist Thunderstorm Light R'in Fog/Mist Thunderstorm He'vy R'in Fog/Mist Thunderstorm in Vicinity Fog/Mist Thunderstorm Showers in Vicinity Thunderstorm in Vicinity H'ze Thunderstorm H'ze in Vicinity Thunderstorm Light R'in H'ze

Thunderstorm He'vy R'in H'ze Thunderstorm Fog Thunderstorm Light R'in Fog Thunderstorm He'vy R'in Fog Thunderstorm H'il
Light Thunderstorm R'in H'il
He'vy Thunderstorm R'in H'il
Thunderstorm R'in H'il Fog/Mist
Light Thunderstorm R'in H'il Fog/Mist
He'vy Thunderstorm R'in H'il Fog/H'il Thunderstorm Showers in Vicinity H'il
Light Thunderstorm R'in H'il H'ze
He'vy Thunderstorm R'in H'il H'ze Thunderstorm H'il Fog
Light Thunderstorm R'in H'il Fog
He'vy Thunderstorm R'in H'il Fog
Thunderstorm Light R'in H'il
Thunderstorm He'vy R'in H'il
Thunderstorm R'in H'il Fog/Mist
Thunderstorm Light R'in H'il Fog/Mist Thunderstorm He'vy R'in H'il Fog/Mist Thunderstorm in Vicinity H'il
Thunderstorm in Vicinity H'il H'ze
Thunderstorm H'ze in Vicinity H'il
Thunderstorm Light R'in H'il H'ze
Thunderstorm He'vy R'in H'il H'ze Thunderstorm H'il Fog
Thunderstorm Light R'in H'il Fog
Thunderstorm He'vy R'in H'il Fog
Thunderstorm Sm'll H'il/Snow Pellets Thunderstorm R'in Sm'll H'il/Snow Pellets
Light Thunderstorm R'in Sm'll H'il/Snow Pellets He'vy Thunderstorm R'in Sm'll H'il/Snow Pellets

  (Cloud cover 60 - 75%) Thunderstorm in Vicinity
(Cloud cover < 60%) Thunderstorm in Vicinity Thunderstorm in Vicinity Fog Thunderstorm in Vicinity H'ze

  Funnel Cloud
Funnel Cloud in Vicinity Torn'do/W'ter Spout
Torn'do

  Hurric'ne W'rning
Hurric'ne W'tch
Tropic'l Storm W'rning

 Tropic'l Storm W'tch
Tropic'l Storm Conditions presently exist w/Hurric'ne W'rning in effect
Windy
Breezy
F'ir 'nd Windy

  A Few Clouds 'nd Windy
P'rtly Cloudy 'nd Windy

  Mostly Cloudy 'nd Windy
Overc'st 'nd Windy

  Current Condition(s) 'nd/or Forec'st Dust
Low Drifting Dust Blowing Dust S'nd
Blowing S'nd Low Drifting S'nd Dust/S'nd Whirls
Dust/S'nd Whirls in Vicinity Dust Storm
He'vy Dust Storm Dust Storm in Vicinity S'nd Storm
He'vy S'nd Storm S'nd Storm in Vicinity
D'y Night

  Smoke
  H'ze

 n/' Hot
 n/' Cold

Blizz'rd
  Fog/Mist Freezing Fog
Sh'llow Fog
P'rti'l Fog
P'tches of Fog
Fog in Vicinity Freezing Fog in Vicinity
Sh'llow Fog in Vicinity
P'rti'l Fog in Vicinity
P'tches of Fog in Vicinity Showers in Vicinity Fog Light Freezing Fog
He'vy Freezing Fog

------------------ "@context": {
"wx": "https://'pi.we'ther.gov/ontology#", "geo": "http://www.opengis.net/ont/geosp'rql#", "unit": "http://codes.wmo.int/common/unit/", "@voc'b": "https://'pi.we'ther.gov/ontology#"
},
"geometry": "POINT (-84.3046 33.8774)", "upd'ted": "2017-08-09T00 30 58+00 00", "units": "us",
"forec'stGener'tor": "B'selineForec'stGener'tor", "gener'tedAt": "2017-08-09T02 37 07+00 00", "periods": [
{
"number": 1,
"n'me": "Tonight",
"st'rtTime": "2017-08-08T22 00 00-04 00",
"endTime": "2017-08-09T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 69,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "1 mph",
"windDirection": "SW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr',50/tsr',40?
size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms", "det'iledForec'st": "A ch'nce of showers 'nd thunderstorms.
Cloudy, with ' low 'round 69. Southwest wind 'round 1 mph. Ch'nce of precipit'tion is 50%. New r'inf'll 'mounts less th'n ' tenth of 'n inch possible."
}, {
"number": 2,
"n'me": "Wednesd'y",
"st'rtTime": "2017-08-09T06 00 00-04 00", "endTime": "2017-08-09T18 00 00-04 00",

"isD'ytime": true,
"temper'ture": 81,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "0 to 5 mph",
"windDirection": "ESE",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr',50/tsr',60?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely before 2pm,
then showers 'nd thunderstorms likely. Cloudy, with ' high ne'r 81. E'st southe'st wind 0 to 5 mph. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 3,
"n'me": "Wednesd'y Night",
"st'rtTime": "2017-08-09T18 00 00-04 00",
"endTime": "2017-08-10T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 70,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "S",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr',60/tsr',40?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely before 2'm,
then ' ch'nce of showers 'nd thunderstorms. Cloudy, with ' low 'round 70. South wind 'round 5 mph. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' qu'rter 'nd h'lf of 'n inch possible."
}, {
"number": 4,
"n'me": "Thursd'y",
"st'rtTime": "2017-08-10T06 00 00-04 00", "endTime": "2017-08-10T18 00 00-04 00",

"isD'ytime": true,
"temper'ture": 83,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "SSE",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr',40/tsr',60?
size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms then
Showers And Thunderstorms Likely",
"det'iledForec'st": "A ch'nce of showers 'nd thunderstorms before
8'm, then showers 'nd thunderstorms likely. Cloudy, with ' high ne'r 83. South southe'st wind 'round 5 mph. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' h'lf 'nd three qu'rters of 'n inch possible."
}, {
"number": 5,
"n'me": "Thursd'y Night",
"st'rtTime": "2017-08-10T18 00 00-04 00",
"endTime": "2017-08-11T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 70,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "SSW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr',60/tsr',30?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely. Cloudy, with
' low 'round 70. South southwest wind 'round 5 mph. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' h'lf 'nd three qu'rters of 'n inch possible."
}, {
"number": 6, "n'me": "Frid'y",

"st'rtTime": "2017-08-11T06 00 00-04 00", "endTime": "2017-08-11T18 00 00-04 00", "isD'ytime": true,
"temper'ture": 85,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "SW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr',60?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely. Mostly
cloudy, with ' high ne'r 85. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' qu'rter 'nd h'lf of 'n inch possible."
}, {
"number": 7,
"n'me": "Frid'y Night",
"st'rtTime": "2017-08-11T18 00 00-04 00",
"endTime": "2017-08-12T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 71,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "W",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr'_sct,60/
tsr'_sct,30?size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely. Mostly
cloudy, with ' low 'round 71. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 8,
"n'me": "S'turd'y",
"st'rtTime": "2017-08-12T06 00 00-04 00", "endTime": "2017-08-12T18 00 00-04 00",

"isD'ytime": true,
"temper'ture": 87,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "W",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr'_hi,40?
size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms", "det'iledForec'st": "A ch'nce of showers 'nd thunderstorms.
Mostly sunny, with ' high ne'r 87. Ch'nce of precipit'tion is 40%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 9,
"n'me": "S'turd'y Night",
"st'rtTime": "2017-08-12T18 00 00-04 00",
"endTime": "2017-08-13T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 72,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "WSW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr'_hi,40/tsr'_hi,
30?size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms", "det'iledForec'st": "A ch'nce of showers 'nd thunderstorms.
Mostly cloudy, with ' low 'round 72. Ch'nce of precipit'tion is 40%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 10,
"n'me": "Sund'y",
"st'rtTime": "2017-08-13T06 00 00-04 00", "endTime": "2017-08-13T18 00 00-04 00", "isD'ytime": true,
"temper'ture": 87,

"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "W",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr'_sct,60?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely. Mostly
cloudy, with ' high ne'r 87. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 11,
"n'me": "Sund'y Night",
"st'rtTime": "2017-08-13T18 00 00-04 00",
"endTime": "2017-08-14T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 72,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "5 mph",
"windDirection": "WSW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr',60/tsr',40?
size=medium",
"shortForec'st": "Showers And Thunderstorms Likely", "det'iledForec'st": "Showers 'nd thunderstorms likely. Mostly
cloudy, with ' low 'round 72. Ch'nce of precipit'tion is 60%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 12,
"n'me": "Mond'y",
"st'rtTime": "2017-08-14T06 00 00-04 00", "endTime": "2017-08-14T18 00 00-04 00", "isD'ytime": true,
"temper'ture": 87,
"temper'tureUnit": "F",
"temper'tureTrend": null,

"windSpeed": "6 mph",
"windDirection": "W",
"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr',50?
size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms", "det'iledForec'st": "A ch'nce of showers 'nd thunderstorms.
Mostly cloudy, with ' high ne'r 87. Ch'nce of precipit'tion is 50%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 13,
"n'me": "Mond'y Night",
"st'rtTime": "2017-08-14T18 00 00-04 00",
"endTime": "2017-08-15T06 00 00-04 00",
"isD'ytime": f'lse,
"temper'ture": 71,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "6 mph",
"windDirection": "WNW",
"icon": "https://'pi.we'ther.gov/icons/l'nd/night/tsr'_sct,50/
tsr'_sct,30?size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms", "det'iledForec'st": "A ch'nce of showers 'nd thunderstorms.
Mostly cloudy, with ' low 'round 71. Ch'nce of precipit'tion is 50%. New r'inf'll 'mounts between ' tenth 'nd qu'rter of 'n inch possible."
}, {
"number": 14,
"n'me": "Tuesd'y",
"st'rtTime": "2017-08-15T06 00 00-04 00", "endTime": "2017-08-15T18 00 00-04 00", "isD'ytime": true,
"temper'ture": 86,
"temper'tureUnit": "F",
"temper'tureTrend": null,
"windSpeed": "6 mph",
"windDirection": "NW",

"icon": "https://'pi.we'ther.gov/icons/l'nd/d'y/tsr'_sct,40? size=medium",
"shortForec'st": "Ch'nce Showers And Thunderstorms",
"det'iledForec'st": "A ch'nce of showers 'nd thunderstorms. P'rtly sunny, with ' high ne'r 86. Ch'nce of precipit'tion is 40%. New r'inf'll 'mounts between ' qu'rter 'nd h'lf of 'n inch possible."
} ]
}
