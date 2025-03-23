// ##############################################
// Version : v3.0.0-rc1
// Auteur : FRDev66
// Date : 23/03/2025
//
// Modification : 
// * SM-25-Intégration-Code-à-distance
// * SM-24-Mesure-du-Vent-confirmer-erroner
// * SM-12-Contrôle-Module-Redémarrer-le-Module-à-distance
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
#include <PubSubClient.h> //Librairie pour la gestion Mqtt 
#include <ArduinoOTA.h>

#define BLYNK_PRINT Serial
#define BLYNK_HEARTBEAT 45



//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";

// Constantes du programme
#define adresseI2CduBME280 0x76              // Adresse I2C du BME280 (0x76, dans mon cas, ce qui est souvent la valeur par défaut)
#define SEALEVELPRESSURE_HPA 1024.90         // https://fr.wikipedia.org/wiki/Pression_atmospherique (1013.25 hPa en moyenne, valeur "par défaut")
#define delaiRafraichissementAffichage 1500  // Délai de rafraîchissement de l'affichage (en millisecondes)
#define tempoMesures 5000 // Délai entre 2 Mesures Statiques (temp / humidité / presssion - en millisecondes - 30 minutes) - par défaut = 240000


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
float millivolt = 0;
float vitesseKM = 0;

// MQTT Broker  
#define MQTT_BROKER       "192.168.1.61"
#define MQTT_BROKER_PORT  1883
#define MQTT_USERNAME     "frdev66"
#define MQTT_KEY          "Lenems66!!"

WiFiClient espClient2;            // Use this for WiFi instead of EthernetClient
PubSubClient client(espClient2);

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
  //pinMode(A0, INPUT);
  //analogWrite(A0, LOW);

  Blynk.begin(auth, ssid, pass);

  setup_mqtt();
  client.publish("esp2/init", "Hello from ESP8266_EXT1");

  initOTA();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.publish("esp2/adresseIP",WiFi.localIP().toString().c_str());
}


void loop() { 
  ArduinoOTA.handle();

  client.loop();

  // Toutes les 30 minutes ==> Lancer une phase de Mesures Statiques
  if ( millis() - tempoDepart >= tempoMesures ) 
  {
    Blynk.connect();
    CheckConnexionBlynk();
    
    //tempoDepart = millis();
    mesure_temp_humidite();
    //ChargeBatterie();

    mesure_vent();
    
    //mqtt_publish("esp2/vitessevent",vitesseKM);
    //mqtt_publish("esp2/temperatureExt",temperatureext);
    tempoDepart = millis();
    mqtt_publish("esp2/millis",tempoDepart); // SM-12 : Remontée valeur tempoDepart basée sur millis()

    if (tempoDepart >=50000)
    {
      client.publish("esp2/admin","max valeur");
      delay(1000); //To allow serial and mqtt messages are delivered
      ESP.restart();
    }

  }




}

void CheckConnexionBlynk() {
  Connected2Blynk = Blynk.connected();
  
  Serial.print("Check Connexion Blynk Cloud : ");
  Serial.println(Connected2Blynk);

  if(!Connected2Blynk){
    Serial.println("Connexion au Serveur Blynk KO !!");
    ConnexionWiFi();  
  }
  else{
    Serial.println("Toujours Connecté au Serveur Blynk !!");    
  }
}


void mesure_temp_humidite() {
  float temperatureext = bme.readTemperature();
  float pression = bme.readPressure() / 100.0F;
  float altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  float humidityext = bme.readHumidity();
  
  Serial.print("Temperature = ");
  Serial.print(temperatureext);
  Serial.println(" °C");

  Serial.print("Pression Atmospherique = ");
  Serial.print(pression);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(altitude);
  Serial.println(" m");

  Serial.print("Humidite = ");
  Serial.print(humidityext);
  Serial.println(" %");

  Blynk.virtualWrite(V6,temperatureext);
  Blynk.virtualWrite(V4,humidityext);
  Blynk.virtualWrite(V5,pression);
  Blynk.virtualWrite(V7,altitude);

  mqtt_publish("esp2/temperatureExt",temperatureext);
  //client.publish("esp2/temperatureExt",temperatureext);
  mqtt_publish("esp2/humiditeExt",humidityext);
  mqtt_publish("esp2/pressionExt",pression);
  mqtt_publish("esp2/altitudeExt",altitude);
}

void mesure_vent() {
  val = analogRead(pinAnometre); // Lecture de la valeur de mesure <-- Anémomètre
  float millivolt = ((val-7)/1024.00)*3300.00;
  float vitesseKM = (millivolt/1000)*50;
  Serial.print("Mesure Anémomètre = ");
  Serial.println(val) ;
  Serial.print("millivolt = ");
  Serial.println(millivolt);
  Serial.print("Vitesse vent Km/h = ");
  Serial.println(vitesseKM);

  mqtt_publish("esp2/vitessevent",vitesseKM);
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

void setup_mqtt() {
  client.setServer(MQTT_BROKER, MQTT_BROKER_PORT);
  reconnect();
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connection au serveur MQTT ...");
    if (client.connect("ESPClientExterieur", MQTT_USERNAME, MQTT_KEY)) {
      Serial.println("MQTT connecté");
    }
    else {
      Serial.print("echec, code erreur= ");
      Serial.println(client.state());
      Serial.println("nouvel essai dans 2s");
    delay(2000);
    }
  }
  //client.subscribe("esp2/temperatureExt"); //souscription au topic du esp2
}

//Fonction pour publier un float sur un topic
void mqtt_publish(String topic, float t) {
  char top[topic.length()+1];
  topic.toCharArray(top,topic.length()+1);
  char t_char[50];
  String t_str = String(t);
  t_str.toCharArray(t_char, t_str.length() + 1);
  client.publish(top,t_char);
  
  Serial.print("topic = ");
  Serial.println(top);
  Serial.print("valeur topic MQTT = ");
  Serial.println(t);
  Serial.println(t_char);
}

void initOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}