#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinLamp 2 // D4

// WiFi
const char* SSID = "Zeus";
const char* PASSWORD = "chva2008";
WiFiClient wifiClient;

// MQTT Server
const char* BROKER = "mqtt.eclipseprojects.io";
int PORT = 1883;

#define ID_MQTT "MGxomm2esd"
#define BADROOM_TOPIC "home/bedroom/lamp"
#define TANK_TOPIC "home/tank/level"
#define GARDEN_TOPIC "home/garden/moisture"
PubSubClient mqttClient(wifiClient);

// Funtion prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void receiveData(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinLamp, OUTPUT);

  Serial.begin(115200);

  connectWiFi();

  mqttClient.setServer(BROKER, PORT);
  mqttClient.setCallback(receiveData);
}

void loop() {
  keepConnection();
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
      Serial.printf("Subscribed to topic: '%s'\n", BADROOM_TOPIC);
      mqttClient.subscribe(BADROOM_TOPIC);
    }
    else {
      Serial.println("Unable to connect to Broker");
      Serial.println("Trying to connect again in 10 seconds...");
      delay(10000);
    }
  }
}

void receiveData(char* topic, byte* payload, unsigned int length){
  String message;

  // getting the payload string
  for(int i = 0; i < length; i++){
    char c = (char)payload[i];
    message += c;
  }

  if(message == "OFF"){
    digitalWrite(pinLamp, HIGH);
  }
  if(message == "ON"){
    digitalWrite(pinLamp, LOW);
  }
  if(message == "MAX") {
    Serial.println("TANK MAX");
  }
  if(message == "MIN") {
    Serial.println("TANK MIN");
  }
  if(message == "EMPTY") {
    Serial.println("TANK EMPTY");
  }
}