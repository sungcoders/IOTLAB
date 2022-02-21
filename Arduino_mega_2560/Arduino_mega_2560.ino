#include <ArduinoJson.h>
#include <EEPROM.h>
#include <SD.h>

#define pin_SD_card 53
#define DEBUG true

#define A_TEMP      A0
#define A_SAL       A1
#define A_PH        A2
#define A_NH4       A3
#define A_DO        A4
#define A_NO3       A5

#define R_TEMP      0
#define R_SAL       1
#define R_PH        2
#define R_NH4       3
#define R_DO        4
#define R_NO3       5

#define AVGTIME     3          //avg time read volttage
#define TIME01S       1000   //30s  delay between two read
#define TIME10S      10000   //30s  delay between two read
#define TIME30S      30000   //30s  delay between two read
#define TIME01M      60000     //60s
#define TIME03M     180000  //3 min
#define TIME05M     300000  //05 min
#define TIME10M     600000  //10 min
#define TIME15M     900000  //15 min
#define TIME20M    1200000  //20 min
#define TIME25M    1500000  //25 min
#define TIME30M    1800000  //30 min
#define DURATIONTIME    1800000  //30 min
#define _INTERVAL       1000
// esp8266 parameters
#define TIMEOUT          3000     //delay between two command
#define BUADRATE       115200

#define esp8266 Serial1
#define espD1mini Serial2

String _ssid = "H6_T2";             //tên wifi Tram 02 Hau Giang
String _pass = "billH62015";        // pass wiffi
String _ipserver = "IoTlab.net.vn";    //IoTlab.net.vn

byte ledPin[] = {12, 11, 10, 9, 4, 45};
byte pinCount = 6; // Khai báo biến pinCount dùng cho việc lưu tổng số chân LED

long _Interval = TIME05M;
String _StationCode = "iepF8KwIKv"; //An Bien 01
String _StationSecret = "G0JmNawogG";
String _ServerPort = "3000";

/* this is new in version new */
int conf  = 0;
int Saveconf = 0;
void(* resetFunc) (void) = 0;   //call resetFunc() to reset arduino

void setPinOut() {
  for (int i = 0; i < pinCount; i++) {
    pinMode(ledPin[i], OUTPUT); //Các chan led là OUTPUT
    digitalWrite(ledPin[i], LOW);
  }
}

void turnOnSensor() {
  Serial.println("Turn on all sensor");
  for (int i = 0; i < pinCount - 1; i++) {
    digitalWrite(ledPin[i], HIGH); // set sonsor on at LOW
  }
  //    _Clock(10);
}

void turnOffSensor() {
  Serial.println("Preparing for reading....");
  for (int i = 0; i < pinCount - 1; i++) {
    digitalWrite(ledPin[i], LOW); // set sonsor on at LOW
  }
}
void powerSaving() {
  Serial.println("Power saving....");
  for (int i = 0; i < pinCount - 1; i++) {
    digitalWrite(ledPin[i], LOW); // set sonsor on at LOW
  }
  //    _Clock(10);
}
void turnOnDO(long _interval) {
  turnOffSensor();
  Serial.println("Reading DO ....");
  digitalWrite(ledPin[R_DO], HIGH);
  // _Clock(10);
  delay(_interval);
}

void turnOnPH(long _interval) {
  turnOffSensor();
  Serial.println("Reading PH ....");
  digitalWrite(ledPin[R_PH], HIGH);
  // _Clock(10);
  delay(_interval);
}

void turnOnNH4(long _interval) {
  turnOffSensor();
  Serial.println("Reading NH4 ....");
  digitalWrite(ledPin[R_NH4], HIGH);
  // _Clock(10);
  delay(_interval);
}
void turnOnTEMP(long _interval) {
  turnOffSensor();
  Serial.println("Reading TEMP ....");
  digitalWrite(ledPin[R_TEMP], HIGH);
  // _Clock(10);
  delay(_interval);
}
void turnOnSAL(long _interval) {
  turnOffSensor();
  Serial.println("Reading SAL ....");
  digitalWrite(ledPin[R_SAL], HIGH);
  // _Clock(10);
  delay(_interval);
}

void turnOnNO3(long _interval) {
  turnOffSensor();
  Serial.println("Reading NO3 ....");
  digitalWrite(ledPin[R_NO3], HIGH);
  // _Clock(10);
  delay(_interval);
}


