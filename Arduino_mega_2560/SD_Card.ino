
String ReadFileSD( char NameFile[] )
{
  String ContentFile = "";
  File ReadFile;

  ReadFile = SD.open(NameFile);
  if(ReadFile)
  {
    while(ReadFile.available())
      ContentFile += char(ReadFile.read());
    ReadFile.close();
    Serial.println("Read(file): "+ ContentFile);
  }
  else
    Serial.println("read(file)ERROR.");
  return ContentFile;
}

void WriteFileSD( char NameFile[], String WriteContent )
{
  File WriteFile;
  char charWriteContent[WriteContent.length()+1];
  WriteFile = SD.open(NameFile, FILE_WRITE);
  if(WriteFile)
  {
    Serial.print("Write(file): ");
    for(int i=0; i< WriteContent.length()+1; ++i)
      charWriteContent[i] = WriteContent[i];
    Serial.println(charWriteContent);
    uint32_t WriteData = WriteFile.write(charWriteContent);
    if(WriteData > 0)
      Serial.println("Write(file) SUCCESS.");
    else
      Serial.println("Write(file) FAIL.");
    WriteFile.close();
  }
  else
    Serial.println("error write file.");
  return ;
}

boolean RemoveFile( char NameFile[] )
{
  int boolRemove = -1;
  if(SD.exists(NameFile))
  {
    boolRemove = SD.remove(NameFile);
    if(boolRemove)
      Serial.println("Remove file SUCCESS.");
    else Serial.println("Remove file FAIL.");
  }
}

void SDInit()
{
  int reconnect = 0;
  do
  {
    if(SD.begin(pin_SD_card))
      Serial.println("_________initialization SD CARD done___________");
    else
      Serial.println("_________initialization SD CARD failed!_________");
    reconnect++;
    if(reconnect >= 20) resetFunc();
  }while(!SD.begin(pin_SD_card));
}

int SaveConfigFile(int configsave, String datasave)
{
  switch(configsave)
  {
    case 1:
      RemoveFile("/config/config1.txt");
      WriteFileSD("/config/config1.txt", datasave);
      break;
    case 2:
      RemoveFile("/config/config2.txt");
      WriteFileSD("/config/config2.txt", datasave);
      break;
    case 3:
      RemoveFile("/config/config3.txt");
      WriteFileSD("/config/config3.txt", datasave);
      break;
    case 4:
      RemoveFile("/config/config4.txt");
      WriteFileSD("/config/config4.txt", datasave);
      break;
    case 5:
      RemoveFile("/config/config5.txt");
      WriteFileSD("/config/config5.txt", datasave);
      break;
    default:
     Serial.println("no task is save");
     break;
  }
}

String ReadConfigFile(int configsave)
{
  String dataget = "";
  switch(configsave)
  {
    case 1:
      dataget = ReadFileSD("/config/config1.txt");
      break;
    case 2:
      dataget = ReadFileSD("/config/config2.txt");
      break;
    case 3:
      dataget = ReadFileSD("/config/config3.txt");
      break;
    case 4:
      dataget = ReadFileSD("/config/config4.txt");
      break;
    case 5:
      dataget = ReadFileSD("/config/config5.txt");
      break;
    default:
     Serial.println("no task read at here");
     break;
  }
  return dataget;
}
