

// void setup() {
//   // initialize digital pin GPIO18 as an output.
//   pinMode(23, OUTPUT);
// }

// // the loop function runs over and over again forever
// void loop() {
//   digitalWrite(23, HIGH); // turn the LED on
//   delay(1000);             // wait for 500 milliseconds
//   digitalWrite(23, LOW);  // turn the LED off
//   delay(1000);             // wait for 500 milliseconds
// }


#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include "Adafruit_INA219.h"
#include "ThingSpeak.h"


const char *ssid = "SFR_EC58";                 // your network SSID (name)
const char *password = "96wwza4yfz24qhtc4mxq"; // your network password

WiFiClient client;

unsigned long myChannelNumber = 1;
const char *myWriteAPIKey = "QBVKQIKLXLKEVIAY";

Adafruit_INA219 ina219;

// sleep parameter

const long uS_TO_S_FACTOR = 1000000;  /* Conversion factor for micro seconds to seconds */
const int TIME_TO_SLEEP_DAY = 1 * 60; /* Time ESP32 will go to sleep (in seconds) */

void setup() {
  // Open serial communications and wait for port to open:

  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_DAY * uS_TO_S_FACTOR);

  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client); // Initialize ThingSpeak

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }

}

void loop() {

    // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Attempting to connect");
    while (WiFi.status() != WL_CONNECTED)
    {
      WiFi.begin(ssid, password);
      delay(5000);
    }
    Serial.println("\nConnected.");
  }


  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print(busvoltage); Serial.print("\t"); 
  // Serial.print(shuntvoltage); Serial.print("\t");
  // Serial.print(loadvoltage); Serial.print("\t");
  Serial.print(current_mA); Serial.print("\t");
  // Serial.println(power_mW);


  ThingSpeak.setField(1, busvoltage);
  ThingSpeak.setField(2, current_mA);

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (x == 200)
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  delay(250);
  esp_light_sleep_start();

  delay(1000);
}


// #include <Arduino.h>
// #include <WiFi.h>
// #include "ThingSpeak.h"
// #include "DHT.h"

// const char *ssid = "SFR_EC58";                 // your network SSID (name)
// const char *password = "96wwza4yfz24qhtc4mxq"; // your network password

// WiFiClient client;

// unsigned long myChannelNumber = 1;
// const char *myWriteAPIKey = "G8GL9TNBMMD1QBHR";

// // parameter DHT11

// #define DHTPIN 13     // Digital pin connected to the DHT sensor use pins 3, 4, 5, 12, 13 or 14 --
// #define DHTTYPE DHT11 // DHT 11
// DHT dht(DHTPIN, DHTTYPE);

// // Variable to hold temperature readings
// float temperatureC;
// float humidity;

// // sleep parameter

// const long uS_TO_S_FACTOR = 1000000;  /* Conversion factor for micro seconds to seconds */
// const int TIME_TO_SLEEP_DAY = 1 * 60; /* Time ESP32 will go to sleep (in seconds) */

// // battery level

// int GPIO32 = 32;

// float xa = 0;
// float xb = 20;
// float xc = 98;
// float ya = 2425;
// float yb = 2275;
// float yc = 2050;
// float b1 = 2425;
// float b2 = yb - ((yc - yb) / (xc - xb)) * xb;

// uint16_t battery;
// float Battery_Level;

// void setup()
// {
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_DAY * uS_TO_S_FACTOR);

//   Serial.begin(9600); // Initialize serial

//   WiFi.mode(WIFI_STA);

//   ThingSpeak.begin(client); // Initialize ThingSpeak

//   dht.begin();
// }

// void loop()
// {
//   // Connect or reconnect to WiFi
//   if (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print("Attempting to connect");
//     while (WiFi.status() != WL_CONNECTED)
//     {
//       WiFi.begin(ssid, password);
//       delay(5000);
//     }
//     Serial.println("\nConnected.");
//   }

//   // Get a new temperature reading

//   temperatureC = dht.readTemperature();
//   humidity = dht.readHumidity();

//   battery = analogRead(GPIO32);

//   if (battery > yb)
//   {
//     Battery_Level = 100 - ((battery - b1) * ((xb - xa) / (yb - ya)));
//   }

//   if ((battery <= yb) && (battery >= yc))
//   {
//     Battery_Level = 100 - ((battery - b2) * ((xc - xb) / (yc - yb)));
//   }

//   if (battery < yc)
//   {
//     Battery_Level = 0;
//   }

//   Serial.print("Temperature : ");
//   Serial.println(temperatureC);

//   Serial.print("Humidity: ");
//   Serial.println(humidity);

//   Serial.print("Analog read: ");
//   Serial.println(battery);

//   Serial.print("Battery level: ");
//   Serial.println(Battery_Level);

//   ThingSpeak.setField(1, temperatureC);
//   ThingSpeak.setField(2, humidity);
//   ThingSpeak.setField(3, battery);
//   ThingSpeak.setField(4, Battery_Level);

//   // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
//   // pieces of information in a channel.  Here, we write to field 1.

//   int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

//   if (x == 200)
//   {
//     Serial.println("Channel update successful.");
//   }
//   else
//   {
//     Serial.println("Problem updating channel. HTTP error code " + String(x));
//   }

//   WiFi.disconnect(true);
//   WiFi.mode(WIFI_OFF);

//   delay(250);
//   esp_light_sleep_start();
// }
