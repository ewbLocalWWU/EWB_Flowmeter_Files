#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#define USERNAME "ewbLocalWWU"
#define DEVICE_ID "esp8366"
#define DEVICE_CREDENTIAL "Gowiththeflow"
#define SSID "Verizon-RC400L-CE" // TODO: Update based on wifi network 
#define SSID_PASSWORD "689ebe0e" // TODO: Update based on wifi network 
#define SENSOR  D4
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
// EWB editions (in progress)
const int buttonPin = 2;
int buttonState = 0;

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}
void setup()
{
  Serial.begin(115200);
  thing.add_wifi(SSID, SSID_PASSWORD);
  pinMode(SENSOR, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;
  attachInterrupt(digitalPinToInterrupt(SENSOR), pulseCounter, FALLING);
  pinMode(buttonPin, INPUT);
}
void loop()
{
  buttonState = digitalRead(buttonPin);
  currentMillis = millis();
  if (buttonState == LOW) {
    // TODO: Reset on button press
    // totalMilliLitres = 0;
  }

  if (currentMillis - previousMillis > interval) {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space
    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
    thing["data"] >> [](pson& out){
    out["Flow Rate"] = flowRate;
    out["Total"]= totalMilliLitres;
     };
    thing.handle();
    thing.stream(thing["data"]);
  }
}