/* Senseur à Hall Effect 
 
 Allume ou éteind une LED connectée sur la pin digitale 8 en fonction  
 de l'activation d'un senseur à Effet Hall US5881LUA sensible au champ 
 magnétique.
 
 Le senseur Effet Hall US5881LUA est disponible chez MC Hobby
    http://shop.mchobby.be/product.php?id_product=86 
 Les aimants surpuissant de Rare Earth sont aussi disponibles chez MC Hobby
    http://shop.mchobby.be/product.php?id_product=87 
 .
 
 Le circuit:
 * LED connectée à la masse (GND) et à la pin 8 par l'intermédiaire d'une résistance de 330 Ohms.
 * Le senseur Effet Hall US5881LUA est connecté comme suit:
     Pin 1: +5v
     Pin 2: Masse/GND
     Pin 3: +5V via une résistance pull-up de 10 KOhms
            MAIS AUSSI
            sur la PIN 2 d'Arduino (pour lecture du senseur)
 * Exemple de circuit, plan sur 
     http://mchobby.be/wiki/index.php?title=Senseur_à_Effet_Hall

 created 2012
 by Meurisse D. - http://www.MCHobby.be (vente de matériel et Kit)
 Licence BY-CC-SA 
 Mentionner explicitement "MCHobby vente de matériel et Kit" .

 */

#include <SimpleDHT.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11


const int ledPin = 8; 
const int hallPin = 2;
static const int DHT_SENSOR_PIN = 4;
//SimpleDHT11 dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );
SimpleDHT11 dht11(DHT_SENSOR_PIN);

int sensorValue; 

void setup(){
  Serial.begin(9600);
  pinMode( ledPin, OUTPUT ); 
  pinMode( hallPin, INPUT );
}




void loop() {
  // lecture du capteur a Effet Hall
  sensorValue = digitalRead( hallPin );
  Serial.print(sensorValue);
  
  // senseurValue = HIGH sans aimant
  // senseurValue = LOW quand POLE SUD aimant
  sensorValue = not( sensorValue );
  
  // Allumer eteindre la LED
  digitalWrite( ledPin, sensorValue );

  // start working...
  Serial.println("=================================");
  Serial.println("Sample DHT11...");
  
  // read without samples.
  byte temperature = 0;
  byte humidity = 0;
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err="); Serial.print(SimpleDHTErrCode(err));
    Serial.print(","); Serial.println(SimpleDHTErrDuration(err)); delay(1000);
    return;
  }
  
  Serial.print("Sample OK: ");
  Serial.print((int)temperature); Serial.print(" *C, "); 
  Serial.print((int)humidity); Serial.println(" H");
  
  // DHT11 sampling rate is 1HZ.
  delay(1500);

  
}
