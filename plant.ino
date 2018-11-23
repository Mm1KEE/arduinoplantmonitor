#define BLINKER_PRINT Serial
#define BLINKER_MQTT
#define BLINKER_DEBUG_ALL
#include <DHTesp.h>
#include <Blinker.h>
#include <SimpleTimer.h>

DHTesp dht;
SimpleTimer timer;
BlinkerNumber Number1("n1");
BlinkerNumber Number2("n2");
BlinkerNumber Number3("n3");
BlinkerText Text1("t1");
BlinkerSlider Slider1("s1");
BlinkerButton Button1("b1");
BlinkerButton Button2("b2");
TempAndHumidity newValues;
uint32_t startTime;
int8_t h, m, s;
int moisture = 0;
int waterTime = 5;
bool relay = false;
char auth[] = "          ";
//char auth[] = "           ";
char ssid[] = "          ";
char pswd[] = "          ";
int dhtPin = 2;
int adPin = A0;
int relayPin = D3;


bool getTemperature() {
  // Reading temperature for humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
  newValues = dht.getTempAndHumidity();
  // Check if any reads failed and exit early (to try again).
  if (dht.getStatus() != 0) {
    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
    return false;
  }

  //Serial.println(" T:" + String(newValues.temperature) + " H:" + String(newValues.humidity));
  return true;
}

void returnStatus()
{
  getTemperature();
  getAD();
  getTime();
  if (Blinker.connected()) {
    Blinker.beginFormat();
    //if()
    Number1.print(newValues.temperature);
    Number2.print(newValues.humidity);
    Number3.print(moisture);
    Text1.print(String(h) + ":" + String(m) + ":" + String(s), "刷新时间");
    Slider1.print(waterTime);
    relayControl();
    Blinker.endFormat();
  }
}

void getTime()
{
  h = Blinker.hour();
  m = Blinker.minute();
  s = Blinker.second();
}

void getAD()
{
  int ad = analogRead(adPin);
  //Serial.println(ad);
  moisture = map(ad, 0, 1024, 0, 100);
}

void relayControl()
{
  if (relay)
  {
    if ((millis() / 1000 - startTime/1000) >= waterTime)
    {
      relayOFF();
    }
    else
    {
      relayON();
    }
  }
  else relayOFF();
}

void relayON()
{
  digitalWrite(relayPin, HIGH);
  relay = true;
  //Button1.icon("toggle-on");
  Button1.color("#FF0000");
  Button1.text("打开");
  Button1.print("relay on");
}

void relayOFF()
{
  digitalWrite(relayPin, LOW);
  relay = false;
  //Button1.icon("toggle-off");
  Button1.color("#FFFFFF");
  Button1.text("关闭");
  Button1.print("relay off");
}
void slider1_callback(int32_t value)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG2("get slider value: ", value);
  waterTime = value;
}

void button1_callback(const String & state)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG2("get button state: ", state);
  if (!relay)
  {
    startTime = millis();
    relayON();
  }
  else
  {
    relayOFF();
  }

}

void button2_callback(const String & state)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  BLINKER_LOG2("get button 2 state: ", state);
  returnStatus();
}

void setup() {
  Serial.begin(115200);
  byte resultValue = 0;
  // Initialize temperature sensor
  dht.setup(dhtPin, DHTesp::DHT11);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(relayPin, LOW);
  Blinker.begin(auth, ssid, pswd);
  //Blinker.attachHeartbeat(returnStatus);
  timer.setInterval(5000, returnStatus);
  //timer.setInterval(1000, relayControl);
  Slider1.attach(slider1_callback);
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
}

void loop()
{
  Blinker.run();
  timer.run();
  if (Blinker.available()) {
    BLINKER_LOG2("Blinker.readString(): ", Blinker.readString());
    /*uint32_t BlinkerTime = millis();
      Blinker.beginFormat();
      Blinker.vibrate();
      Blinker.print("millis", BlinkerTime);
      Blinker.endFormat();*/
  }

  //Blinker.delay(5000);
  //returnStatus();
}


