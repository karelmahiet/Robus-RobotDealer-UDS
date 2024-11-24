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
// #include <SharpIR.h>
// #include "LiquidCrystal.h"

// //initialisation lcd
// LiquidCrystal lcd(8, 9, 2, 3, 4, 7);

// //pins
// int bouton1 = 34; //buton +, oui
// int bouton2 = 33; //bouton -, non
// int bouton3 = 32; //bouton Confirmer

// //variables pour les boutons
// int lastState1 = HIGH, lastState2 = HIGH, lastState3 = HIGH;

// //variables de jeu
// int scores[5];

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
//   return random(1, 11);
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
//       if (nbJoueurs > maxJoueurs)
//         nbJoueurs = maxJoueurs;
//     }

//     if (boutonAppuye(bouton2, lastState2))
//     {
//       nbJoueurs--;
//       if (nbJoueurs < minJoueurs)
//         nbJoueurs = minJoueurs;
//     }

//     if (boutonAppuye(bouton3, lastState3))
//     {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Confirme!");
//       delay(2000);
//       return nbJoueurs;
//     }

//     delay(100);
//   }
// }

// //gere tour joueur
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
//         delay(2000);
//         return score;
//       }
//     }
//     else if (boutonAppuye(bouton2, lastState2))
//     {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Tour termine!");
//       delay(2000);
//       break;
//     }
//   }

//   return score;
// }

// //tour Luigi
// int jouerLuigi()
// {
//   int somme = pigeCarte() + pigeCarte();

//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Luigi's turn!");
//   delay(2000);

//   while (somme < 17)
//   {
//     somme += pigeCarte();

//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Luigi: ");
//     lcd.print(somme);
//     delay(2000);
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
// }

// //affiche le(s) gagnant(s)
// void afficherGagnants(int nbJoueurs, int scoreLuigi)
// {
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Gagnants:");
//   delay(2000);

//   bool gagnantTrouve = false;

//   for (int i = 1; i <= nbJoueurs; i++)
//   {
//     if (scores[i] >= scoreLuigi && scores[i] <= 21)
//     {
//       lcd.clear();
//       lcd.setCursor(0, 0);
//       lcd.print("Joueur ");
//       lcd.print(i);
//       lcd.setCursor(0, 1);
//       lcd.print("Score: ");
//       lcd.print(scores[i]);
//       gagnantTrouve = true;
//       delay(2000);
//     }
//   }

//   if (!gagnantTrouve)
//   {
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Luigi");
//     lcd.setCursor(0, 1);
//     lcd.print("est gagnant!");
//     delay(2000);
//   }
// }

// //jouer une game
// void jouerPartie()
// {
//   int nbJoueurs = nbJoueursPartie();

//   for (int P = 1; P <= nbJoueurs; P++)
//   {
//     scores[P] = pigeCarte() + pigeCarte();
//   }

//   for (int P = 1; P <= nbJoueurs; P++)
//   {
//     scores[P] = hitOrNot(P, scores[P]);
//   }

//   scores[5] = jouerLuigi();

//   afficherGagnants(nbJoueurs, scores[5]);
// }

// void setup()
// {
//   pinMode(bouton1, INPUT_PULLUP);
//   pinMode(bouton2, INPUT_PULLUP);
//   pinMode(bouton3, INPUT_PULLUP);

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
