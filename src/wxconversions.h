/*----------------------------------------------------------------------*
  Weather conversion calculations
  for avr-libc ATMEL series 32bit SAMD21 CPUs

  Celc > Fahr double c2f(double [temp °celcius])
                returns (double [temp °fahrenheit])
  Fahr > Celc double f2c(double [temp °fahrenheit])
                retuns (double [temp °celcius])
  Humidity    double rh(double [dew point °celcius], double [temp °celcius])
                returns (double [% rel humidity])
  Windchill   float wc(double [temp °celcius], int [MPH windspeed]}
                returns (float [windchill °celcius])
  PA -> "HG   double p2h(double pascals)
                returns double [presure in inches mercury])
  Dew Point   double dp(double [temp °celcius or °fahrenheit],
                        int [% rel humidity])
                returns double [dew point °celcius or °fahrenheit]
  Heat Index  double hi(double [temp °celcius or °fahrenheit],
                        int [% rel humidity], bool [°celcius input ?])
                returns heat index double [temp °celcius or °fahrenheit]
  MED         double med(int [uvindex], int [altitude meters], bool [on water ?],
                         bool [on snow ?], int [fitz skin type], int [spf applied])
                returns int [mins] to Min Erythemal Dose (MED) - sunburn

  © 2017 Bob Smith https://github.com/bethanysciences
  MIT license
 *----------------------------------------------------------------------*/

// ------------------------------------- temperature celcius to fahrenheit
double c2f(double c2f_tempC) {
  return ((c2f_tempC * 9) / 5) + 32;
}

// ------------------------------------- temperature fahrenheit to celcius
double f2c(double f2c_tempF) {
  return ((f2c_tempF * 9) / 5) + 32;
}

// ------------------ dew point celcius, temperature celcius to humidity %
double rh(double rh_dewPointC, double rh_tempC) {
  return (100 * (exp((17.271 * rh_dewPointC) /
         (237.7 + rh_dewPointC))) / (exp((17.271 * rh_tempC) /
         (237.7 + rh_tempC))) + 0.5);
}

// --------------- windspeed MPH, temperature celcius to windchill celcius
float wc(int wc_tempC, int wc_windspeed) {
  float result;
  wc_windspeed = wc_windspeed * 1.852;      // convert to KPH
  result = 13.12 + 0.6215 * wc_tempC - 11.37 *
    pow(wc_windspeed,0.16) + 0.3965
    * wc_tempC * pow(wc_windspeed,0.16);
  if (result < 0 ) {
    return result - 0.5;
  }
  else {
    return result + 0.5;
  }
}

// -------------------------------------- barometer pascals to in. mercury
double p2h(double p2h_pascals) {
    return (p2h_pascals * 0.000295333727);
}

// ----------------------------------- temperature, humididty to dew point
double dp(double dp_temp, int dp_humd) {
    double temp = (237.7 * ((17.271 * dp_temp) /
                (237.7 + dp_temp) +
                log(dp_humd * 0.01))) /
                (17.271 - ((17.271 * dp_temp) /
                (237.7 + dp_temp) +
                log(dp_humd * 0.01)));
    return dp_temp;
}

// ---------- temperature, humididty to heat index (celcius or fahrenheit)
double hi(double hi_temp, double hi_humd, bool hi_celcius) {
  if (!hi_celcius) {                           // test if temp in Celcius
    if (hi_temp < 26 || hi_humd < 40) {         // test out of limits (c)
      return hi_temp;
    }
  }
  else if (hi_temp < 80 || hi_humd < 40) {      // test out of limits (f)
    return hi_temp;
  }
  double c1=-42.38, c2=2.049;
  double c3=10.14, c4=-0.2248;
  double c5=-6.838e-3, c6=-5.482e-2;
  double c7=1.228e-3, c8=8.528e-4;
  double c9=-1.99e-6;
  double t = hi_temp;
  double r = hi_humd;
  double a = ((c5 * t) + c2) * t + c1;
  double b = ((c7 * t) + c4) * t + c3;
  double c = ((c9 * t) + c8) * t + c6;
  double rv = (c * r + b) * r + a;
  return rv;
}

// ------------------------------- minutes to Minimal Erythemal Dose (MED)
double med(int m_uvindex, int m_alt, bool m_h20,
           bool m_snow, int m_fitz, int m_spf) {
  double uvi_f =                        // factor UV index reading
        (m_uvindex * (m_alt * 1.2)) +   // altitude factor (1.2 x MSL)
        (m_uvindex * (m_h20 * 1.5)) +   // water factor (1.5x)
        (m_uvindex * (m_snow * 1.85));  // snow factor (1.85x)

  double s2med_b =
        (-3.209E-5 * pow(m_fitz, 5)) +   // Fitz score @ 1 UV index
        (2.959E-3 * pow(m_fitz, 4)) -  // 5th order polynomial plot
        (0.103 * pow(m_fitz, 3)) +
        (1.664* pow(m_fitz, 2)) +
        (3.82 * m_fitz) +
         34.755;

  double s2med = ((s2med_b / uvi_f) * m_spf);   // seconds to MED
  int m2med = s2med / 60;                 // convert secs to mins
  if (m2med > 480) m2med = 480;        // max at 6 hrs (480 mins)
  return m2med;
}
