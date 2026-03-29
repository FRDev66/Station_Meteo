// ##############################################
// Version : v3.1.0-LowPower
// Auteur  : FRDev66 + Copilot
// Date    : 2025
// ##############################################

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ---------------------------
// PARAMÈTRES UTILISATEUR
// ---------------------------
#define I2C_BME_ADDR 0x76
#define SEALEVELPRESSURE_HPA 1024.90

const char* ssid = "Livebox-1F90";
const char* pass = "o3jwTuDzadcmQAtZ2r";

#define MQTT_BROKER       "192.168.1.49"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "mqtt"
#define MQTT_KEY          "Electronlibre66!!"

// Intervalle entre mesures (en secondes)
#define SLEEP_DURATION_SEC 40 

// ---------------------------
// OBJETS
// ---------------------------
Adafruit_BME280 bme;
WiFiClient espClient;
PubSubClient client(espClient);

// ---------------------------
// FONCTIONS
// ---------------------------

void wifiOn() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
    delay(100);
  }
}

void wifiOff() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(50);
}

void mqtt_publish(String topic, float value) {
  char top[topic.length() + 1];
  topic.toCharArray(top, topic.length() + 1);

  char payload[20];
  dtostrf(value, 1, 2, payload);

  client.publish(top, payload);
}

void setup_mqtt() {
  client.setServer(MQTT_BROKER, MQTT_BROKER_PORT);

  while (!client.connected()) {
    client.connect("ESPClient_Meteo", MQTT_USERNAME, MQTT_KEY);
  }
}

void mesure_vent() {
  int val = analogRead(A0);
  float millivolt = ((val - 7) / 1024.0) * 3300.0;
  float vitesseKM = (millivolt / 1000.0) * 50.0;

  mqtt_publish("esp2/vitessevent", vitesseKM);
}

void setup() {
  Serial.begin(115200);
  delay(200);

  // ---------------------------
  // INITIALISATION BME280
  // ---------------------------
  if (!bme.begin(I2C_BME_ADDR)) {
    Serial.println("Erreur BME280 !");
  }

  // Mode ultra basse consommation
  bme.setSampling(
    Adafruit_BME280::MODE_FORCED,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::SAMPLING_X1,
    Adafruit_BME280::FILTER_OFF
  );

  // ---------------------------
  // MESURES
  // ---------------------------
  bme.takeForcedMeasurement();

  float temperature = bme.readTemperature();
  float humidity    = bme.readHumidity();
  float pressure    = bme.readPressure() / 100.0F;
  float altitude    = bme.readAltitude(SEALEVELPRESSURE_HPA);

  // ---------------------------
  // WIFI + MQTT
  // ---------------------------
  wifiOn();
  setup_mqtt();

  mqtt_publish("esp2/temperatureExt", temperature);
  mqtt_publish("esp2/humiditeExt", humidity);
  mqtt_publish("esp2/pressionExt", pressure);
  mqtt_publish("esp2/altitudeExt", altitude);

  mesure_vent();

  wifiOff();

  // ---------------------------
  // DEEP SLEEP
  // ---------------------------
  ESP.deepSleep((uint64_t)SLEEP_DURATION_SEC * 1e6);
}

void loop() {
  // Ne sera jamais exécuté (DeepSleep)
}