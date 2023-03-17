#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<HttpClient.h>
#include<WiFiClient.h>
#include<DallasTemperature.h>
#include<OneWire.h>
int pulse=A0; 
int Signal;
int buz=5;
int dallas=4;

OneWire oneWire(dallas);
DallasTemperature sensors(&oneWire);

WiFiClient wificlient;
HTTPClient http;

const char* ssid="Galaxy A53";
const char* password="uprn6050";
unsigned long int last_post=0;
int post_interval=1000;
int count=0;

void setup_wifi()
{
  WiFi.begin(ssid,password);
  while(WiFi.status()!= WL_CONNECTED)
  {
    delay(1000);
    Serial.println("CONNECTING....");
  }
  Serial.println("CONNECTED");
}
void send_data(String A)
{
  if(WiFi.status()==WL_CONNECTED&&(millis()-last_post)>=post_interval)
  {
    http.begin(wificlient,"http://192.168.240.159:5000/test");
    http.addHeader("Content-Type","application/x-www-form-urlencoded");
    String data=A;
    int httpcode=http.POST(data);
    String payload=http.getString();
    Serial.println(httpcode);
    last_post=millis();
    if(httpcode>0)
    {
      String res=http.getString();
      Serial.println(res);
    }
    http.end();
  }
}
void setup()
{
  Serial.begin(9600);
  sensors.begin();
  setup_wifi();
}
void loop()
{
  count++;
  Signal=analogRead(pulse); 
  sensors.requestTemperatures(); 
  Serial.print("Temp is : ");
  float val=sensors.getTempCByIndex(0);
  Serial.println(val);
  Serial.print("Heart Rate= "); 
  Serial.println(Signal);
  String a=String(val)+","+String(Signal);
  if(count>100)
  {
    send_data(a);
    count=0;
  }
}
