#include<DallasTemperature.h>
#include<OneWire.h>
#define Tem 4
//#define Threshold_tem 35

OneWire onewire(Tem);
DallasTemperature Sensors(&onewire);

void setup(){
  Serial.begin(9600);
  Sensors.begin();
}
void loop()
{
  Serial.print("Tem is ");
  float CurrentTemp= Sensors.getTempCByIndex(0);
  Serial.println(CurrentTemp);

  delay(1000);
}
