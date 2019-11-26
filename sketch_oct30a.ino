#include <ESP8266WiFi.h>      // add esp8266 specific wifi
#include <WiFiClient.h>      // Include the Wi-Fi library
#include <ESP8266mDNS.h>      // mdns
#include "DHT.h"              // dht11 code
#include <ThingsBoard.h>      // thingsboard interface library, wraps pubsub

//#define WIFI_AP             "Grant"
//#define WIFI_PASSWORD       "truhands"
#define WIFI_AP             "potato"
#define WIFI_PASSWORD       "iotato123"
#define DHTPIN 4 // D2 pin physical
#define DHTTYPE DHT11 
//#define TOKEN               "nYaLqJLryPOXnHMW4BEp"
#define   TOKEN               "3luklrZmiLBtItexaAga" // taki's demo account key for that device
#define THINGSBOARD_SERVER  "demo.thingsboard.io" 
DHT dht(DHTPIN, DHTTYPE); // create dht11 object?

WiFiClient espClient; // create wifi
// Initialize ThingsBoard instance
ThingsBoard tb(espClient); // create thingsboard client passing in wifi connection
// the Wifi radio's status
int status = WL_IDLE_STATUS; // set wifi status tracker to idle

void setup() {
  Serial.begin(115200); // begin Serial Connection
  delay(10);

  WiFi.begin(WIFI_AP, WIFI_PASSWORD); // start wifi connection to wifi ap with password
  InitWiFi();
  Serial.println('\n');
  Serial.println("Connected to " + WiFi.SSID()); // print useful info
  Serial.println("IP address is" + WiFi.localIP());
  // if mDNS is ok do if statement
  if (MDNS.begin("iot")) { // create mdns broadcast
    Serial.println("MDNS responder started");
    // MDNS.addService("esp", "tcp", 8080); //add esp service on 8080 via tcp
  }
  else {
    Serial.println("Error in MDNS responder starting");
  }
  dht.begin(); // start dht11 read
}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}
// utility function to reconnect wifi
void reconnect() {
  // Loop until we're reconnected
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    WiFi.begin(WIFI_AP, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected to AP");
  }
}

void loop() {
  delay(1000); //delay 1 sec
  if (WiFi.status() != WL_CONNECTED) {
    reconnect();
  }
  MDNS.update(); // update mdns

  if (!tb.connected()) { // if thingsboard connection is dead
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) { // if connection with token to url fails
      Serial.println("Failed to connect");
      return;
    }
  }

  Serial.println("Sending data...");

  float h = dht.readHumidity(); // read humidity as float
  float t = dht.readTemperature(); // read temp as float
  float q = analogRead(A0); // read Analog pin A0 for MQ data in float
  tb.sendTelemetryFloat("temperature", t); //send temp
  tb.sendTelemetryFloat("humidity", h); // send humidity
  tb.sendTelemetryFloat("Air Quality Val", q); // send quality

  tb.loop();
}
