#include "Wire.h"

int data;
float voltage;
int chargeBat;

void setup() {
   pinMode(A1, INPUT);
   analogWrite(A1, LOW);

   Serial.begin(9600);
 }

void loop() {
   
  ChargeBatterie();

}

void ChargeBatterie() {
//La fonction analogRead de Arduino renvoie une valeur comprise entre 0 et 1023 avec une correspondance linéaire par rapport à la tension de la pin analogique qui doit être entre 0 et 5V
//ValeurADC = Tension * 1023 / 5 et inversement : Tension = ValeurADC * 5 / 1023 ==> 5/1023 = 0.0048
   data = analogRead(A1);
   voltage = data * 0.0048; (5/3600)
   //chargeBat = (voltage * 100) / tension;
   if(voltage > 3.7) {
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

   delay(1000);  
}