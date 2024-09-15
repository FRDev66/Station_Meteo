#include "Wire.h"

int data;
float voltage;

void setup() {
   pinMode(A1, INPUT);
   analogWrite(A1, LOW);

   Serial.begin(9600);
 }

void loop() {
   data = analogRead(A1);
   voltage = data * 0.0048;

   Serial.print(voltage);
   Serial.println(" Volts");

   delay(500);

}