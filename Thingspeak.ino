#include <ESP8266WiFi.h>
#include <DHT.h>
#include <ThingSpeak.h>
#include <SPI.h>
#include <Wire.h>
#include "MQ135.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     // Reset pin # (or -1 if sharing Arduino reset pin)

#define DHTTYPE DHT22  // DHT 11
#define DHTPIN 15

#define relay_fan 5
#define relay_light 16
#define relay_fridge 4
#define relay_oven 0

#define buzzer_alarm 12
#define pir_human 13

int alarm_status;
int pir_status = 0;

DHT dht(15, DHT22);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //Baad me initialize karna

WiFiClient client;

long myChannelNumber = 2136364;
long myChannelNumber1 = 2136561;
long myChannelNumber2 = 2136570;
const char myWriteAPIKey[] = "8D132M3CY1BNVL51";  //to send data on thingspeak//
const char myWriteAPIKey1[] = "0L7Q51H92T4NV4GV";
const char myWriteAPIKey2[] = "KZU3HVNL73QB12PO";
//to send data on thingspeak

void setup() {
  // put your setup code here, to run once:
  Serial.println("HI");
  Serial.begin(115200);  //Baud Rate
  WiFi.begin("vivoY73", "12345678");
  dht.begin();
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  //Blynk.begin(auth, ssid, pass);
  ThingSpeak.begin(client);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.println("..");
  }
  Serial.println();
  Serial.println("NodeMCU is connected!");
  Serial.println(WiFi.localIP());
  delay(100);

  pinMode(pir_human, INPUT);
  pinMode(buzzer_alarm, OUTPUT);

  pinMode(relay_fan, OUTPUT);
  pinMode(relay_light, OUTPUT);
  pinMode(relay_fridge, OUTPUT);
  pinMode(relay_oven, OUTPUT);

  digitalWrite(buzzer_alarm, LOW);
  digitalWrite(relay_fan, HIGH);
  digitalWrite(relay_light, HIGH);
  digitalWrite(relay_fridge, HIGH);
  digitalWrite(relay_oven, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  MQ135 gasSensor = MQ135(A0);

  float air_quality = gasSensor.getPPM();
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  pir_status = digitalRead(pir_human);
  alarm_status = digitalRead(buzzer_alarm);

  if (pir_status == 1) {
    Serial.println("Person Detected");
  } else if (pir_status == 0) {
    Serial.println("No One in Room");
  }

  if (air_quality > 66000) {
    digitalWrite(buzzer_alarm, HIGH);  //buzzer is on node open
    digitalWrite(relay_fan, LOW);      //relay is on node close
    Serial.println("Buzzer Status: ON");
    Serial.println("Exhaust Fan: ON");
  } else {
    digitalWrite(buzzer_alarm, LOW);
    digitalWrite(relay_fan, HIGH);
    Serial.println("Buzzer Status: OFF");
    Serial.println("Exhaust Fan: OFF");
  }

  Serial.print("Air Quality: ");
  Serial.print(air_quality);
  Serial.println(" PPM");

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C");

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.println(" %");

  Serial.println();
  Serial.println("****************************");
  Serial.println();


  Serial.println("Temperature: " + (String)t);
  Serial.println("Humidity: " + (String)h);
  ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
  //delay(1000);
  ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
  // delay(1000);
  ThingSpeak.writeField(myChannelNumber, 3, air_quality, myWriteAPIKey);
  delay(1000);
}