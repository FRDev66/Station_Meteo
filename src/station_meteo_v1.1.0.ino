// ##############################################
// Version : v1.1.0_rc2
// Auteur : FRDev66
// Date : 14/12/2022
//
// Modification : 
// * fonctionnalité permettant de transmettre à l'Application que le dispositif est connecté
//
// ##############################################

#define BLYNK_TEMPLATE_ID           "TMPLvMXPVYpR"
#define BLYNK_DEVICE_NAME           "Meteo"
#define BLYNK_AUTH_TOKEN            "e4Ra7py3GsW9pAvPpFUZwiwB17pZIYHJ"

#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define BLYNK_PRINT Serial

#define PI 3.1415926535897932384626433832795
#define vitessems_topic "anemometre/vitesseVentMS"        // Topic Vitesse du vent (en m/s)
#define vitessekmh_topic "anemometre/vitesseVentKMH"      // Topic Vitesse du vent (en km/h)


//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

const int ledPin = 4; 
const int hallPin = 2;
static const int DHT_SENSOR_PIN = 5;
SimpleDHT11 dht11(DHT_SENSOR_PIN);

int sensorValue;

volatile int rpmcount = 0;
volatile float vitVentMS = 0;
volatile float vitVentKMH = 0;
int rpm = 0;
unsigned long lastmillis = 0;

int dureeMesVitVent = 5000; // en ms, durée de mesure de la vitesse du vent. 
//Choisir un multiple de 1000 pour le calcul de la vitesse du vent

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

void mesure_anemometre() {
    // lecture du capteur a Effet Hall
  sensorValue = digitalRead( hallPin );
  Serial.print("sensorValue = ");
  Serial.println(sensorValue);
  sensorValue = not( sensorValue );

  digitalWrite( ledPin, sensorValue );

  rpmcount = pulseIn(hallPin, HIGH); //Enregistrement de temps entre deux passage(période).
  Serial.print("Periode:");              //Affichage du temps de passage sur la voie série.
  Serial.println(rpmcount);

  Serial.print("Vitesse:");              //Affichage de la vitesse.
  Serial.print(2 * PI * 0.08 * pow(10, 6) / rpmcount); // V=2.3,14.F(N).R.N  ( F(N)= fonction d'étalonage R= rayon N= nombre de tours par seconde).
  Serial.println(" M/S ");

  //rpm_vent();
}

void mesure_temp_humidite() {
 // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  //float t = (float)temperature;
  //float h = (float)humidity;  
  
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

void getVitesseVent() {
    //detachInterrupt(hallPin); 
    mesure_anemometre();
    //rpm = rpmcount * ( 60 / ( dureeMesVitVent / 1000 ) ); 
    //Serial.println(rpm);
    
    if ( rpmcount > 0 ) {
      //vitVentKMH = ( rpmcount + 6.174 ) / 8.367;
      //vitVentMS = ( ( ( rpm + 6.174 ) / 8.367 ) * 1000 ) / 3600; 
      vitVentMS = 2*PI*0.08*rpmcount;      
    } else {
      vitVentKMH = 0;
      vitVentMS = 0;
    }
    Serial.println("Vitesse (m/s) : ");
    Serial.println(vitVentMS);

    rpmcount = 0;           // Redémarre le compte tour
    lastmillis = millis();  // et réinitialise le chrono
    //attachInterrupt(hallPin, rpm_vent, FALLING); // Rélance l'interruption du compte tour
}

void rpm_vent() { 
  Serial.println("Tour = ");
  rpmcount++;
  Serial.println(rpmcount);
}

void loop() {
  Blynk.run();
  // put your main code here, to run repeatedly:
  
  //mesure_anemometre();

  //getVitesseVent();
  
  // Si la temporisation est active,
  //if ( tempoActive ) { 
  // Et si il s'est écoulé 3 secondes,
  if ( millis() - tempoDepart >= 3000 ) {
    tempoDepart = millis();
    mesure_temp_humidite();
    statusConnexion();
  // Et on désactive la temporisation pour ne pas afficher ce message une seconde fois
    //tempoActive = 0; 
  }

  //if (millis() - tempoDepart >= 1000){
  //  tempoDepart = millis();
  //  getVitesseVent();    
  //}

}

void statusConnexion() {
  // Récupération de la force du signal WiFi en dBm
  int rssi = WiFi.RSSI();
  // Affiche le RSSI
  Serial.print("Force du Signal");
  Serial.println(rssi);
  // Envoi de la Force du signal vers la Broche Virtuelle V1
  Blynk.virtualWrite(V1,rssi);
}

