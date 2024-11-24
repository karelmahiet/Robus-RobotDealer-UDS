// /*
// Projet: Épreuve du combattant
// Equipe: P15
// Auteurs: Karel Mahiet
// Description: Blackjack simplifié
// Date: novembre 2024
// */

// #include <Arduino.h>
// #include <librobus.h>
// #include <Wire.h>
// #include <Adafruit_TCS34725.h>
// #include "LiquidCrystal.h"
// #include "Adafruit_TCS34725.h"

// #define commonAnode true

// // initialisation lcd
// LiquidCrystal lcd(8, 9, 2, 3, 4, 7);

// Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// // pins
// byte gammatable[256];
// int couleur = 0;
// int red = 51;
// int blue = 50;
// int green = 49;
// int bouton1 = 34; // buton +, oui
// int bouton2 = 33; // bouton -, non
// int bouton3 = 32; // bouton Confirmer
// int rougeval = 0;
// int bleuval = 0;
// int vertval = 0;

// // variables pour les boutons
// int lastState1 = HIGH, lastState2 = HIGH, lastState3 = HIGH;

// // variables de jeu
// int scores[5];

// void beep(int hauteur)
// {
//   AX_BuzzerON(hauteur, 100);
// }

// bool boutonAppuye(int pin, int &lastState)
// {
//   int currentState = digitalRead(pin);
//   if (currentState == LOW && lastState == HIGH)
//   {
//     lastState = currentState;
//     delay(50);
//     return true;
//   }
//   lastState = currentState;
//   return false;
// }

// int pigeCarte()
// {
//   int couleur = 0;
//   while (couleur == 0)
//   {
//     uint16_t clear, red, green, blue;

//     tcs.setInterrupt(false); // turn on LED

//     delay(60); // takes 50ms to read

//     tcs.getRawData(&red, &green, &blue, &clear);

//     tcs.setInterrupt(true); // turn off LED

//     Serial.print("C:\t");
//     Serial.print(clear);
//     Serial.print("\tR:\t");
//     Serial.print(red);
//     Serial.print("\tG:\t");
//     Serial.print(green);
//     Serial.print("\tB:\t");
//     Serial.print(blue);

//     // Figure out some basic hex code for visualization
//     uint32_t sum = clear;
//     float r, g, b;
//     r = red;
//     r /= sum;
//     g = green;
//     g /= sum;
//     b = blue;
//     b /= sum;
//     r *= 256;
//     g *= 256;
//     b *= 256;
//     Serial.print("\t");
//     Serial.print((int)r, HEX);
//     Serial.print((int)g, HEX);
//     Serial.print((int)b, HEX);
//     Serial.println();

//     // COULEUR: rouge=1, orange=2, jaune=3, vert=4, bleu=5, mauve=6, rose=7, brun=8, bleuPale=9, beige=10, gris=11

//     if (red > 1100 && red < 1450 && green > 450 && green < 700 && blue > 500 && blue < 800)
//     {
//       couleur = 1;
//       Serial.print("rouge");
//     }
//     else if (red > 1300 && red < 1600 && green > 800 && green < 1150 && blue > 500 && blue < 800)
//     {
//       couleur = 2;
//       Serial.print("orange");
//     }
//     else if (red > 1500 && red < 1900 && green > 1600 && green < 2000 && blue > 850 && blue < 1300)
//     {
//       couleur = 3;
//       Serial.print("jaune");
//     }
//     else if (red > 600 && red < 900 && green > 1250 && green < 1550 && blue > 950 && blue < 1200)
//     {
//       couleur = 4;
//       Serial.print("vert");
//     }
//     else if (red > 250 && red < 500 && green > 900 && green < 1100 && blue > 1550 && blue < 1800)
//     {
//       couleur = 5;
//       Serial.print("bleu");
//     }
//     else if (red > 650 && red < 850 && green > 800 && green < 1050 && blue > 1400 && blue < 1750)
//     {
//       couleur = 6;
//       Serial.print("mauve");
//     }
//     else if (red > 1650 && red < 2200 && green > 1250 && green < 1400 && blue > 1600 && blue < 2100)
//     {
//       couleur = 7;
//       Serial.print("rose");
//     }
//     else if (red > 500 && red < 700 && green > 400 && green < 500 && blue > 300 && blue < 500)
//     {
//       couleur = 8;
//       Serial.print("brun");
//     }
//     else if (red > 1500 && red < 1950 && green > 1300 && green < 1850 && blue > 950 && blue < 1600)
//     {
//       couleur = 9;
//       Serial.print("beige");
//     }
//     else if (red > 1500 && red < 2000 && green > 1950 && green < 2500 && blue > 1950 && blue < 2500)
//     {
//       couleur = 10;
//       Serial.print("blanc");
//     }
//     else
//     {
//       couleur = 0;
//       Serial.print("AUCUNE COULEUR DETECTE");
//       delay(300);
//     }
//   }

