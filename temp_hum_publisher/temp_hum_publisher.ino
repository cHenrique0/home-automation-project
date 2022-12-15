#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DTH11 2                 // D4 - ESP8266
#define DHT_TYPE DHT11

// DHT instance
DHT dht(DTH11, DHT_TYPE);

// WiFi
const char* SSID = "Zeus";                        // WiFi name
const char* PASSWORD = "chva2008";                // WiFi pass
WiFiClient wifiClient;                            // WiFi client

// MQTT server
const char* BROKER = "mqtt.eclipseprojects.io";   // MQTT host
const int PORT = 1883;                            // MQTT port
#define ID_MQTT "v7s0CVQNkQ"                      // Client ID
#define HUM_TOPIC "home/living_room/humidity"     // Topic published
#define TEMP_TOPIC "home/living_room/temperature" // Topic published
PubSubClient mqttClient(wifiClient);              // MQTT client

// Functions prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void sendData();
double getTemperature();
double getHumidity();

void setup() {
  Serial.begin(115200);
  
  dht.begin();

  connectWiFi();

  mqttClient.setServer(BROKER, PORT);

}

void loop() {
  keepConnection();
  sendData();
  mqttClient.loop();
}

void keepConnection(){
  if(!mqttClient.connected()) {
    connectMQTT();
  }

  connectWiFi();
}

void connectWiFi(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  }

  Serial.printf("Connecting to WiFi: %s\n", SSID);

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(400);
    Serial.print(".");
  }

  Serial.println();
  Serial.printf("Successfully connected to %s\n", SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT(){
  while(!mqttClient.connected()){
    Serial.printf("Connecting to MQTT BROKER: %s\n", BROKER);
    if(mqttClient.connect(ID_MQTT)){
      Serial.println("Successfully connected to Broker!");
    }
    else {
      Serial.println("Unable to connect to Broker");
      Serial.println("Trying to connect again in 10 seconds...");
      delay(10000);
    }
  }
}

void sendData(){
  char msgTemp[10];
  char msgHum[10];

  double temp = getTemperature();
  double hum = getHumidity();

  sprintf(msgTemp, "%f", temp);
  sprintf(msgHum, "%f", hum);

  Serial.printf("Publishing topic: %s\n", TEMP_TOPIC);
  mqttClient.publish(TEMP_TOPIC, msgTemp);
  Serial.printf("Publishing topic: %s\n", TEMP_TOPIC);
  mqttClient.publish(HUM_TOPIC, msgHum);
}

double getTemperature() {
  delay(1000);
  double temperature = dht.readTemperature(); // in °C
  return temperature;
}

double getHumidity() {
  delay(1000);
  double humidity = dht.readHumidity();
  return humidity;
}


