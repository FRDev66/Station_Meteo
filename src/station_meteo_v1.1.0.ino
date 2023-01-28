// ##############################################
// Version rc : v1.1.0_BugFix#5
// Version Prod : v1.1
// Auteur : FRDev66
// Date : 28/01/2023
//
// Modification : 
// * Intégration d'une solution de reboot de la connexion WiFi
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

//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

// Constantes du programme
#define adresseI2CduBME280 0x76              // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)
#define SEALEVELPRESSURE_HPA 1024.90         // https://fr.wikipedia.org/wiki/Pression_atmospherique (1013.25 hPa en moyenne, valeur "par défaut")
#define delaiRafraichissementAffichage 1500  // Délai de rafraîchissement de l'affichage (en millisecondes)

Adafruit_BME280 bme; // I2C
//Adafruit_BME280 bme(BME_CS); // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

byte nombreDePeripheriquesTrouves = 0;    // Variable indiquant combien de périphériques I2C ont répondu "présent"

//unsigned long delayTime;
unsigned long tempoDepart = 0;

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

  //mesure_temp_humidite();

  Blynk.begin(auth, ssid, pass);
}


void loop() { 
    
  //Mise en place de la fonction de Reboot Connexion WiFi
  if(!Blynk.connected()){
    Serial.println("Module NON CONNECTE !");
    Serial.print("Connexion en cours...");
    //Blynk.connectWiFi(ssid, pass);
  }
  else {
    if ( millis() - tempoDepart >= 60000 ) { // 1 mesure toutes les 15 minutes
      Serial.println("Connexion toujours en cours...");
      tempoDepart = millis();
      mesure_temp_humidite();
      statusConnexion();
    // Et on désactive la temporisation pour ne pas afficher ce message une seconde fois
    //tempoActive = 0; 
    }
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


void statusConnexion() {
  // Récupération de la force du signal WiFi en dBm
  int rssi = WiFi.RSSI();
  // Affiche le RSSI
  Serial.print("Force du Signal");
  Serial.println(rssi);
  // Envoi de la Force du signal vers la Broche Virtuelle V1
  Blynk.virtualWrite(V1,rssi);
}