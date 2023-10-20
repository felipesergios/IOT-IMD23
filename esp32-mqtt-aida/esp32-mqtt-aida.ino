#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h>
#include "DHT.h"
#include <iostream>
#include <string>

WiFiClient ESPWiFiClient;
PubSubClient mqttClient(ESPWiFiClient);

//char* wifi_ssid = "Rede do enriquinho";
//char* wifi_password = "enriquinho123";
// char* wifi_ssid = "motorolah";
// char* wifi_password = "87654321";
char* wifi_ssid = "brisa-1783640";
char* wifi_password = "ruaw6waq";
int wifi_timeout = 100000;

const char* mqtt_broker = "io.adafruit.com"; // trocar pelo endereço do ainda fruit
const char* mqtt_client_username = "enrico_luigi"; // add username
const char* mqtt_client_password = "aio_Vdoh58azR72xJVGnM1KbwIUoz4UZ"; //  add senha
const int mqtt_port = 1883;
int mqtt_timeout = 10000;

int led = 2;

#define DHTPIN 4
#define DHTTYPE DHT11

#define HUMIDITY_INDEX 0
#define TEMPERATURE_INDEX 1

#define TOPIC_PUBLISH_TEMP "enrico_luigi/feeds/teste_envio_temp"
#define TOPIC_PUBLISH_HUMIDITY "enrico_luigi/feeds/teste_conexao_esp32_mqttbroker"

// array to store sensor data
float data[2];

// DHT object to use the DHT libray api
DHT dht(DHTPIN, DHTTYPE);

void readSensorDataAndPublish(){
  readSensorData();
  mqttClient.publish(TOPIC_PUBLISH_HUMIDITY, String(data[HUMIDITY_INDEX], 2).c_str(), true);
  mqttClient.publish(TOPIC_PUBLISH_TEMP, String(data[TEMPERATURE_INDEX], 2).c_str(), true);
  displayInMonitorSerialSensorData();
}

void readSensorData(){
  data[HUMIDITY_INDEX] = dht.readHumidity();
  data[TEMPERATURE_INDEX] = dht.readTemperature();
}

void displayInMonitorSerialSensorData(){
  Serial.print("Humidade: ");
  Serial.println(data[HUMIDITY_INDEX]);
  Serial.print("Temperatura: ");
  Serial.println(data[TEMPERATURE_INDEX]);
}

void connectMQTT() {
  Serial.print("Reconectando ao MQTT Broker..");

  unsigned long startTime = millis();
  while (!mqttClient.connected() && (millis() - startTime < mqtt_timeout)) {
      Serial.print(".");
      String clientId = "ESP32Client_enriquinho_imd_123";
      clientId += String(random(0xffff), HEX);
      
      if (mqttClient.connect(clientId.c_str(), mqtt_client_username, mqtt_client_password)) {
        Serial.println();
        Serial.print("Conectado ao broker MQTT!");
      }
      delay(100);
  }
  Serial.println();
}

void connectWiFi(){
  //WiFi.mode(WIFI_STA); //"station mode": permite o ESP32 ser um cliente da rede WiFi
  WiFi.begin(wifi_ssid, wifi_password);
  Serial.print("Conectando à rede WiFi .. ");

  unsigned long startTime = millis();
  while(WiFi.status() != WL_CONNECTED && (millis() - startTime < wifi_timeout)){
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Falhou!");
  } else{
    Serial.print("Conectado com o IP: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  connectWiFi();
  
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Conectando ao broker MQTT ..");
    mqttClient.setServer(mqtt_broker, mqtt_port);
  }

  Serial.println("Inicializando o DHT.");
  dht.begin();
  Serial.println("DHT inicializado.");
}

void loop() {
  if (!mqttClient.connected()){
    connectMQTT();
  }

  if (mqttClient.connected()){
    mqttClient.loop();
    digitalWrite(led, HIGH);
    readSensorDataAndPublish();
    delay(5000);
    digitalWrite(led, LOW);
    readSensorDataAndPublish();
    delay(5000);
  }
}
