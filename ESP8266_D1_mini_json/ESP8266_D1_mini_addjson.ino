/*
 * pubsubclient version 2.8.0 by Nic O'leary 
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <stdint.h>
/*buffer data pubish*/
#define Buffer 240

#define endln Serial.println();
#define show Serial.print
#define showln Serial.println
#define SendA Serial.write

#define ssid_WiFi  "SUNG_LEE"                     //  "Nha Tro Di 3 2.4G"
#define pass_WiFi  "123456789"             //  "123456789"
#define mqtt_server  "192.168.10.113"

WiFiClient espClient;
PubSubClient client(espClient);

int8_t boolread = 0;
boolean boolwrite = 0;
boolean boolconfig = 0;
char ch_readdata[Buffer];
char ch_writedata[Buffer];
int numberconfig = 0;

uint32_t NowTime = 0;

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, 0);
  Serial.begin(115200);
  setup_wifi();
  /* setup Server */
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  /* setup arduino json */
  
}

void loop()
{
  if (!client.connected())
    reconnectMQTT();
  client.loop();
  
  SerialRead();
}

String ReadWait(unsigned int seconds)
{
  seconds = seconds*1000;
  String StringData = "";
  unsigned long Time = millis();
  while(millis() - Time <= seconds)
  {
    while(Serial.available())
    {
      StringData = Serial.readString();
      delay(100);
      if(!Serial.available())
        Time = seconds;
    }
  }
  return StringData;
}

void SerialRead()
{
  String datacalib = "";
  String datagetvol = "";
  String DataBuffer = "";
  while(Serial.available())
  {
    DataBuffer = Serial.readString();
    delay(50);
    if(DataBuffer.length() <= 100 && DataBuffer.length() >= 40)
      DataPublish("MQTT_voltage", DataBuffer);
    if(DataBuffer.length() > 100)
      DataPublish("MQTT_calib", DataBuffer);
//    if(!Serial.available())
//      show(DataBuffer);
  }
  if(DataBuffer == "readdata") {
    if(boolread) {
      delay(50);
      Serial.print("request_readdata");
      DataBuffer = "";
      boolread = 0;
    }
  }
  if(DataBuffer == "writedata") {
    if(boolwrite) {
      delay(100);
      Serial.print(ch_writedata);
      DataBuffer = "";
      boolwrite = 0;
    }
  }
  if(DataBuffer == "configdata") {
    if(boolconfig) {
    delay(50);
    Serial.print(numberconfig);
    DataBuffer = "";
    numberconfig = 0;
    boolconfig = 0;
    }
  }
}

/* -----------------------Program Function-----------------------*/

void setup_wifi()
{
  digitalWrite(BUILTIN_LED, 0);
  delay(10);
//  show("Connecting to " + String(ssid_WiFi));
//  endln

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_WiFi, pass_WiFi);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    show(".");
  }

//  showln("\nWiFi Connected \nIP address: ");
//  showln(WiFi.localIP());
}

void DataPublish(char topic[], String DataUser)
{
  uint8_t value = 0;
  char msg[Buffer];
  char payload[DataUser.length()+1];
  for(int i=0; i<= DataUser.length(); ++i)
    payload[i] = DataUser[i];
  snprintf(msg, Buffer, payload);
  boolean boolPub = client.publish(topic, msg);
//  if(!boolPub)
    //showln("mistake to send data to server.");
}

void callback(char* topic, byte* payload, unsigned int length)
{
  String mess_payload = "";
  String topic_convert = String(topic);
  char msg_process[length+1];
  /* message for MQTT*/
  //show("Message [" + topic_convert + "]");
  for (int i = 0; i < length; i++)
    mess_payload += (char)payload[i];
  //endln
  //showln(mess_payload);

//  json_parse_data(msg_process);
  if(topic_convert == "MQTT_read_data_sub")
  {
    boolread = 2;
    for(int j=0; j<=length; j++)
      ch_readdata[j] = mess_payload[j];
    client.publish("mqtt_status", "request read");
  }
  if(topic_convert == "MQTT_write_data_sub")
  {
    boolwrite = 1;
    for(int j=0; j<=length; j++)
      ch_writedata[j] = mess_payload[j];
    client.publish("mqtt_status", "request write");
  }
  if(topic_convert == "MQTT_click_config_sub")
  {
    boolconfig = 1;
    numberconfig = mess_payload.toInt();
    client.publish("mqtt_status", "request show configuration");
  }
  delay(100);
} /* end callback */

void reconnectMQTT()
{
  while (!client.connected())
  {
//    show("Connecting to MQTT Server...");
    if(WiFi.status() != WL_CONNECTED)
      setup_wifi();
    if(client.connect("ESP"))
    {
      digitalWrite(BUILTIN_LED, 1);
//      showln("connected MQTT");
      client.publish("mqtt_status", "ESP connected");
      client.subscribe("MQTT_click_config_sub");
      client.subscribe("MQTT_write_data_sub");
      client.subscribe("MQTT_read_data_sub");
    }
    else
    {
//      showln("failed, rc = " + String(client.state()) + " try again in 5 seconds");
      delay(5000);
    }
  } /* end while */
} /* end reconnect MQTT*/
