/*
Projet: MegaGénial
Auteurs: Équipe P15
Description: Algorithme du dealer
Date: 21 novembre 2024
*/

#include <Arduino.h>
#include <librobus.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "LiquidCrystal.h"

//-----------Capteur de couleur et LCD ----------
#define commonAnode true
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
LiquidCrystal lcd(8, 9, 2, 3, 4, 7);;
int lastState1 = HIGH, lastState2 = HIGH, lastState3 = HIGH;

//--------------Constantes---------------
int nbBitDroite45 = 900;
int nbBitGauche45 = 925;
int nbBitDroite90 = 1780;
int nbBitGauche90 = 1840;
int nbBitDroite180 = 3656;
int nbBitGauche180 = 3704;
int nbBitGauche225 = 4600;

float vitesseDroitRapide = 0.205;
float vitesseGaucheRapide = 0.215;
float vitesseDroitLent = 0.155;
float vitesseGaucheLent = 0.165;
float vitesseRotationRapide = 0.4;
float vitesseRotationLente = 0.2;

//-----------Variables mouvement-------------
int etatPast = 0;
int etat = 0; // 0:arrêt, 1:avance, 2:recule, 3:TourneDroit, 4:TourneGauche, 5:suivreLigne, 6:deposeCarte
int nbBitMouvement = 0;
float vitesseDroit = 0;
float vitesseGauche = 0;
float vitesseRotation = 0;

int nbArretsTour3 = 0;

//----------------Variables BlackJack----------------
int nbJoueurs = 0;
int joueurActif = 5; //commence au Dealer

int nbToursEffectue = 0;

int scores[5] = {0, 0, 0, 0, 0};
int nbAsJoueurs[5] = {0, 0, 0, 0, 0};

//----------------Booléens----------------
bool estAuDepart = true;
bool actionChoisie = false;
bool estEnPause = false;

//------------------Pins-------------------
int PinLigneGauche = A1;
int PinLigneMilieu = A2;
int PinLigneDroit = A3;
int PinMoteur = 13;

int red = 51;
int blue = 50;
int green = 49;
int bouton1 = 34; // buton +, oui
int bouton2 = 33; // bouton -, non
int bouton3 = 32; // bouton Confirmer
int rougeval = 0;
int bleuval = 0;
int vertval = 0;

int etatLigneGauche = 0;
int etatLigneMilieu = 0;
int etatLigneDroit = 0;

//-----------------Couleurs-----------------
int limitedetectionCarte = 150;

//-----------------Encodeurs-----------------
int32_t encodeurGauche = 0;
int32_t encodeurDroite = 0;

