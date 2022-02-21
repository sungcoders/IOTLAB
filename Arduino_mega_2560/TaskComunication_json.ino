/*
 * JSON version 6.19.0 by Beniot Blanchon
 * communication with ESP
 * 3 work at here {read data, write data, config}
*/
String espD1miniReadBuffer(unsigned int Time)
{
  String StringBuffer = "";
  unsigned long Timecharbuffer = millis();
  while(millis() - Timecharbuffer <= Time)
  {
    while(espD1mini.available())
      StringBuffer += char(espD1mini.read());
//    if(!espD1mini.available())
//      Timecharbuffer = Time;
  }
  return StringBuffer;
}

float json_parse_calib(String StringRead, String GET)
{
  /* setup for arduino json _ payload of callback is json example from server */
  char jsondata[StringRead.length()];
  for(int i=0; i<= StringRead.length(); ++i)
    jsondata[i] = StringRead[i];
  StaticJsonDocument <256> doc;
  deserializeJson(doc, jsondata);

  int ConfigSave = doc["config"];
  float SAL_intercept = doc["SAL_Inte"];
  float SAL_slope = doc["SAL_Slo"];
  float PH_intercept = doc["PH_Inte"];
  float PH_slope = doc["Ph_Slo"];
  float NH4_E0 = doc["NH4_E0"];
  float NH4_m = doc["NH4_m"];
  float OXI_intercept = doc["OXI_Inter"];
  float OXI_slope = doc["OXI_Slo"];

  if(GET == "config")
    return ConfigSave;
  if(GET == "SALi")
    return SAL_intercept;
  if(GET == "SALs")
    return SAL_slope;
  if(GET == "PHi")
    return PH_intercept;
  if(GET == "PHs")
    return PH_slope;
  if(GET == "NH4E0")
    return NH4_E0;
  if(GET == "NH4m")
    return NH4_m;
  if(GET == "OXIi")
    return OXI_intercept;
  if(GET == "OXIs")
    return OXI_slope;
}

String json_parse_getvol(String jsonstring)
{
/* setup for arduino json _ payload of callback is json example from server */
  char jsondata[240];
  for(int i=0; i <= jsonstring.length(); ++i)
    jsondata[i] = jsonstring[i];
  StaticJsonDocument <256> doc;
  deserializeJson(doc, jsondata);
  float TEMP_getvol = doc["TEMP_getvol"];
  float SAL_getvol = doc["SAL_getvol"];
  float PH_getvol = doc["PH_getvol"];
  float NH4_getvol = doc["NH4_getvol"];
  float OXI_getvol = doc["OXI_getvol"];
  
  Serial.println("TEMP_getvol: " + String(TEMP_getvol));
  Serial.println("SAL_getvol: " + String(SAL_getvol));
  Serial.println("PH_getvol: " + String(PH_getvol));
  Serial.println("NH4_getvol: " + String(NH4_getvol));
  Serial.println("OXI_getvol: " + String(OXI_getvol));

/* show json to String output in Serial */
}

String json_create_getvol(float temp, float sal, float ph, float nh4, float oxi)
{
  String StringRead = ReadFileSD("/config/getvol.txt");
  char jsondata[StringRead.length()];
  for(int i=0; i<= StringRead.length(); ++i)
    jsondata[i] = StringRead[i];
    
  StaticJsonDocument <256> doc;
  deserializeJson(doc, jsondata);
  doc["TEMP_getvol"] = float(temp);
  doc["SAL_getvol"] = int(sal*1024/5);
  doc["PH_getvol"] = int(ph*1024/5);
  doc["NH4_getvol"] = int(nh4*1024/5);
  doc["OXI_getvol"] = int(oxi*1024/5);

/* show json to String output in Serial */
  String output;
  serializeJson(doc, output);
  Serial.println("output voltage: "+output);
  delay(500);
  RemoveFile("/config/sgetvol.txt");
  WriteFileSD("/config/sgetvol.txt", output);
  return output;
}