//   Serial.print("\t");
//   beep(440);
//   delay(200); // A4
//   beep(494);
//   delay(200); // B4
//   beep(523);
//   delay(200); // C5
//   beep(440);
//   delay(400); // A4
//   delay(5000);

//   return couleur;
// }

// int nbJoueursPartie()
// {
//   int nbJoueurs = 1;
//   const int minJoueurs = 1;
//   const int maxJoueurs = 4;

//   while (true)
//   {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Nb de joueurs?");
//     lcd.setCursor(0, 1);
//     lcd.print(nbJoueurs);

//     if (boutonAppuye(bouton1, lastState1))
//     {
//       nbJoueurs++;
//       beep(784);
//       delay(100); // G5
//       beep(880);
//       delay(100); // A5
//       beep(988);
//       delay(100); // B5
//       beep(1047);
//       delay(300); // C6

//       if (nbJoueurs > maxJoueurs)
//       {
//         nbJoueurs = maxJoueurs;
//         beep(262);
//         delay(300); // C4
//         beep(220);
//         delay(300); // A3
//         beep(196);
//         delay(400); // G3
//       }
//     }

//     if (boutonAppuye(bouton2, lastState2))
//     {
//       nbJoueurs--;
//       beep(1047);
//       delay(100); // C6
//       beep(988);
//       delay(100); // B5
//       beep(880);
//       delay(100); // A5
//       beep(784);
//       delay(300); // G5

//       if (nbJoueurs < minJoueurs)
//       {
//         nbJoueurs = minJoueurs;
//         beep(262);
//         delay(300); // C4
//         beep(220);
//         delay(300); // A3
//         beep(196);
//         delay(400); // G3
//       }
//     }

//     if (boutonAppuye(bouton3, lastState3))
//     {
//       beep(523);
//       delay(100); // C5
//       beep(587);
//       delay(100); // D5
//       beep(659);
//       delay(100); // E5
//       beep(587);
//       delay(200); // D5

//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Confirme!");
//       delay(1000);
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("detection...");
//       return nbJoueurs;
//     }

//     delay(100);
//   }
// }

// // gere tour joueur
// int hitOrNot(int P, int scoreInitial)
// {
//   int score = scoreInitial;

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Joueur ");
//   lcd.print(P);
//   lcd.setCursor(0, 1);
//   lcd.print("Score: ");
//   lcd.print(score);
//   delay(2000);

//   while (true)
//   {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Carte? (oui,non)");

//     if (boutonAppuye(bouton1, lastState1))
//     {

//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("detection...");

//       int carte = pigeCarte();
//       score += carte;

//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Carte: ");
//       lcd.print(carte);
//       lcd.setCursor(0, 1);
//       lcd.print("Score: ");
//       lcd.print(score);
//       delay(2000);

//       if (score > 21)
//       {
//         lcd.clear();
//         lcd.setCursor(0, 0);
//         lcd.print("BUSTED!");
//         delay(1500);
//         return score;
//       }
//     }
//     else if (boutonAppuye(bouton2, lastState2))
//     {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Tour termine!");
//       delay(1500);
//       break;
//     }
//   }

//   return score;
// }

// // affiche le(s) gagnant(s)
// void afficherGagnants(int nbJoueurs, int scoreLuigi)
// {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Gagnants:");
//   delay(2000);

//   bool gagnantTrouve = false;

//   if (scoreLuigi > 21)
//   {
//     scoreLuigi = 0;
//   }

//   for (int i = 0; i <= nbJoueurs; i++)
//   {
//     if (scores[i] > scoreLuigi && scores[i] <= 21)
//     {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Joueur ");
//       lcd.print(i);
//       lcd.setCursor(0, 1);
//       lcd.print("Score: ");
//       lcd.print(scores[i]);
//       gagnantTrouve = true;