void beepAncien(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void beep(int hauteur)
{
  AX_BuzzerON(hauteur, 100);
  delay(200);
  AX_BuzzerOFF();
}

bool boutonAppuye(int pin, int &lastState)
{
  int currentState = digitalRead(pin);
  if (currentState == LOW && lastState == HIGH)
  {
    lastState = currentState;
    delay(50);
    return true;
  }
  lastState = currentState;
  return false;
}

int detectionCarte()
{
  int couleur = 0;
  while (couleur == 0)
  {
    uint16_t clear, red, green, blue;

    tcs.setInterrupt(false); // turn on LED

    delay(60); // takes 50ms to read

    tcs.getRawData(&red, &green, &blue, &clear);

    tcs.setInterrupt(true); // turn off LED

    Serial.print("C:\t");
    Serial.print(clear);
    Serial.print("\tR:\t");
    Serial.print(red);
    Serial.print("\tG:\t");
    Serial.print(green);
    Serial.print("\tB:\t");
    Serial.print(blue);

    // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    r = red;
    r /= sum;
    g = green;
    g /= sum;
    b = blue;
    b /= sum;
    r *= 256;
    g *= 256;
    b *= 256;
    Serial.print("\t");
    Serial.print((int)r, HEX);
    Serial.print((int)g, HEX);
    Serial.print((int)b, HEX);
    Serial.println();

    //COULEUR: rouge=11, orange=2, jaune=3, vert=4, bleu=5, mauve=6, rose=7, brun=8, beige=9, blanc=10

    if (red > 1100 && red < 1450 && green > 450 && green < 700 && blue > 500 && blue < 800)
    {
      couleur = 11;
      Serial.print("rouge");
    }
    else if (red > 1300 && red < 1600 && green > 800 && green < 1150 && blue > 500 && blue < 800)
    {
      couleur = 2;
      Serial.print("orange");
    }
    else if (red > 1500 && red < 1900 && green > 1600 && green < 2000 && blue > 850 && blue < 1300)
    {
      couleur = 3;
      Serial.print("jaune");
    }
    else if (red > 600 && red < 900 && green > 1250 && green < 1550 && blue > 950 && blue < 1200)
    {
      couleur = 4;
      Serial.print("vert");
    }
    else if (red > 250 && red < 500 && green > 900 && green < 1100 && blue > 1550 && blue < 1800)
    {
      couleur = 5;
      Serial.print("bleu");
    }
    else if (red > 650 && red < 850 && green > 800 && green < 1050 && blue > 1400 && blue < 1750)
    {
      couleur = 6;
      Serial.print("mauve");
    }
    else if (red > 1650 && red < 2200 && green > 1250 && green < 1400 && blue > 1600 && blue < 2100)
    {
      couleur = 7;
      Serial.print("rose");
    }
    else if (red > 500 && red < 700 && green > 400 && green < 500 && blue > 300 && blue < 500)
    {
      couleur = 8;
      Serial.print("brun");
    }
    else if (red > 1500 && red < 1950 && green > 1300 && green < 1850 && blue > 950 && blue < 1600)
    {
      couleur = 9;
      Serial.print("beige");
    }
    else if (red > 1500 && red < 2000 && green > 1950 && green < 2500 && blue > 1950 && blue < 2500)
    {
      couleur = 10;
      Serial.print("blanc");
    }
    else
    {
      couleur = 0;
      Serial.print("AUCUNE COULEUR DETECTE");
      delay(200);
    }
  }

  Serial.print("\t");
  beep(440);
  delay(200); // A4
  beep(494);
  delay(200); // B4
  beep(523);
  delay(200); // C5
  beep(440);
  delay(400); // A4
  delay(5000);

  return couleur;
}

void deposerCarte(int valeur)
{
   //Gère le pointage
    bool estAs = false;

    if(valeur == 11)
    {
        estAs = true;
    }

    scores[joueurActif-1] += valeur;
    if(estAs)
    {
      nbAsJoueurs[joueurActif-1] += 1;
    }
    if(scores[joueurActif-1] > 21 && nbAsJoueurs[joueurActif-1] >= 1)
    {
        scores[joueurActif-1] -= 10;
        nbAsJoueurs[joueurActif-1] --;
    }

    //Depose 1 carte
    analogWrite(PinMoteur, 125);
    delay(2000);
    analogWrite(PinMoteur, 0);
}

int calculerNbJoueurs()
{
  int nbJoueurs = 1;
  const int minJoueurs = 1;
  const int maxJoueurs = 4;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nb de joueurs?");

  while (true)
  {
    lcd.setCursor(0, 1);
    lcd.print(nbJoueurs);

    if (boutonAppuye(bouton1, lastState1))
    {
      nbJoueurs++;
      beep(784);
      delay(100); // G5
      beep(880);
      delay(100); // A5
      beep(988);
      delay(100); // B5
      beep(1047);
      delay(300); // C6

      if (nbJoueurs > maxJoueurs)
      {
        nbJoueurs = maxJoueurs;
        beep(262);
        delay(300); // C4
        beep(220);
        delay(300); // A3
        beep(196);
        delay(400); // G3
      }
    }

    if (boutonAppuye(bouton2, lastState2))
    {
      nbJoueurs--;
      beep(1047);
      delay(100); // C6
      beep(988);
      delay(100); // B5
      beep(880);
      delay(100); // A5
      beep(784);
      delay(300); // G5

      if (nbJoueurs < minJoueurs)
      {
        nbJoueurs = minJoueurs;
        beep(262);
        delay(300); // C4
        beep(220);
        delay(300); // A3
        beep(196);
        delay(400); // G3
      }
    }

    if (boutonAppuye(bouton3, lastState3))
    {
      beep(523);
      delay(100); // C5
      beep(587);
      delay(100); // D5
      beep(659);
      delay(100); // E5
      beep(587);
      delay(200); // D5

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Confirme!");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("detection...");
      return nbJoueurs;
    }

    delay(100);
  }
}


// gère tour joueur
void demanderCarte()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Joueur ");
  lcd.print(joueurActif);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(scores[joueurActif-1]);
  delay(2000);

  while (true)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Carte? (oui,non)");

    if (boutonAppuye(bouton1, lastState1))
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("detection...");

      int carte = detectionCarte();
      deposerCarte(carte);
      int nouveauScore = scores[joueurActif-1];

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Carte: ");
      lcd.print(carte);
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(nouveauScore);
      delay(2000);

      if (nouveauScore > 21)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("BUSTED!");
        delay(1500);
      }
    }
    else if (boutonAppuye(bouton2, lastState2))
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Tour termine!");
      delay(1500);
      break;
    }
  }
}

