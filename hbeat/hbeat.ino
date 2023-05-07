#include<ESP8266WiFi.h>
#include<ESP8266HTTPClient.h>
#include<HttpClient.h>
#include<WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define N 10
#define ONE_WIRE_BUS 5
#define stat_led 16



WiFiClient wificlient;
HTTPClient http;
OneWire onewire(ONE_WIRE_BUS); //Creates a OneWire instance
DallasTemperature tempSensor(&onewire); //Pass OneWire reference to Dallas Temperature.

volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile boolean Pulse = false;
volatile boolean QS = false;

volatile int Rate[N];
volatile unsigned long CurrBeatTime = 0;
volatile unsigned long LastBeatTime = 0;
volatile int P = 500;
volatile int T = 500;
volatile int Threshold = 512;
volatile int Amplifier = 100;

int PulseSensorPin = 17;
int FadePin = 4;
int FadeRate = 0;

const char* ssid = "OnePlus Nord";
const char* password = "498898@@";

unsigned long int last_post = 0;
int post_interval = 1000;
int count = 0;

void setupTimer(int m /* msec */) {
  timer0_isr_init();
  timer0_attachInterrupt(timer0_ISR);
  timer0_write(ESP.getCycleCount() + 80000L * m); // 80MHz/1000 == 1msec
}
void setup() {
  Serial.begin(9600);
  setup_wifi();
  tempSensor.begin(); //Start temp sensor
  pinMode(FadePin, OUTPUT);
  analogWriteRange(255);

  pinMode(stat_led, OUTPUT);

  noInterrupts();
  setupTimer(10);

  interrupts();
  LastBeatTime = getCurrentTime(); // msec

}

void setup_wifi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("CONNECTING....");
    delay(1000);
  }
  Serial.println("CONNECTED");
}

void send_data(String A)
{
  if (WiFi.status() == WL_CONNECTED && (millis() - last_post) >= post_interval)
  {
    http.begin(wificlient, "http://192.168.205.159:5000/test");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    String data = A;
    int httpcode = http.POST(data);
    String payload = http.getString();
    Serial.println(httpcode);
    last_post = millis();
    if (httpcode > 0)
    {
      String res = http.getString();
      Serial.println(res);
    }
    http.end();
  }
}

void timer0_ISR (void) {
  noInterrupts();
  Signal = system_adc_read();
  CurrBeatTime = getCurrentTime(); // msec
  unsigned long interval = CurrBeatTime - LastBeatTime;

  // hold bottom
  if ((Signal < Threshold) && (interval > (IBI * 3) / 5)) {
    if (Signal < T) {
      T = Signal;
      //Serial.println("T:" + String(T));
    }
  }

  // hold peak
  if (Signal > Threshold && Signal > P) {
    P = Signal;
    //Serial.println("P:" + String(P));
  }

  if (interval > 250 /* ms */) {

    // check if Signal is over Threshold
    if ((Signal > Threshold) && !Pulse && (interval > (IBI * 3) / 5)) {
      Pulse = true;
      IBI = interval;

      if (Rate[0] < 0) { // first time
        Rate[0] = 0;
        LastBeatTime = getCurrentTime();
        setupTimer(10);
        noInterrupts();
        return;
      } else if (Rate[0] == 0) {  // second time
        for (int i = 0; i < N; ++i) {
          Rate[i] = IBI;
        }
      }

      word running_total = 0;
      for (int i = 0; i < N - 1; ++i) {
        Rate[i] = Rate[i + 1];
        running_total += Rate[i];
      }

      Rate[N - 1] = IBI;
      running_total += IBI;
      running_total /= N;
      BPM = 60000 / running_total;
      QS = true;
      LastBeatTime = getCurrentTime();
    }
  }

  // check if Signal is under Threshold
  if ((Signal < Threshold) && Pulse) {
    Pulse = false;
    Amplifier = P - T;
    Threshold = Amplifier / 2 + T; // revise Threshold
    P = Threshold;
    T = Threshold;
  }

  // check if no Signal is over 2.5 sec
  if (interval > 2500 /* ms */) {
    Threshold = 512;
    P = 500;
    T = 500;
    LastBeatTime = getCurrentTime();
    for (int i = 0; i < N; ++i) {
      Rate[i] = -1;
    }
  }
  setupTimer(10);
  interrupts();
}


unsigned long getCurrentTime() {
  return ESP.getCycleCount() / 80000L;
}


void loop() {
  count++;
  // Serial.println(count);
  if (count == 10) {
    digitalWrite(stat_led, HIGH);
  }
  if (count == 20) {
    count = 0;
    digitalWrite(stat_led, LOW);
  }
  tempSensor.requestTemperatures();
  int myTEMP = tempSensor.getTempCByIndex(0); // Calls function on our tempSensor object that returns temp as an int.
  if (QS) {
    FadeRate = 255;
    if (BPM > 125) {
      BPM = 130;
    }
    else if (BPM < 60) {
      BPM = 60;
    }
    else {
      BPM = BPM;
    }
    //    Serial.print("BPM: ");
    //    Serial.println(BPM);
    //
    //    Serial.print("BPM: ");
    //    Serial.println(BPM);
    //    Serial.print("Temp: ");
    //    Serial.println(myTEMP);
    QS = false;
    //noInterrupts();
    String fval=String(BPM) + "," + String(myTEMP) + ",";
    send_data(fval);
    //delay(100);
    //interrupts();

  }

  FadeRate -= 15;
  FadeRate = constrain(FadeRate, 0, 255);
  analogWrite(FadePin, FadeRate);
  delay(20);
}