//       beep(659);
//       delay(125); // E5
//       beep(523);
//       delay(125); // C5
//       beep(587);
//       delay(125); // D5
//       beep(659);
//       delay(125); // E5
//       beep(784);
//       delay(125); // G5
//       beep(659);
//       delay(125); // E5
//       beep(587);
//       delay(125); // D5
//       beep(523);
//       delay(125); // C5
//       beep(440);
//       delay(200); // A4
//       delay(200); // Pause
//       delay(2000);
//     }
//   }

//   if (!gagnantTrouve && scoreLuigi <= 21)
//   {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Luigi");
//     lcd.setCursor(0, 1);
//     lcd.print("est gagnant!");

//     // Super Mario Bros. Flagpole Fanfare
//     beep(1318);
//     delay(150); // E6
//     beep(987);
//     delay(150); // B5
//     beep(1046);
//     delay(150); // C6
//     beep(1174);
//     delay(150); // D#6
//     beep(1318);
//     delay(150); // E6
//     beep(987);
//     delay(150); // B5
//     beep(1046);
//     delay(150); // C6
//     beep(1174);
//     delay(150); // D#6
//     beep(1318);
//     delay(300); // E6
//     delay(150); // Pause

//     beep(880);
//     delay(150); // A5
//     beep(987);
//     delay(150); // B5
//     beep(1046);
//     delay(150); // C6
//     beep(1174);
//     delay(150); // D#6
//     beep(880);
//     delay(150); // A5
//     beep(987);
//     delay(150); // B5
//     beep(1046);
//     delay(150); // C6
//     beep(1174);
//     delay(150); // D#6
//     beep(1318);
//     delay(300); // E6
//     delay(150); // Pause
//     delay(2000);
//   }
// }

// // tour Luigi
// int jouerLuigi(int nbJoueurs)
// {

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Luigi's turn!");
//   delay(1500);
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("detection...");
//   int somme = pigeCarte() + pigeCarte();

//   while (somme < 17)
//   {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("detection...");

//     somme += pigeCarte();

//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Luigi: ");
//     lcd.print(somme);
//     delay(1000);
//   }

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   if (somme > 21)
//   {
//     lcd.print("Luigi bust!");
//   }
//   else
//   {
//     lcd.print("Luigi stop!");
//   }
//   delay(2000);

//   return somme;
//   afficherGagnants(nbJoueurs, somme);
// }

// // jouer une game
// void jouerPartie()
// {
//   int nbJoueurs = nbJoueursPartie();

//   for (int P = 1; P <= nbJoueurs; P++)
//   {
//     // avance until stop
//     lcd.clear();
//     lcd.write("J"[P]);
//     scores[P] = pigeCarte() + pigeCarte();
//   }

//   for (int P = 1; P <= nbJoueurs; P++)
//   {
//     scores[P] = hitOrNot(P, scores[P]);
//   }

//   scores[5] = jouerLuigi(nbJoueurs);

//   afficherGagnants(nbJoueurs, scores[5]);
// }

// void setup()
// {

//   pinMode(bouton1, INPUT_PULLUP);
//   pinMode(bouton2, INPUT_PULLUP);
//   pinMode(bouton3, INPUT_PULLUP);
//   pinMode(red, OUTPUT);
//   pinMode(blue, OUTPUT);
//   pinMode(green, OUTPUT);

//   Serial.begin(9600);
//   Serial.println("Color View Test!");
//   if (tcs.begin())
//   {
//     Serial.println("Found sensor");
//   }
//   else
//   {
//     Serial.println("No TCS34725 found ... check your connections");
//     while (1)
//       ; // halt!
//   }

//   // thanks PhilB for this gamma table!
//   // it helps convert RGB colors to what humans see
//   for (int i = 0; i < 256; i++)
//   {
//     float x = i;
//     x /= 255;
//     x = pow(x, 2.5);
//     x *= 255;

//     if (commonAnode)
//     {
//       gammatable[i] = 255 - x;
//     }
//     else
//     {
//       gammatable[i] = x;
//     }
//     Serial.println(gammatable[i]);
//   }

//   lcd.begin(16, 2);
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Bienvenue chez");
//   lcd.setCursor(0, 1);
//   lcd.print("Luigi!");
//   delay(2000);

//   randomSeed(analogRead(A0));
// }

// void loop()
// {
//   jouerPartie();
// }