// affiche le(s) gagnant(s)
void afficherGagnants()
{
  int scoreLuigi = scores[4];
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gagnants:");
  delay(2000);

  bool gagnantTrouve = false;

  if (scoreLuigi > 21)
  {
    scoreLuigi = 0;
  }

  for (int i = 0; i <= nbJoueurs; i++)
  {
    if (scores[i] > scoreLuigi && scores[i] <= 21)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Joueur ");
      lcd.print(i);
      lcd.setCursor(0, 1);
      lcd.print("Score: ");
      lcd.print(scores[i]);
      gagnantTrouve = true;

      beep(659);
      delay(125); // E5
      beep(523);
      delay(125); // C5
      beep(587);
      delay(125); // D5
      beep(659);
      delay(125); // E5
      beep(784);
      delay(125); // G5
      beep(659);
      delay(125); // E5
      beep(587);
      delay(125); // D5
      beep(523);
      delay(125); // C5
      beep(440);
      delay(200); // A4
      delay(200); // Pause
      delay(2000);
    }
  }

  if (!gagnantTrouve && scoreLuigi <= 21)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luigi");
    lcd.setCursor(0, 1);
    lcd.print("est gagnant!");

    // Super Mario Bros. Flagpole Fanfare
    beep(1318);
    delay(150); // E6
    beep(987);
    delay(150); // B5
    beep(1046);
    delay(150); // C6
    beep(1174);
    delay(150); // D#6
    beep(1318);
    delay(150); // E6
    beep(987);
    delay(150); // B5
    beep(1046);
    delay(150); // C6
    beep(1174);
    delay(150); // D#6
    beep(1318);
    delay(300); // E6
    delay(150); // Pause

    beep(880);
    delay(150); // A5
    beep(987);
    delay(150); // B5
    beep(1046);
    delay(150); // C6
    beep(1174);
    delay(150); // D#6
    beep(880);
    delay(150); // A5
    beep(987);
    delay(150); // B5
    beep(1046);
    delay(150); // C6
    beep(1174);
    delay(150); // D#6
    beep(1318);
    delay(300); // E6
    delay(150); // Pause
    delay(2000);
  }
}

// tour Luigi
void jouerLuigi()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Luigi's turn!");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("detection...");

  deposerCarte(detectionCarte());
  delay(400);

  while (scores[joueurActif-1] < 17)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("detection...");

    deposerCarte(detectionCarte());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Luigi: ");
    lcd.print(scores[joueurActif-1]);
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  if (scores[joueurActif-1] > 21)
  {
    lcd.print("Luigi bust!");
  }
  else
  {
    lcd.print("Luigi stop!");
  }
  delay(2000);
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};

void avance(){
  MOTOR_SetSpeed(RIGHT, vitesseDroit);
  MOTOR_SetSpeed(LEFT, vitesseGauche);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > nbBitMouvement && etat != 5)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret();
    delay(100);

    etat = 5;
  }
};

