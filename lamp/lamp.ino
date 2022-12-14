#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define pinBtn 12 // D6

// WiFi
const char* SSID = "Zeus";
const char* PASSWORD = "chva2008";
WiFiClient wifiClient;

// MQTT server
const char* BROKER = "mqtt.eclipseprojects.io";
int PORT = 1883;

#define ID_MQTT "BCI01" // Client ID
#define TOPIC "BCIButton" // Topic
PubSubClient MQTT(wifiClient);

// Functions prototypes
void keepConnection();
void connectWiFi();
void connectMQTT();
void sendData();

void setup() {
  pinMode(pinBtn, INPUT_PULLUP);

  Serial.begin(115200);

  connectWiFi();
  
  MQTT.setServer(BROKER, PORT);

}

void loop() {
  keepConnection();
  sendData(); // mandar informação apenas se houver mudanças
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

void sendData(){
  static bool stateBtn = HIGH;
  static bool previousBtnState = HIGH;
  static unsigned long debounceBtn;

  stateBtn = digitalRead(pinBtn);
  if((millis() - debounceBtn) > 30){
    if(!stateBtn && previousBtnState){
      // Button pressed
      MQTT.publish(TOPIC, "1");
      Serial.println("Pressed Button.");

      debounceBtn = millis();
    } else if(stateBtn && !previousBtnState) {
      // Button release
      MQTT.publish(TOPIC, "0");
      Serial.println("Released Button.");

      debounceBtn = millis();
    }
  }
  previousBtnState = stateBtn;
}
