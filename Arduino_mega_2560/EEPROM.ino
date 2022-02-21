
int EPPRead(int address)
{
  int value = EEPROM.read(address);
  delay(50);
//  Serial.println("gia tri doc eep: "+ String(value));
  return value;
}

void EPPWrite(int address, int value)
{
  /* An EEPROM write takes 3.3 ms to complete - The EEPROM memory has a specified life of 100,000 write/erase cycles */
  EEPROM.write(address, value);
//  Serial.println("gia tri ghi eep: "+ String(value));
  delay(50);
}

void EEPConfig(int ConfigSave)
{
  int EEPCount = 0;
  EPPWrite(0, ConfigSave);
  while(EPPRead(0) != ConfigSave)
  {
    delay(200);
    ++EEPCount;
    EPPWrite(0, ConfigSave);
    delay(50);
    if(EPPRead(0) == ConfigSave)
      break;
    if(EEPCount == 5){
      Serial.println("EEPROM(write) FAIL");
      break;
    }
  }
  return ConfigSave;
}

int EEPReadConfig(int address)
{
  int EEPReadconf = 0;
  delay(200);
  EEPReadconf = EPPRead(0);
  Serial.println(EEPReadconf);
  if(EEPReadconf >=0 && EEPReadconf <6)
    return EEPReadconf;
  else Serial.println("fail to read config from EEPROM");
}
