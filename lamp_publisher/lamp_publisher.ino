#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinSwitch 12                              // Pin D6

// WiFi
const char* SSID = "Zeus";                        // WiFi name
const char* PASSWORD = "chva2008";                // WiFi pass
WiFiClient wifiClient;                            // WiFi client

// MQTT server
const char* BROKER = "mqtt.eclipseprojects.io";   // MQTT host
const int PORT = 1883;                            // MQTT port
#define ID_MQTT "nehlZUXd2i"                      // Client ID
#define TOPIC "home/bedroom/lamp"                 // Topic published
PubSubClient mqttClient(wifiClient);              // MQTT client

// Functions prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void sendData();
bool getSwitchState();

void setup() {
  pinMode(pinSwitch, INPUT_PULLUP);

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

int sendData(){
  if(getSwitchState()) {
    Serial.printf("Lamp ON! Publishing topic: %s\n", TOPIC);
    mqttClient.publish(TOPIC, "ON");
  } else {
    Serial.printf("Lamp OFF! Publishing topic: %s\n", TOPIC);
    mqttClient.publish(TOPIC, "OFF");
  }
  /* static bool stateBtn = HIGH;
  static bool previousStateBtn = HIGH;
  static unsigned long debounceBtn;

  stateBtn = digitalRead(pinSwitch);
  if((millis() - debounceBtn) > 30){
    if(!stateBtn && previousStateBtn){
      // Button pressed
      Serial.printf("Pressed Button! Publishing topic: %s\n", TOPIC);
      mqttClient.publish(TOPIC, "ON");

      debounceBtn = millis();
    } else if(stateBtn && !previousStateBtn) {
      // Button release
      Serial.printf("Released Button! Publishing topic: %s\n", TOPIC);
      mqttClient.publish(TOPIC, "OFF");

      debounceBtn = millis();
    }
  }
  previousStateBtn = stateBtn; */
}

bool getSwitchState(){
  const int debounce = 50;
  static bool switchState;
  static bool previousSwitchState;
  static bool saveSwitchState = true;
  static unsigned long switchDelay = 0;

  if( (millis() - switchDelay) > debounce ) {
    switchState = digitalRead(pinSwitch);
    if( switchState && (switchState != previousSwitchState) ) {
      saveSwitchState = !saveSwitchState;
      switchDelay = millis();
    }
    previousSwitchState = switchState;
  }

  return saveSwitchState;
}
