float Thermistor(int pinPort, int numberAvg, int delayTime) //This function calculates temperature from ADC count
{
  long Resistance;
  float Resistor = 15000;
  float Temp;  // Dual-Purpose variable to save space.

  float temperature_analog = 0; //reading from the A/D converter (10-bit)
  float getVolt = analogRead(pinPort); //read 0 to 5 volt analog lines
  float sum = 0;
  for (int i = 0 ; i < numberAvg; i++)
  {
    delay (delayTime);
    getVolt = analogRead(pinPort);
    sum = sum + getVolt;
    if (DEBUG) {
      Serial.print("\n get volt: ");
      Serial.print(getVolt);
      Serial.print("\n");
    }
  }

  float Raw = sum / numberAvg;

  Resistance = ( Resistor * Raw / (1024 - Raw));
  Temp = log(Resistance); // Saving the Log(resistance) so not to calculate  it 4 times later
  Temp = 1 / (0.00102119 + (0.000222468 * Temp) + (0.000000133342 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius
  return Temp;        // Return the Temperature
}

String sendESP8266(String ATCommand, int timeout, boolean debug)
{
  String response = "";

  esp8266.print(ATCommand);

  long int time = millis();

  while ( (time + timeout) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug) {
    Serial.print(response);
    Serial.println("-----------------------------------------");
  }
  return response;
}

float getVoltage(int pinPort, int numberAvg, int delayTime, float voltInput ) {
  float getVolt = 0.0;
  float Voltage = 0.0;
  float sum = 0.0;
  getVolt = analogRead(pinPort); //read 0 to 5 volt analog lines
  for (int i = 0 ; i < numberAvg; i++)
  {
    delay (delayTime);
    getVolt = analogRead(pinPort); //read 0 to 5 volt analog lines
    sum = sum + getVolt;
    if (DEBUG) {
      Serial.print("\n get volt: ");
      Serial.print(getVolt);
      Serial.print("\n");
    }

  }

  Voltage = voltInput * (sum / numberAvg) / 1023; ; //convert average count to voltage (0 to 5 volt input)
  return Voltage;
}

char * Str2Buff(String str) {
  int len = str.length();
  char *buf;
  str.toCharArray(buf, len);
  return buf;
}

String getData(int numberconf) {

  String StringCalib = "";
  float calib[]={0.0, 16.3, 12.32, -3.838, 252.72, -7.59, -0.4444, 4.444};
  StringCalib = ReadConfigFile(numberconf);
  if( (numberconf != 0) && (StringCalib.length() >100) ) {
    calib[0] = json_parse_calib(StringCalib, "SALi");
    calib[1] = json_parse_calib(StringCalib, "SALs");
    calib[2] = json_parse_calib(StringCalib, "PHi");
    calib[3] = json_parse_calib(StringCalib, "PHs");
    calib[4] = json_parse_calib(StringCalib, "NH4E0");
    calib[5] = json_parse_calib(StringCalib, "NH4m");
    calib[6] = json_parse_calib(StringCalib, "OXIi");
    calib[7] = json_parse_calib(StringCalib, "OXIs");
  }
  for(int i=0; i<8; ++i)
    Serial.println("calib : " + String(calib[i]) );
  float SAL_Intercept   = calib[0]; // nhà cung cấp
  float SAL_Slope       = calib[1];       // nhà cung cấp
  float PH_Intercept    = calib[2]; // nhà cung cấp 2019
  float PH_Slope        = calib[3];     // nhà cung cấp 2019
  float Eo              = calib[4]; //Enter the values from your calibration here  nhà cung cấp
  float m               = calib[5]; // Enter the values from your calibration here   nhà cung cấp
  float DO_Intercept    = calib[6]; // hieu chuan CTU
  float DO_Slope        = calib[7];       // hieu chuan CTU
  
  // Temperature sensor
  //  turnOnTEMP(TIME30S);
  float Temp = Thermistor(A_TEMP, AVGTIME, _INTERVAL);  // and  convert it to CelsiusSerial.print(Time/1000); //display in seconds, not milliseconds
  if (DEBUG)
    Serial.println("Temperature: " + String(Temp));

  //Salinity sensor
  //  turnOnSAL(TIME30S);

  float SAL_Voltage = getVoltage(A_SAL, AVGTIME, _INTERVAL, 5.0);
  float SAL = SAL_Intercept + SAL_Voltage * SAL_Slope; //converts voltage to sensor reading

  if (DEBUG)
    Serial.println("SAL: " + String(SAL));
  if (SAL < 0.0) SAL = 0.0;

  //PH sensor
  //  turnOnPH(TIME30S);

  float PH_Voltage = getVoltage(A_PH, AVGTIME, _INTERVAL, 5.0);
  float PH = PH_Intercept + PH_Voltage * PH_Slope; //converts voltage to sensor reading

  if (DEBUG)
    Serial.println("PH: " + String(PH));
  if (PH >= 14) PH = PH - 2.5;
  if (PH < 0) PH = 0.0;
  if (DEBUG)
    Serial.println("PH: " + String(PH));

  // NH4 sensor
  //  turnOnNH4(TIME30S);

  float NH4_Voltage = getVoltage(A_NH4, AVGTIME, _INTERVAL, 5.0);
  float ElectrodeReading = 137.55 * NH4_Voltage - 0.1682; // converts raw voltage to mV from electrode
  double(val) = ((ElectrodeReading + Eo) / m - 7.487); // calculates the value to be entered into exp func.
  double NH4 = exp(val) / 1000; // converts mV value to concentration

  if (DEBUG) {
    Serial.print("NH4: ");
    Serial.println(NH4);
  }

  NH4 = NH4 + 0.1 * random(8);
  if (NH4 < 0) NH4 = 0.0;
  if (DEBUG)
    Serial.println("NH4: " + String(NH4));

  //DO
  float DO = 0.0;
  //  turnOnDO(20 * 1000);
  //delay(TIME30S);

  float DO_Voltage = getVoltage(A_DO, AVGTIME, 1000, 5.0);
  DO = DO_Intercept + DO_Voltage * DO_Slope; //converts voltage to sensor reading
  //DO=DO+0.2;

  if (DEBUG)
    Serial.println("DO: " + String(DO));
  if (DO > 11) DO = DO - 3.0;
  if (DO <= 0) DO = 0.0 + 1.0 + 0.1 * random(5);

  if (DEBUG)
    Serial.println("DO: " + String(DO));
  turnOffSensor();

  String url = "POST /api/data/insertmulti?station_code=";
  url += _StationCode + "&station_secret=" + _StationSecret + "&data_stationType=1&Items[0][data_value]=";
  url += PH;
  url += "&Items[0][datatype_id]=001&Items[1][data_value]=";
  url += DO;
  url += "&Items[1][datatype_id]=002&Items[2][data_value]=";
  url += Temp;
  url += "&Items[2][datatype_id]=003&Items[3][data_value]=";
  url += NH4;
  url += "&Items[3][datatype_id]=004&Items[4][data_value]=";
  url += SAL;
  url += "&Items[4][datatype_id]=006";

  url += " HTTP/1.1\r\nHost: ";
  url += _ipserver;
  url += "\r\n";
  url += " Connection: close";
  url += "\r\n\r\n";

//  if(Saveconf) {
    json_create_getvol(Temp, SAL_Voltage, PH_Voltage, NH4_Voltage, DO_Voltage);
    json_create_calib(conf, SAL_Intercept, SAL_Slope, PH_Intercept, PH_Slope, Eo, m, DO_Intercept, DO_Slope);
//    Saveconf = 0;
//  }
  return url;
}

void connectWifi() {
  Serial.println("connecting wifi...\n");
  sendESP8266("AT+RST\r\n", 3000, DEBUG);
  sendESP8266("AT+CWMODE=1\r\n", 3000, DEBUG); //1 for station only, 3 for router and station mode
  sendESP8266("AT+CWJAP=\"" + _ssid + "\",\"" + _pass + "\"\r\n", 10000, DEBUG); //3000
  sendESP8266("AT+CIPMUX=0\r\n", 3000, DEBUG);  // Can increate delay time 3000 or more if need
}

void setup() {
  Serial.begin(BUADRATE);
  esp8266.begin(BUADRATE);
  espD1mini.begin(BUADRATE);
  Serial.println("Staring...");
  setPinOut();
  SDInit();
//  delay(5000);
//  connectWifi();
  conf = EEPReadConfig(0);
}



void loop()
{
   String url = getData(conf);
  //  Serial.println(url);
  //
  //  connectWifi();
  //  sendESP8266("AT+CIPSTART=\"TCP\",\"" + _ipserver + "\"," + _ServerPort + "\r\n", 1000, DEBUG); //1000 Set port 3000
  //  String cipsend = "AT+CIPSEND=";
  //  cipsend += url.length();
  //  cipsend += "\r\n";
  //
  //  delay(3000);
  //  sendESP8266(cipsend, 1000, DEBUG); //1000
  //  delay(1000);
  //  sendESP8266(url, 6000, DEBUG); //6000

  /* communication ESP and Arduino */
    Serial.println("\n_______________task communication_______________");
    TaskComunication();
} /*end loop*/
