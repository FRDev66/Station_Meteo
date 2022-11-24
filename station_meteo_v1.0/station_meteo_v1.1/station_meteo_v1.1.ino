
#define BLYNK_TEMPLATE_ID           "TMPLvMXPVYpR"
#define BLYNK_DEVICE_NAME           "Meteo"
#define BLYNK_AUTH_TOKEN            "e4Ra7py3GsW9pAvPpFUZwiwB17pZIYHJ"

#include <SimpleDHT.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

//#include <ESP8266WiFi.h>
#include <ESP8266_Lib.h>
//#include <BlynkSimpleEsp8266.h>
#include <BlynkSimpleShieldEsp8266.h>
//#include <SPI.h>
//#include <WiFi.h>
//#include <BlynkSimpleWifi.h>

//#define EspSerial Serial1
//#define BLYNK_PRINT Serial1

//#define ESP8266_BAUD 115200
//ESP8266 wifi(&EspSerial);
//ESP8266 wifi(&Serial);

      //#define BLYNK_PRINT DebugSerial
      //#include <BlynkSimpleStream.h>

//#define BLYNK_PRINT Serial // Enables Serial Monitor
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Livebox-1F90";
char pass[] = "o3jwTuDzadcmQAtZ2r";


const int ledPin = 8; 
const int hallPin = 2;
static const int DHT_SENSOR_PIN = 4;
SimpleDHT11 dht11(DHT_SENSOR_PIN);

BlynkTimer timer;

int sensorValue;

int tempoActive = 0;
// Temps à l'activation de la tempo
unsigned long tempoDepart = 0;

void setup(){
  Serial.begin(9600);
  //Serial1.begin(115200);
  pinMode( ledPin, OUTPUT ); 
  pinMode( hallPin, INPUT );

  //EspSerial.begin(ESP8266_BAUD);  
  Serial1.begin(115200);  
  //delay(10);

  //Blynk.begin(auth, Serial1);
        //   Blynk.config(auth);   
    
  //timer.setInterval(1000L, mesure_temp_humidite);
}

void mesure_anemometre() {
    // lecture du capteur a Effet Hall
  sensorValue = digitalRead( hallPin );
  //Serial.print("sensorValue = ");
  //Serial.println(sensorValue);
  
  // senseurValue = HIGH sans aimant
  // senseurValue = LOW quand POLE SUD aimant
  sensorValue = not( sensorValue );
  
  // Allumer eteindre la LED
  digitalWrite( ledPin, sensorValue );

  //Blynk.virtualWrite(V4, sensorValue);
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

  //Blynk.virtualWrite(V5, h);
  //Blynk.virtualWrite(V6, t);
    
}


void loop() {
  Blynk.run();
  
  mesure_anemometre();
  
  // Activation de la temporisation
  //tempoActive = 1;
  //tempoDepart = millis();
  // Serial.print("tempoActive = ");
  // Serial.println(tempoActive);
  // Serial.print("tempoDepart = ");
  // Serial.println(tempoDepart);
  // Serial.print("millis = ");
  // Serial.println(millis());
  // Serial.print("delta = ");
  // Serial.println(millis() - tempoDepart);  
  
  
  // Si la temporisation est active,
  //if ( tempoActive ) { 
  // Et si il s'est écoulé 3 secondes,
    if ( millis() - tempoDepart >= 3000 ) {
      tempoDepart = millis();
      mesure_temp_humidite();
  // Et on désactive la temporisation pour ne pas afficher ce message une seconde fois
      //tempoActive = 0; 
    }
  //} 

  //timer.run();

  //Serial.println("HELLO !!");

   // read from port 1, send to port 0:  
  if (Serial1.available()) {  
   int inByte = Serial1.read();  
   //Serial.print("Test Serial1");
   //Serial.println(inByte);
   Serial.write(inByte);  
  }  
  // read from port 0, send to port 1:  
  if (Serial.available()) {  
   int inByte = Serial.read();  
   Serial1.write(inByte);  
  }  
  
}
