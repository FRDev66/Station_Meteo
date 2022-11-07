#include <ESP8266WiFi.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin("Livebox-1F90", "o3jwTuDzadcmQAtZ2r");

  Serial.print("Connection en cours");
  while (WiFi.status() != WL_CONNECTED) // tant que l'esp n'est pas connecté au réseau on attends
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connecté, adresse IP : ");
  Serial.println(WiFi.localIP()); // affichage de l'adresse IP
}

void loop() {
  // put your main code here, to run repeatedly:

}
