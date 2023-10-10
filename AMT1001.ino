#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>

#define WIFI_SSID       "WIFI_SSID"
#define WIFI_PASS       "WIFI_PASSWD"

#define MQTT_SERVER     "io.adafruit.com"
#define MQTT_PORT       1883
#define MQTT_USERNAME   "mqttusername"
#define MQTT_KEY        "mqttpasword"
const int JSON_BUFFER_SIZE = 256;
// Definir dados do LCD
LiquidCrystal_I2C lcd(0x27,16,2); 
const int pinoAnalogico = 0; // Substitua pelo número do pino analógico que você deseja ler
// Saida de Umidade
float voltOutHum = 0;

// Definir objetos de cliente WiFi y MQTT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_KEY);

// Definir feeds MQTT (puedes agregar más si lo deseas)
Adafruit_MQTT_Publish feed = Adafruit_MQTT_Publish(&mqtt, MQTT_USERNAME "/feeds/DATACENTER");
Adafruit_MQTT_Subscribe miSuscripcion = Adafruit_MQTT_Subscribe(&mqtt, MQTT_USERNAME "/feeds/mi-otro-feed");

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Iniciando");
  lcd.init();
  Serial.begin(115200);
  delay(10);

  // Conéctate a la red WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
    lcd.print("Conectando a WiFi...");
  }
  Serial.println("Conectado a WiFi");
  lcd.print("Conectado a WiFi");
  lcd.init();
  

  // Inicia la conexión MQTT
  mqtt.connect();
}

void loop() {
    int valorAnalogico = analogRead(pinoAnalogico); // Lê o valor analógico do pino
  voltOutHum = valorAnalogico / 30;
  Serial.print("Valor %: ");
  Serial.println(voltOutHum);
  // Crie um objeto JSON
  StaticJsonDocument<JSON_BUFFER_SIZE> jsonDoc;

  // Adicione variáveis ao objeto JSON
  jsonDoc["sensor"] = "AMT1001";
  jsonDoc["temperatura"] = 25.5;
  jsonDoc["umidade"] = voltOutHum;

  // Serialize o objeto JSON em uma string
  String jsonString;
  serializeJson(jsonDoc, jsonString);



  // Mantén la conexión MQTT activa
  if (!mqtt.connected()) {
    Serial.println("Conexão MQTT perdida. Reconectando...");
    lcd.print("Conexão MQTT perdida. Reconectando...");
    if (mqtt.connect()) {
      Serial.println("Reconexão exitosa");
      lcd.print("Conexão MQTT-OK");
      lcd.clear();
      
    } else {
      Serial.println("Erro ao conectar aguarde 10s...");
      lcd.print("Erro ao conectar aguarde 10s...");
      delay(10000);
    }
  }

  // Publica un mensaje en el feed
  static uint32_t lastMillis = 0;
  lcd.init();
  if (millis() - lastMillis > 10000) {
    if (feed.publish(voltOutHum)) {
      Serial.println("Mensaje publicado correctamente");
      lcd.print("Publicando...");
    } else {
      Serial.println("Error al publicar el mensaje");
      lcd.print("Erro ao publicar");
    }
    lastMillis = millis();
  }

  // Lee mensajes recibidos en la suscripción
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(1000))) {
    if (subscription == &miSuscripcion) {
      Serial.print(F("Mensaje recibido: "));
      Serial.println((char *)miSuscripcion.lastread);
    }
  }
  lcd.init();
}