void recule(){
 
  MOTOR_SetSpeed(RIGHT, -vitesseDroit);
  MOTOR_SetSpeed(LEFT, -vitesseGauche);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite < -nbBitMouvement)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret();
    delay(100);
  }
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -vitesseRotation);
  MOTOR_SetSpeed(LEFT, vitesseRotation);

  encodeurGauche = ENCODER_Read(0);

  if(encodeurGauche > nbBitMouvement && etat !=5)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret();
    delay(100);
  }
  
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, vitesseRotation);
  MOTOR_SetSpeed(LEFT, -vitesseRotation);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite > nbBitMouvement)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret();
    delay(100);
  }
};

void resetAlgo()
{
    etatPast = 0;
    etat = 0;
    nbBitMouvement = 0;
    vitesseDroit = 0;
    vitesseGauche = 0;
    vitesseRotation = 0;

    nbArretsTour3 = 0;

    //----------------Variables BlackJack----------------
    nbJoueurs = 0;
    joueurActif = 5; //commence au Dealer

    nbToursEffectue = 0;

    for(int i = 0; i < 5; i++)
    {
      scores[i] = 0;
      nbAsJoueurs[i] = 0;
    }

    //----------------Booléens----------------
    estAuDepart = true;
    actionChoisie = false;
    estEnPause = false;

    //------------------Pins-------------------
    etatLigneGauche = 0;
    etatLigneMilieu = 0;
    etatLigneDroit = 0;

    //-----------------Encodeurs-----------------
    encodeurGauche = 0;
    encodeurDroite = 0;

    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Bienvenue chez");
    lcd.setCursor(0, 1);
    lcd.print("Luigi!");
    delay(2000);

    randomSeed(analogRead(A0));

    delay(100);
    beepAncien(3);
}

void detecterLigneNoire()
{
   etatLigneDroit = analogRead(PinLigneDroit);
   etatLigneMilieu = analogRead(PinLigneMilieu);
   etatLigneGauche = analogRead(PinLigneGauche);

   if(etatLigneGauche > limitedetectionCarte && etatLigneMilieu > limitedetectionCarte && etatLigneDroit > limitedetectionCarte)
   {//Noir, Noir, Noir
        ENCODER_Reset(0);
        ENCODER_Reset(1);
        arret();
        delay(100);
   }
}

bool joueurActifPeutJouer()
{
   bool peutJouer = true;

   switch (joueurActif)
    {
    case 1:
      if(scores[joueurActif-1] >= 21 && nbJoueurs < 1)
      {
          peutJouer = false;
      }
      return peutJouer;
      break;
    case 2:
      if(scores[joueurActif-1] >= 21 && nbJoueurs < 2)
      {
          peutJouer = false;
      }
      return peutJouer;
      break;
    case 3:
      if(scores[joueurActif-1] >= 21 && nbJoueurs < 3)
      {
          peutJouer = false;
      }
      return peutJouer;
      break;
    case 4:
      if(scores[joueurActif-1] >= 21 && nbJoueurs < 4)
      {
          peutJouer = false;
      }
      return peutJouer;
      break;

    default:
      peutJouer = false;
      return peutJouer;
      break;
    }
}

void gererDistribution()
{
   /*++joueurActif;

   if(joueurActifPeutJouer())
   {
       if(nbToursEffectue == 0 && joueurActif != 5)
       {
         lcd.clear();
         lcd.write("J"[joueurActif-1]);
         deposerCarte(detectionCarte());
         delay(400);
         deposerCarte(detectionCarte());
       }
       else if(nbToursEffectue == 1)
       {
        if(joueurActif != 5)
        {
          lcd.clear();
          lcd.write("J"[joueurActif-1]);
          demanderCarte();
        }
        else
        {
          lcd.clear();
          lcd.write("Dealer");
          jouerLuigi();
        }
       }
   }

   if(joueurActif == 5) //Le Dealer
   {
      ++nbToursEffectue;
      joueurActif = 0; //reset
   }

   if(nbToursEffectue == 2)
   {
     afficherGagnants();
   }*/

   /*etat = 1;
   vitesseDroit = vitesseDroitRapide;
   vitesseGauche = vitesseGaucheRapide;
   nbBitMouvement = 100;
   ENCODER_Reset(0);
   ENCODER_Reset(1);*/

   delay(600);
   vitesseDroit = vitesseDroitLent;
   vitesseGauche = vitesseGaucheLent;
   avance();
   delay(600);
   etat = 5;
}