String json_create_calib(int conf, float sal_inter, float sal_slope, float ph_inter, float ph_slope, float nh4_e0, float nh4_m, float oxi_inter, float oxi_slope)
{
  String StringRead = ReadFileSD("/config/calib.txt");
  char jsondata[StringRead.length()];
  for(int i=0; i<= StringRead.length(); ++i)
    jsondata[i] = StringRead[i];
    
  StaticJsonDocument <256> doc;
  deserializeJson(doc, jsondata);
  doc["config"] = conf;
  doc["SAL_Inte"] = sal_inter;
  doc["SAL_Slo"] = sal_slope;
  doc["PH_Inte"] = ph_inter;
  doc["Ph_Slo"] = ph_slope;
  doc["NH4_E0"] = nh4_e0;
  doc["NH4_m"] = nh4_m;
  doc["OXI_Inter"] = oxi_inter;
  doc["OXI_Slo"] = oxi_slope;
  
/* show json to String output in Serial */
  String output;
  serializeJson(doc, output);
  Serial.println("output calib: "+ output);
  RemoveFile("/config/scalib.txt");
  WriteFileSD("/config/scalib.txt", output);
  return output;
}

void TaskComunication()
{
  delay(2000);
  /* use to debug when Serial of ESP8266 D1 mini turn on */
//  charReadBuffer(3000);
  String DataBuffer = "";
  int ReadRequest = 0;

  Serial.print("send read data request:");
  espD1mini.print("readdata");
  delay(100);
  DataBuffer = espD1miniReadBuffer(6000);
  if(DataBuffer == "request_readdata") {
    Serial.print("du lieu nhan dc: ");  Serial.println(DataBuffer);
    DataBuffer = "";
    ReadRequest = 2;
    Saveconf = 1;
  }
  delay(5000);
  if(ReadRequest == 2)
  {
    String ReadSvoltage = ReadFileSD("/config/sgetvol.txt");
    char ReadSvoltage1[ReadSvoltage.length()+1];
    for(int i=0; i<= ReadSvoltage.length(); ++i)
      ReadSvoltage1[i] = ReadSvoltage[i];
    espD1mini.write(ReadSvoltage1);
    Serial.println("getvoltage is sent" + String(ReadSvoltage.length()) );
      delay(10000);
    String ReadScalib = ReadFileSD("/config/scalib.txt");
    char ReadScalib1[ReadScalib.length()+1];
    for(int i=0; i<= ReadScalib.length(); ++i)
      ReadScalib1[i] = ReadScalib[i];
    espD1mini.write(ReadScalib1);
    Serial.println("ReadScalib is sent" + String(ReadScalib.length()) );
  }

  delay(5000);
  Serial.print("send write data request:");
  espD1mini.print("writedata");
  delay(100);
  DataBuffer = espD1miniReadBuffer(6000);
  if(DataBuffer != "") {
    RemoveFile("/config/tconf.txt");
    WriteFileSD("/config/tconf.txt", DataBuffer);
//    Serial.print("du lieu nhan dc: ");  Serial.println(DataBuffer);
    DataBuffer = "";
    DataBuffer = ReadFileSD("/config/tconf.txt");
    int dataconfig = json_parse_calib(DataBuffer, "config");
//    Serial.print("dataconfig = "); Serial.println(dataconfig);
    SaveConfigFile(dataconfig, DataBuffer);
    Serial.println("saved file config to SD CARD = " + String(dataconfig) );

    EEPConfig(dataconfig);
    if(dataconfig != 0) {
      Serial.println("saved config to EEPROM and RESET TO APPLY = " + String(dataconfig));
      delay(1000);
      resetFunc();
    }
    else Serial.println("can not use config is 0");
  }
  else Serial.println("no task is requested");


  Serial.print("send config data request:");
  espD1mini.print("configdata");
  delay(100);
  DataBuffer = espD1miniReadBuffer(6000);
    int dataconfig1 = DataBuffer.toInt();
//    Serial.print("du lieu nhan dc: ");  Serial.println(DataBuffer);
//    Serial.print("so lieu chuyen doi: "); Serial.println(dataconfig1);
    DataBuffer = "";

    if(dataconfig1)
    {
      String ReadFileConfig = "";
      ReadFileConfig = ReadConfigFile(dataconfig1);
      char ReadFileConfig1[ReadFileConfig.length()+1];
      for(int i=0; i<= ReadFileConfig.length(); ++i)
        ReadFileConfig1[i] = ReadFileConfig[i];
      espD1mini.write(ReadFileConfig1);
      delay(5000);
    }
} /* end taskcomunication */
