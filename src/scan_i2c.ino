#include <Wire.h>                         // Inclus la bibliothèque Wire.h, afin de pouvoir envoyer des instructions sur le port i2c

int nombreDePeripheriquesTrouves = 0;    // Variable indiquant combien de périphériques I2C ont répondu "présent"

// ========================
// Initialisation programme
// ========================
void setup() {

  Serial.begin(115200);

  Serial.println("                                    ~~ SCANNER I2C ~~                                       ");
  Serial.println("Scanne toutes les adresses i2c, afin de repérer tous les périphériques connectés à l'arduino");
  Serial.println("============================================================================================");
  Serial.println();

  // Initialisation de la liaison i2C
  Wire.begin();

  // Boucle de parcous des 127 adresses i2c possibles
  for (int adresseI2C = 0; adresseI2C < 127; adresseI2C++)
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
  
}

// =================
// Boucle principale
// =================
void loop() {
  // Aucun code ici, car tout se passe dans la fonction setup !
}