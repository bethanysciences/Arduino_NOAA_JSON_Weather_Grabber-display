/*----------------------------------------------------------------------*
  Fetch NOAA Forecasts and return JSON-LD
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

char server[]   = "api.weather.gov";
int port        = 443;
char location[] = "/points/33.8774,-84.3046"; // station KPDK
char product[]  = "/forecast";

void fetchNOAA() {
  if (connectServer(server)) {
    if (sendRequest(location, product) && skipResponseHeaders()) {
      client.stop();
      return;
    }
  }
}

bool connectServer(const char* hostName) {
  bool ok = client.connectSSL(hostName, port);
  return ok;
}

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
