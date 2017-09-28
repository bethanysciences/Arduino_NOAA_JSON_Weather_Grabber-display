#include "RTCZero.h"
#include "SimpleTimer.h"
#include "WiFi101.h"
#include "ArduinoJson.h"

RTCZero rtc;
int UTCOffset     = -4;
char timeZone[]   = "EDT";
char ampm[2][3]   = {"am", "pm"};
#define Time24    false
#define MIN       60000                  // 1 minute microseconds
SimpleTimer timer;

const char ssid[]     = "iotworld";
const char pass[]     = "iotworld";
int status  = WL_IDLE_STATUS;

WiFiSSLClient client;
char server[]       = "api.weather.gov";
char locForecast[]  = "/points/33.8774,-84.3046";
char prodForecast[] = "/forecast";
char locCurrent[]   = "/stations/KPDK";
char prodCurrent[]  = "/observations/current";
const unsigned long HTTPTimeout = 10000;


void convertTime(int utcHour, int utcDay, int *locHour, int *locDay, int *AorP) {
  *AorP = 0;
  *locHour = utcHour + UTCOffset;
  if (*locHour < 0) {
    *locHour += 24;
    *locDay = utcDay - 1;
  }
  else if (*locHour > 23) {
    *locHour -= 24;
    *locDay = utcDay - 1;
  }
  if (*locHour >= 12) {
    *AorP = 1;
  }
  if (!Time24) {
    if (*locHour > 12) *locHour -= 12;
    else if (*locHour == 0) *locHour += 12;
  }
}
double c2f(double celsius) {
    return ((celsius * 9) / 5) + 32;
}
double pa2hg(double pascals) {
    return (pascals * 0.000295333727);
}
char *dtostrf (double val, signed char width, unsigned char prec, char *sout) {
  // convert floats to fixed string
  // val      Your float variable;
  // width    Length of the string that will be created INCLUDING decimal point;
  // prec     Number of digits after the deimal point to print;
  // sout     Destination of output buffer
    char fmt[20];
    sprintf(fmt, "%%%d.%df", width, prec);
    sprintf(sout, fmt, val);
    return sout;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  
  // ------------------ WIFI SETUP --------------------- //
  Serial.print("connect to >");       Serial.print(ssid);
  while (status != WL_CONNECTED) {status = WiFi.begin(ssid, pass); Serial.print(">"); delay(500);}
  Serial.print("> connected ");       Serial.println();
  
  
  digitalWrite(LED_BUILTIN, HIGH);
  
  
  // ------------------ SET CLOCK --------------------- //
  rtc.begin();
  unsigned long epoch;
  Serial.print("set time > ");                  Serial.print(epoch);
  while (epoch == 0) { epoch = WiFi.getTime();  Serial.print(">"); delay(500); }
  Serial.print(" NTP epoch acquired ");         Serial.print(epoch);
  rtc.setEpoch(epoch);
  Serial.print(" > time (GMT)"); Serial.print(rtc.getHours()); 
    Serial.print(":"); Serial.println(rtc.getMinutes()); 


  // ------------------ SET TIMERS --------------------- //  
  timer.setInterval(MIN * 1, getCurrent);
  Serial.print("getCurrent interval "); Serial.print((MIN * 1) / 1000); 
    Serial.println(" secs");


  // ------------------ HEADER --------------------- // 
  Serial.print("current");  Serial.print(",");       
  Serial.print("stamp");    Serial.print(",");
  Serial.print("temp");     Serial.print(",");
  Serial.print("rHum");     Serial.print(",");   
  Serial.print("htIx");     Serial.print(",");
  Serial.print("pressure"); Serial.print(",");
  Serial.println("  wind"); 

  getCurrent(); 
  
}

void loop() {
  timer.run();
}

