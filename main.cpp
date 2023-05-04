#include <Arduino.h>
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <Arduino_MKRIoTCarrier.h>
#include <ArduinoJson.h>

#include "config.h"

WiFiClient wifiClient;
PubSubClient client(wifiClient);
MKRIoTCarrier carrier;  

int wait_time = 5000;

void connectWiFi(){
  while (WiFi.status() != WL_CONNECTED){
    Serial.println("Connecting to WiFi...");
    WiFi.begin(SSID, PASSWORD);
    delay(500);
  }
  Serial.println("Connected to WiFi!");
}

void reconnectMQTTClient(){
  while (!client.connected()){
    Serial.print("Connecting to MQTT");
    if (client.connect(CLIENT_NAME.c_str())){
      Serial.println("Connected!");
      client.subscribe(CLIENT_TELEMETRY_TOPIC.c_str());
    }
    else{
      Serial.print("Failed - Current State: ");
      Serial.println(client.state());    
      delay(wait_time);
    }
  }
}

void clientCallback(char *topic, uint8_t *payload, unsigned int length){
  char buff[length + 1];
  for (int i = 0; i < length; i++){
    buff[i] = (char)payload[i];
  }
  buff[length] = '\0';
  Serial.print("Message Received:");
  Serial.println(buff);
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, buff);
  JsonObject obj = doc.as<JsonObject>();
  float tempReceived = obj["Temp"];
  float humidReceived = obj["Humid"];
  Serial.println("Temp Recieved: " + String(tempReceived) + " Humid Recieved: " + String(humidReceived));
}

void createMQTTClient()
{
    client.setServer(BROKER.c_str(), 1883);
    client.setCallback(clientCallback);
    reconnectMQTTClient();
}

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;

  carrier.noCase();
  carrier.begin();

  connectWiFi();
  createMQTTClient();
}

void loop() {
  reconnectMQTTClient();
  client.loop();

  float temp = carrier.Env.readTemperature();
  float humidity = carrier.Env.readHumidity();

  DynamicJsonDocument doc(1024);
  doc["Temp"] = temp;
  doc["Humid"] = humidity;

  string telemetry;
  serializeJson(doc, telemetry);

  Serial.println("Sending JSON");
  Serial.println(telemetry.c_str());
  client.publish(CLIENT_TELEMETRY_TOPIC.c_str(), telemetry.c_str());
  telemetry.clear();
  delay(wait_time);
}