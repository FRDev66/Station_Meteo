// ##############################################
// Version rc : v2.0.0-rc1
// Version Prod : v2.0.0
// Auteur : FRDev66
// Date : 13/03/2023
//
// Modification : 
// * Augmentation de l'intervalle de temps entre 2 mesures Statiques (Température / Humidité / pression)
//
// ##############################################
/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/

#define BLYNK_TEMPLATE_ID           "TMPLvMXPVYpR"
#define BLYNK_DEVICE_NAME           "Meteo"
#define BLYNK_AUTH_TOKEN            "e4Ra7py3GsW9pAvPpFUZwiwB17pZIYHJ"

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>

#define BLYNK_PRINT Serial
#define BLYNK_HEARTBEAT 180

//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

// Constantes du programme
#define adresseI2CduBME280 0x76              // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)
#define SEALEVELPRESSURE_HPA 1024.90         // https://fr.wikipedia.org/wiki/Pression_atmospherique (1013.25 hPa en moyenne, valeur "par défaut")
#define delaiRafraichissementAffichage 1500  // Délai de rafraîchissement de l'affichage (en millisecondes)
#define tempoMesures 240000 // Délai entre 2 Mesures Statiques (temp / humidité / presssion - en millisecondes - 30 minutes)


Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

byte nombreDePeripheriquesTrouves = 0;    // Variable indiquant combien de périphériques I2C ont répondu "présent"

//unsigned long delayTime;
unsigned long tempoDepart = 0;

bool Connected2Blynk = false;

// Déclaration Variables pour la Fonction ChargeBatterie()
// Fonction ChargeBatterie() : permet de remonter le niveau de charge de la batterie via le port V2
int data;
float voltage;
int chargeBat;

// Déclaration Variables pour la Fonction Anémomètre
int pinAnometre = A0 ; // Connection au PIN A0
int val = 0 ;

void setup() {
  Serial.begin(115200);
  Serial.println(F("                                    ~~ SCANNER I2C ~~                                       "));
  Serial.println(F("Scanne toutes les adresses i2c, afin de repérer tous les périphériques connectés à l'arduino"));
  Serial.println(F("============================================================================================"));
  Serial.println();

  // Initialisation de la liaison i2C
  Wire.begin();

  // Boucle de parcous des 127 adresses i2c possibles
  for (byte adresseI2C = 0; adresseI2C < 127; adresseI2C++)
  {
    Wire.beginTransmission(adresseI2C);             // Interrogation de l'adresse i2c ciblée
    if (Wire.endTransmission () == 0)               // Si cela s'est bien passé, c'est qu'il y a un périphérique connecté à cette adresse
    {
      Serial.print(F("Périphérique i2c trouvé à l'adresse : "));
      Serial.print(adresseI2C, DEC);                // On affiche son adresse au format décimal
      Serial.print(F(" (0x"));
      Serial.print(adresseI2C, HEX);                // … ainsi qu'au format hexadécimal (0x..)
      Serial.println(F(")"));
      
      nombreDePeripheriquesTrouves++;
      //delay(1);                                     // Temporisation, avant de passer au scan de l'adresse suivante
    }
  }

  // Affichage final, indiquant le nombre total de périphériques trouvés sur le port I2C de l'arduino
  if (nombreDePeripheriquesTrouves == 0) {
    Serial.println(F("Aucun périphérique I2C trouvé…"));
  }
  else if (nombreDePeripheriquesTrouves == 1) {
    Serial.println();
    Serial.println(F("1 périphérique trouvé !"));
  }
  else {
    Serial.println();
    Serial.print(nombreDePeripheriquesTrouves);
    Serial.println(F("périphériques trouvés !"));
  }
  Serial.println(F("Scan terminé."));  

  if(!bme.begin(adresseI2CduBME280)) {
    Serial.println(F("--> ÉCHEC…"));
  } else {
    Serial.println(F("--> RÉUSSIE !"));
  }
  Serial.println();

  // déclare le PIN raccordé à la Batterie Pôle POSITIF : A1
  pinMode(A0, INPUT);
  analogWrite(A0, LOW);

  Blynk.begin(auth, ssid, pass);
}


void loop() { 
  val = analogRead(pinAnometre); // Lecture de la valeur de mesure <-- Anémomètre
  Serial.print(val) ;
  
  // Toutes les 30 minutes ==> Lancer une phase de Mesures Statiques
  if ( millis() - tempoDepart >= tempoMesures ) 
  {
    CheckConnexionBlynk();
    tempoDepart = millis();
    mesure_temp_humidite();
    //ChargeBatterie();
  }
}

void CheckConnexionBlynk() {
  Connected2Blynk = Blynk.connected();
  if(!Connected2Blynk){
    Serial.println("Connexion au Serveur Blynk KO !!");
    ConnexionWiFi();  
  }
  else{
    Serial.println("Toujours Connecté au Serveur Blynk !!");    
  }
}

void mesure_temp_humidite() {
  float temperature = bme.readTemperature();
  float pression = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidity = bme.readHumidity();
  
  Serial.print("Temperature = ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pression Atmospherique = ");
  Serial.print(pression);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidite = ");
  Serial.print(humidity);
  Serial.println(" %");

  Blynk.virtualWrite(V6,temperature);
  Blynk.virtualWrite(V4,humidity);
  Blynk.virtualWrite(V5,pression);
  Blynk.virtualWrite(V7,altitude);
}

void ConnexionWiFi() {
  if(WiFi.status() == WL_CONNECTED){  
    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP()); 
    statusConnexion();
  }
  else{
    Serial.println("\nCheck Router ");
    WiFi.begin(ssid, pass); 
    Blynk.begin(auth, ssid, pass);  
  }
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

void ChargeBatterie() {
//La fonction analogRead de Arduino renvoie une valeur comprise entre 0 et 1023 avec une correspondance linéaire par rapport à la tension de la pin analogique qui doit être entre 0 et 5V
//ValeurADC = Tension * 1023 / 5 et inversement : Tension = ValeurADC * 5 / 1023 ==> 5/1023 = 0.0048
   data = analogRead(A0);
   voltage = data * 0.0048;
   //chargeBat = (voltage * 100) / tension;
   if (voltage > 3.7) {
    chargeBat = 100;   
   } else if (voltage > 3.63) {
    chargeBat = 90;       
   } else if (voltage > 3.56) {
    chargeBat = 80;
   } else if (voltage > 3.49) {
    chargeBat = 70;
   } else if (voltage > 3.42) {
    chargeBat = 60;
   } else if (voltage > 3.35) {
    chargeBat = 50;
   } else if (voltage > 3.28) {
    chargeBat = 40;
   } else if (voltage > 3.21) {
    chargeBat = 30;
   } else if (voltage > 3.14) {
    chargeBat = 20;
   } else if (voltage > 3.07) {
    chargeBat = 10;
   } else if (voltage < 3.0) {
    chargeBat = 0;       
   }
          
   Serial.print(voltage);
   Serial.println(" Volts");
   Serial.print(chargeBat);
   Serial.println(" %");

   Blynk.virtualWrite(V2,chargeBat);

   //delay(1000);  
}