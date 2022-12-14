#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinLamp 2

// WiFi
const char* SSID = "Zeus";
const char* PASSWORD = "chva2008";
WiFiClient wifiClient;

// MQTT Server
const char* BROKER = "mqtt.eclipseprojects.io";
int PORT = 1883;

#define ID_MQTT "BCI02"
#define TOPIC "BCIButton"
PubSubClient MQTT(wifiClient);

// Funtion prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void receiveData(char* topic, byte* payload, unsigned int length);

void setup() {
  pinMode(pinLamp, OUTPUT);

  Serial.begin(115200);

  connectWiFi();
  MQTT.setServer(BROKER, PORT);
  MQTT.setCallback(receiveData);
}

void loop() {
  keepConnection();
  MQTT.loop();
}

void keepConnection(){
  if(!MQTT.connected()) {
    connectMQTT();
  }

  connectWiFi();
}

void connectWiFi(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  }

  Serial.print("Connecting to WiFi: ");
  Serial.print(SSID);

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED){
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Successfully connected to ");
  Serial.print(SSID);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT(){
  while(!MQTT.connected()){
    Serial.print("Connecting to MQTT BROKER: ");
    Serial.println(BROKER);
    if(MQTT.connect(ID_MQTT)){
      Serial.println("Successfully connected to Broker!");
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

  Serial.println(message);

  if(message == "0"){
    digitalWrite(pinLamp, LOW);
  }
  if(message == "1"){
    digitalWrite(pinLamp, HIGH);
  }
}