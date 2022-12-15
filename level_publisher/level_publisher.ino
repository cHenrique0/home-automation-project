#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinMaxLevel 5                             // Pin D1
#define pinMinLevel 4                             // Pin D2
bool maxLevel, minLevel;

// WiFi
const char* SSID = "Zeus";                        // WiFi name
const char* PASSWORD = "chva2008";                // WiFi pass
WiFiClient wifiClient;                            // WiFi client

// MQTT server
const char* BROKER = "mqtt.eclipseprojects.io";   // MQTT host
const int PORT = 1883;                            // MQTT port
#define ID_MQTT "nehlZUXd2i"                      // Client ID
#define TOPIC "home/backyard/tank"                 // Topic published
PubSubClient mqttClient(wifiClient);              // MQTT client

// Functions prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void sendData();

void setup() {
  pinMode(pinMaxLevel, INPUT_PULLUP);
  pinMode(pinMinLevel, INPUT_PULLUP);

  Serial.begin(115200);

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
  maxLevel = digitalRead(pinMaxLevel);
  minLevel = digitalRead(pinMinLevel);

  if(maxLevel && minLevel) {
    Serial.println("The tank is at maximum level!");
    // Serial.printf("Publishing topic: %s\n", TOPIC);
    mqttClient.publish(TOPIC, "MAX");
  }
  else if(maxLevel == 0 && minLevel) {
    Serial.println("The tank is at minimum level!");
    // Serial.printf("Publishing topic: %s\n", TOPIC);
    mqttClient.publish(TOPIC, "MIN");
  }
  else if (maxLevel == 0 && minLevel == 0) {
    Serial.println("Alert! The tank is empty!");
    // Serial.printf("Publishing topic: %s\n", TOPIC);
    mqttClient.publish(TOPIC, "EMPTY");
  }
}