void suivreLigne()
{
    etatLigneDroit = analogRead(PinLigneDroit);
    etatLigneMilieu = analogRead(PinLigneMilieu);
    etatLigneGauche = analogRead(PinLigneGauche);

    if(etatLigneGauche > limitedetectionCarte && etatLigneDroit < limitedetectionCarte)
    {//Noir, X, Blanc: tourne vers la gauche
        vitesseDroit = vitesseDroitRapide;
        vitesseGauche = 0;
        avance();
    }
    else if(etatLigneGauche < limitedetectionCarte && etatLigneDroit > limitedetectionCarte)
    {//Blanc, X, Noir: tourne vers la droite
        vitesseDroit = 0;
        vitesseGauche = vitesseGaucheRapide;
        avance();
    }
    else if(etatLigneGauche < limitedetectionCarte && etatLigneMilieu > limitedetectionCarte
            && etatLigneDroit < limitedetectionCarte)
    {//Blanc, Noir, Blanc: avance tout droit 
        vitesseDroit = vitesseDroitRapide;
        vitesseGauche = vitesseGaucheRapide;
        avance();
    }
    else if(etatLigneGauche > limitedetectionCarte && etatLigneMilieu > limitedetectionCarte
            && etatLigneDroit > limitedetectionCarte)
    {//Noir, Noir, Noir
        if(nbToursEffectue != 2) //Dépose les cartes
          {
              arret();
              etat = 6;
          }
          else //Ne s'arrête pas
          {
              ++nbArretsTour3;
              if(nbArretsTour3 == 5)
              {
                  arret();
                  etat = 0;
                  resetAlgo();
              }
              else
              {
                  vitesseDroit = vitesseDroitRapide;
                  vitesseGauche = vitesseGaucheRapide;
                  avance();
              }
          }
    }
    else
    {//Avance tout droit
        vitesseDroit = vitesseDroitRapide;
        vitesseGauche = vitesseGaucheRapide;
        avance();
    }
}

void setup()
{
  BoardInit();
  pinMode(PinLigneGauche, INPUT);
  pinMode(PinLigneMilieu, INPUT);
  pinMode(PinLigneDroit, INPUT);
  pinMode(PinMoteur, INPUT);
  pinMode(bouton1, INPUT_PULLUP);
  pinMode(bouton2, INPUT_PULLUP);
  pinMode(bouton3, INPUT_PULLUP);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(green, OUTPUT);

    for (int i = 0; i < 256; i++)
    {
        float x = i;
        x /= 255;
        x = pow(x, 2.5);
        x *= 255;

        if (commonAnode) {
            gammatable[i] = 255 - x;
        } else {
            gammatable[i] = x;
        }
      Serial.println(gammatable[i]);
    }

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Bienvenue chez");
  lcd.setCursor(0, 1);
  lcd.print("Luigi!");
  delay(2000);

  randomSeed(analogRead(A0));

  delay(100);
  beepAncien(3);
}

void loop()
{
  etatPast = etat;
  if (estAuDepart){
      beepAncien(2);
      estAuDepart = false;
      nbJoueurs = calculerNbJoueurs();

      //Dépose une carte pour le Dealer
       lcd.clear();
       lcd.write("Dealer");
       deposerCarte(detectionCarte());
       lcd.clear();
       lcd.write("Allons-y!");

       joueurActif = 0;
       etat = 5;
  }

  if(boutonAppuye(bouton3, lastState3) && !estEnPause && !estAuDepart)
  {
      estEnPause = true;
      while(estEnPause)
      {
          if(boutonAppuye(bouton3, lastState3))
          {
            estEnPause = false;
          }
      }
  }

  if (etatPast != etat){ //fait une pause minimale entre les changements d'état
    arret();
    delay(50);
  }
  else{
    switch (etat)
    {
    case 0:
      arret();
      break;
    case 1:
      avance();
      break;
    case 2:
      recule();
      break;
    case 3:
      tourneDroit();
      break;
    case 4:
      tourneGauche();
      break;     
    case 5:
      suivreLigne();
      break;      
    case 6:
      gererDistribution();
      break;
    default:
      avance();
      etat = 1;
    break;
    }
  }

  delay(200); 
}