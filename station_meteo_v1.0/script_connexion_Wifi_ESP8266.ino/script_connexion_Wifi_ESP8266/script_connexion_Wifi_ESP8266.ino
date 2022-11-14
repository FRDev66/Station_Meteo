/*
Ce programme permet de chercher tous les accès wifi disponibles à portée
*/
#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  // déconnection WiFi si une connexion préalable était faite
  // WiFi.mode(WIFI_STA);
  // WiFi.disconnect();
  // delay(100);

  // Serial.println("Fait");

  // put your setup code here, to run once:
  //Serial.begin(115200);
  WiFi.begin("Livebox-1F90", "o3jwTuDzadcmQAtZ2r");

  Serial.print("Connection en cours");
  while (WiFi.status() != WL_CONNECTED) // tant que l'esp n'est pas connecté au réseau on attends
  {
    delay(3000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connecté, adresse IP : ");
  Serial.println(WiFi.localIP()); // affichage de l'adresse IP
  
} 

void loop() {
  // Serial.println("scan demarre");
  // // WiFi.scan donne le nombre de points wifi disponibles
  // int n = WiFi.scanNetworks();
  // Serial.println("scan terminé");
  // if (n == 0) {
  //   Serial.println("aucun point");
  // } else {
  //   Serial.print(n);
  //   Serial.println(" nombre trouvé :");
  //   for (int i = 0; i < n; ++i) {
  //     // Print SSID and RSSI
  //     Serial.print(i + 1);
  //     Serial.print(": ");
  //     Serial.print(WiFi.SSID(i));
  //     Serial.print(" (");
  //     Serial.print(WiFi.RSSI(i));
  //     Serial.print(")");
  //     Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
  //     delay(10);      
  //   }
  // }
  // Serial.println("");
  // // attente pour un nouveau scan
  // delay(5000);
}