void getCurrent() {
  fetchNOAA(server, locCurrent, prodCurrent);
}
void fetchNOAA(const char* server, const char* location, const char* product) {
  if (connectServer(server)) {
    if (sendRequest(location, product) && skipResponseHeaders()) {
      if (product == prodCurrent) parseCurrent();
    }
    client.stop();
  }
}
bool connectServer(const char* hostName) { bool ok = client.connectSSL(hostName, 443); return ok;}
bool sendRequest(const char* location, const char* product) {
  client.print("GET ");
  client.print(location);
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
bool parseCurrent() {
  const size_t bufferSize = JSON_ARRAY_SIZE(0) + JSON_ARRAY_SIZE(1) + 
                          6*JSON_OBJECT_SIZE(1) + 6*JSON_OBJECT_SIZE(2) + 
                          16*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(16) + 
                          JSON_OBJECT_SIZE(28) + 2410;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  JsonObject& root = jsonBuffer.parseObject(client);
  if (!root.success()) { Serial.print("parse fail"); return false; }
  JsonObject& _context           = root["@context"];
  JsonObject& temperature        = root["temperature"];
  JsonObject& dewpoint           = root["dewpoint"];
  JsonObject& windDirection      = root["windDirection"];
  JsonObject& windSpeed          = root["windSpeed"];
  JsonObject& barometricPressure = root["barometricPressure"];
  JsonObject& seaLevelPressure   = root["seaLevelPressure"];
  JsonObject& relativeHumidity   = root["relativeHumidity"];
  JsonObject& heatIndex          = root["heatIndex"];

  const char* timestamp = root["timestamp"];
  const char* text      = root["textDescription"];
  const char* icon      = root["icon"];
  int temp    = temperature["value"];
  float dwpt  = dewpoint["value"];
  int wDir    = windDirection["value"];
  float wind  = windSpeed["value"];
  double baro = barometricPressure["value"];
  double sLvl = seaLevelPressure["value"];
  float rHum  = relativeHumidity["value"];
  float htIx  = heatIndex["value"];

  String ageString = String(timestamp);
  int firstDash    = ageString.indexOf("-");
  int secondDash   = ageString.lastIndexOf("-");
  int firstT       = ageString.indexOf("T");
  int firstColon   = ageString.indexOf(":");
  int secondColon  = ageString.lastIndexOf(":");
  
  String yearStr   = ageString.substring(0, firstDash);
  String monthStr  = ageString.substring(firstDash + 1);
  String dayStr    = ageString.substring(secondDash + 1);
  String hourStr   = ageString.substring(firstT + 1);
  String minuteStr = ageString.substring(firstColon + 1);

  int cyear        = yearStr.toInt();
  int cmonth       = monthStr.toInt();
  int cday         = dayStr.toInt();
  int chour        = hourStr.toInt();
  int cminute      = minuteStr.toInt();

  int clocHour, clocDay, cAorP;
  convertTime(chour, cday, &clocHour, &clocDay, &cAorP);
  char agePretty[50];
  sprintf(agePretty, "%d:%02d%s", clocHour, cminute, ampm[cAorP]);

  temp = c2f(temp);
  dwpt = c2f(dwpt);
  htIx = c2f(htIx);
  baro = pa2hg(baro);
  
  char temp_d[7]; dtostrf(temp, 3, 0, temp_d);
  char dwpt_d[7]; dtostrf(dwpt, 3, 0, dwpt_d);
  char wind_d[7]; dtostrf(wind, 3, 0, wind_d);
  char baro_d[9]; dtostrf(baro, 2, 2, baro_d);
  char sLvl_d[9]; dtostrf(sLvl, 3, 2, sLvl_d);
  char rHum_d[7]; dtostrf(rHum, 3, 0, rHum_d);
  char htIx_d[7]; dtostrf(htIx, 3, 0, htIx_d);

  char textStr[80]; sprintf(textStr, "Currently %s", text);
  char tempStr[40]; sprintf(tempStr, "%s",  temp_d);  
  char rHumStr[40]; sprintf(rHumStr, "%s%%",  rHum_d);
  char dwptStr[40]; sprintf(dwptStr, "%s°F",   dwpt_d);
  char htIxStr[40]; sprintf(htIxStr, "%s°F",   htIx_d);
  char baroStr[40]; sprintf(baroStr, "%sin", baro_d);
  char windStr[40]; sprintf(windStr, "%smph",  wind_d);


  // ------------------ LOCAL TIME --------------------- //
  int locHour, locDay, AorP;
  convertTime(rtc.getHours(), rtc.getDay(), &locHour, &locDay,  &AorP);    
  char curTime[50];
  sprintf(curTime, "%d:%02d%s", locHour, rtc.getMinutes(), ampm[AorP]);


  // ------------------ DEBUG --------------------- //
  Serial.print(curTime);    Serial.print(",");          
  Serial.print(agePretty);  Serial.print(",");
  Serial.print(tempStr);    Serial.print(",");
  Serial.print(rHumStr);    Serial.print(",");   
  Serial.print(htIxStr);    Serial.print(",");  
  Serial.print(baroStr);    Serial.print(",");   
  Serial.println(windStr);
  

  return true;
}

