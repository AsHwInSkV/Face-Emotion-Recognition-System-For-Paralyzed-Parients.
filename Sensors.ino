#include<DallasTemperature.h>
#include<OneWire.h>

int Tem=4;
int pulse=A0;

OneWire onewire(Tem);
DallasTemperature Sensors(&onewire);

void setup() {
  Serial.begin(9600);
  Sensors.begin();
}

void loop() {
  Sensors.requestTemperatures();
  int Signal=analogRead(pulse);
  Serial.print("Heart Rate : ");
  Serial.println(Signal);
  Serial.print("Tem is ");
  Serial.println(Sensors.getTempCByIndex(0));
  delay(1000);  
}
