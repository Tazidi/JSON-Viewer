#include "DHTesp.h"
#include "PubSubClient.h"
#include "WiFi.h"

// Pin hardware
const int DHT_PIN = 19;
const int LED_GREEN_PIN = 25;
const int LED_YELLOW_PIN = 26;
const int LED_RED_PIN = 27;
const int RELAY_PUMP_PIN = 16;
const int BUZZER_PIN = 13;

// Wi-Fi dan MQTT
const char* ssid = "Wokwi-GUEST";
const char* pass = "";
const char* broker = "broker.hivemq.com";
const int brokerPort = 1883;

// Variabel jaringan
String macAddr;
String ipAddr;

// Objek sensor dan komunikasi
DHTesp dhtSensor;
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// Variabel untuk mengontrol LED dan buzzer
bool isRedActive = false;
bool isYellowActive = false;
bool isGreenActive = false;
bool isBuzzerActive = false;

void WifiSetup() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  ipAddr = WiFi.localIP().toString().c_str();
  macAddr = WiFi.macAddress();

  Serial.print("Connected at: ");
  Serial.println(ipAddr);
  Serial.print("MAC: ");
  Serial.println(macAddr);
}

void MqttSetup() {
  mqtt.setServer(broker, brokerPort);
}

void Reconnect() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqtt.connected()) {
    Serial.println("Reconnecting to MQTT Broker...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str())) {
      Serial.print("ID: ");
      Serial.println(clientId);
      Serial.println("Connected.");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_GREEN_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WifiSetup();
  MqttSetup();

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
}

void controlLedsAndBuzzer(float temperature) {
  if (temperature > 35) {
    digitalWrite(LED_RED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    isRedActive = true;
    isBuzzerActive = true;

    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    isYellowActive = false;
    isGreenActive = false;
  } else if (temperature >= 30 && temperature <= 35) {
    digitalWrite(LED_YELLOW_PIN, HIGH);
    isYellowActive = true;

    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_GREEN_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    isRedActive = false;
    isGreenActive = false;
    isBuzzerActive = false;
  } else {
    digitalWrite(LED_GREEN_PIN, HIGH);
    isGreenActive = true;

    digitalWrite(LED_RED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    isRedActive = false;
    isYellowActive = false;
    isBuzzerActive = false;
  }

  if (temperature >= 35) {
    digitalWrite(RELAY_PUMP_PIN, HIGH);  // Mengaktifkan relay
  } else {
    digitalWrite(RELAY_PUMP_PIN, LOW);   // Mematikan relay
  }
}

void publishStatus() {
  String statusMessage = String("Red LED: ") + (isRedActive ? "ON" : "OFF") +
                         ", Yellow LED: " + (isYellowActive ? "ON" : "OFF") +
                         ", Green LED: " + (isGreenActive ? "ON" : "OFF") +
                         ", Buzzer: " + (isBuzzerActive ? "ON" : "OFF") +
                         ", Relay: " + (digitalRead(RELAY_PUMP_PIN) ? "ON" : "OFF");
  mqtt.publish("monitoring/status", statusMessage.c_str());
}

void loop() {
  if (!mqtt.connected()) {
    Reconnect();
  }

  mqtt.loop();

  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  float temperature = data.temperature;
  float humidity = data.humidity;

  // Kontrol LED dan buzzer berdasarkan suhu
  controlLedsAndBuzzer(temperature);

  // Mengirimkan suhu dan kelembapan ke topik MQTT masing-masing
  char tempString[10];
  char humString[10];
  dtostrf(temperature, 4, 2, tempString);
  dtostrf(humidity, 4, 2, humString);
  mqtt.publish("monitoring/temperature", tempString);
  mqtt.publish("monitoring/humidity", humString);

  // Mengirimkan status LED, buzzer, dan relay ke topik status
  publishStatus();

  delay(1000); // Delay sebelum pengukuran berikutnya
}
