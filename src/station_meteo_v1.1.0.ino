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

//#include <SimpleDHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Wire.h>  

#define DHT_SENSOR_TYPE DHT_TYPE_11
#define BLYNK_PRINT Serial
#define SEALEVELPRESSURE_HPA (990.28) // Définition de la pression vs altitude à 190m (990.28 hPa)

#define adresseI2CduBME280 0x76


//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

const int ledPin = 4; 
const int hallPin = 2;
//static const int DHT_SENSOR_PIN = 5;
//SimpleDHT11 dht11(DHT_SENSOR_PIN);
Adafruit_BME280 bme;

int sensorValue;

unsigned long lastmillis = 0;

byte nombreDePeripheriquesTrouves = 0;

int tempoActive = 0;
// Temps à l'activation de la tempo
unsigned long tempoDepart = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("                                    ~~ SCANNER I2C ~~                                       ");
  Serial.println("Scanne toutes les adresses i2c, afin de repérer tous les périphériques connectés à l'arduino");
  Serial.println("============================================================================================");
  Serial.println();

  // Initialisation de la liaison i2C
  Wire.begin();

  // Boucle de parcous des 127 adresses i2c possibles
  for (byte adresseI2C = 0; adresseI2C < 127; adresseI2C++)
  {
    Wire.beginTransmission(adresseI2C);             // Interrogation de l'adresse i2c ciblée
    if (Wire.endTransmission () == 0)               // Si cela s'est bien passé, c'est qu'il y a un périphérique connecté à cette adresse
    {
      Serial.print("Périphérique i2c trouvé à l'adresse : ");
      Serial.print(adresseI2C, DEC);                // On affiche son adresse au format décimal
      Serial.print(" (0x");
      Serial.print(adresseI2C, HEX);                // … ainsi qu'au format hexadécimal (0x..)
      Serial.println(")");
      
      nombreDePeripheriquesTrouves++;
      delay(1);                                     // Temporisation, avant de passer au scan de l'adresse suivante
    }
  }

  // Affichage final, indiquant le nombre total de périphériques trouvés sur le port I2C de l'arduino
  if (nombreDePeripheriquesTrouves == 0) {
    Serial.println("Aucun périphérique I2C trouvé…");
  }
  else if (nombreDePeripheriquesTrouves == 1) {
    Serial.println();
    Serial.println("1 périphérique trouvé !");
  }
  else {
    Serial.println();
    Serial.print(nombreDePeripheriquesTrouves);
    Serial.println("périphériques trouvés !");
  }
  Serial.println("Scan terminé.");  

  //attachInterrupt(hallPin, rpm_vent, FALLING);
  
  pinMode( ledPin, OUTPUT );
  pinMode( hallPin, INPUT );

  //if (!bme.begin(0x77)) {
	//	Serial.println("Aucun capteur BME280 trouvé, vérifier le câblage !");
	//	while (1);
	//}

  // Initialisation du BME280
  Serial.print("Initialisation du BME280, à l'adresse [0x");
  Serial.print(adresseI2CduBME280, HEX);
  Serial.println("]");
  
  if(!bme.begin(adresseI2CduBME280)) {
    Serial.println("--> ÉCHEC…");
  } else {
    Serial.println("--> RÉUSSIE !");
  }
  Serial.println();

  Blynk.begin(auth, ssid, pass);
}


/* void mesure_temp_humidite() {
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
    
} */

void mesure_bme280() {
 // start working...
  Serial.println("=================================");
  Serial.println("Sample BME280...");
  
  // read without samples.
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pression = bme.readPressure();
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

 
  Serial.print("Sample OK: ");
  Serial.print(temperature);
  Serial.print(" *C, "); 
  Serial.print(humidity);
  Serial.println(" H");
  Serial.print(pression);
  Serial.println(" hPa");
  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println("m");

  Blynk.virtualWrite(V6,temperature);
  Blynk.virtualWrite(V4,humidity);
  Blynk.virtualWrite(V5,pression);
  Blynk.virtualWrite(V7,altitude);

    
}

void loop() {
  Blynk.run();
  // put your main code here, to run repeatedly:
  
  // Si la temporisation est active,
  //if ( tempoActive ) { 
  // Et si il s'est écoulé 3 secondes,
  if ( millis() - tempoDepart >= 3000 ) {
    tempoDepart = millis();
    //mesure_temp_humidite();
    mesure_bme280();
  // Et on désactive la temporisation pour ne pas afficher ce message une seconde fois
    //tempoActive = 0; 
  }


}

