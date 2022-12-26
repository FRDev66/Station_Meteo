//###############################################
//## 
//## Version : v1.1.0-rc3
//## Date : 26/12/2022
//## Auteur : FRDev66
//## 
//## Modifications : 
//## Initialisation de la Release RC3 :
//## - Remplacement du Capteur DHT11 par un Capteur EMP280
//## Objectif(s) : 
//## - Permettre d'utiliser le Module en Extérieur 
//## -- pour prise en compte des Mesures Température <0
//## - Intégration des Données de Pression Atmosphérique
//##
//###############################################

#define BLYNK_TEMPLATE_ID           "TMPLvMXPVYpR"
#define BLYNK_DEVICE_NAME           "Meteo"
#define BLYNK_AUTH_TOKEN            "e4Ra7py3GsW9pAvPpFUZwiwB17pZIYHJ"

#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define BLYNK_PRINT Serial


//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

const int ledPin = 4; 
const int hallPin = 2;
static const int DHT_SENSOR_PIN = 5;
SimpleDHT11 dht11(DHT_SENSOR_PIN);

int sensorValue;

unsigned long lastmillis = 0;

int tempoActive = 0;
// Temps à l'activation de la tempo
unsigned long tempoDepart = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //attachInterrupt(hallPin, rpm_vent, FALLING);
  
  pinMode( ledPin, OUTPUT );
  pinMode( hallPin, INPUT );

  Blynk.begin(auth, ssid, pass);
}


void mesure_temp_humidite() {
 // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  float t = (float)temperature;
  float h = (float)humidity;  
  
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.print(SimpleDHTErrCode(err));
    Serial.print(",");
    Serial.println(SimpleDHTErrDuration(err));
    return;
  }
 
  Serial.print("Sample OK: ");
  Serial.print((int)temperature);
  Serial.print(" *C, "); 
  Serial.print((int)humidity);
  Serial.println(" H");

  Blynk.virtualWrite(V6,temperature);
  Blynk.virtualWrite(V4,humidity);
    
}

void loop() {
  Blynk.run();
  // put your main code here, to run repeatedly:
  
  // Si la temporisation est active,
  //if ( tempoActive ) { 
  // Et si il s'est écoulé 3 secondes,
  if ( millis() - tempoDepart >= 3000 ) {
    tempoDepart = millis();
    mesure_temp_humidite();
  // Et on désactive la temporisation pour ne pas afficher ce message une seconde fois
    //tempoActive = 0; 
  }


}

