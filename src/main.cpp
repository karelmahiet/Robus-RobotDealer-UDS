/*
Projet: Épreuve du combattant
Equipe: P15
Auteurs: Jérémie Perron
Description: Algorithme de l'octogone
Date: 28 octobre 2024
*/

#include <Arduino.h>
#include <librobus.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <SharpIR.h>

//-----------Capteur de couleur----------
#define commonAnode true
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

//-----------Capteur de distance----------
int InputDetecteObjet = 2;
SharpIR detecteur( SharpIR::GP2Y0A21YK0F, A0);

//--------------Constantes---------------
int nbBitDroite90 = 1800; //1828
int nbBitGauche90 = 1850;  //1850
int nbBitDroite180 = 3656;
int nbBitGauche180 = 3704;

int nbBitRejoindreMilieu = 1750;
int nbBitRejoindreLigne = 2500;
int nbBitRejoindreBut = 5500;


float vitesseDroitRapide = 0.591;
float vitesseGaucheRapide = 0.615;
float vitesseDroitLent = 0.305;
float vitesseGaucheLent = 0.315;
float vitesseDroitTresLent = 0.191;
float vitesseGaucheTresLent = 0.215;
float vitesseRotationRapide = 0.4;
float vitesseRotationLente = 0.1;

int angleOuvertServoGauche = 105;
int angleOuvertServoDroite = 50;
int angleFermeServoGauche = 35;
int angleFermeServoDroite = 120;

float vide = 999;

//-----------Variables mouvement-------------
int nbBitMouvement = 0;
float vitesseDroit = 0;
float vitesseGauche = 0;
float vitesseRotation = 0;

int nbBitRotationScan = 0;
int nbBitTranslationVersObjet = 0;
int directionRotationLivraison = 0; //3: droite, 4: gauche

//----------------Booléens----------------
bool estAuDepart = true;
bool estAuMilieu = false;
bool actionChoisie = false;
bool sequenceEnCours = false;

bool enScan = false;
bool rouleVersObjet = false;
bool positionnementDeCapture = false;
bool captureObjet = false;
bool livreObjet = false;
bool positionnementDeDepot = false;
bool depotObjet = false;
bool positionnementRetourMilieu = false;
bool retourVersMilieu = false;

//------------------Pins-------------------
int PinDistance = A0;
int PinLigneGauche = A1;
int PinLigneMilieu = A2;
int PinLigneDroit = A3;
int PinSon = A4;

int etatLigneGauche = 0;
int etatLigneMilieu = 0;
int etatLigneDroit = 0;

//------------------États-------------------
int etat = 0; // 0:arrêt, 1:avance, 2:recule, 3:TourneDroit, 4:TourneGauche, 5:suivreLigne, 6:detecterCercle
int etatPast = 0;

//-----------------Couleurs-----------------
int couleurEnCours = 1; // 1:rouge, 2:jaune, 3:vert, 4:bleu
int limiteDetectionCouleur = 370;

//-----------------Séquences-----------------
int sequenceEtats[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float sequenceNbBitMouvement[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float sequenceVitesseDroit[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float sequenceVitesseGauche[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float sequenceVitesseRotation[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int compteurSequence = 0;
int nbActionsSequence = 0;

//-----------------Encodeurs-----------------
int32_t encodeurGauche = 0;
int32_t encodeurDroite = 0;

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void ChoisirAction()
{
  if(sequenceEnCours)
  {
      etat = sequenceEtats[compteurSequence];
      vitesseRotation = sequenceVitesseRotation[compteurSequence];
      vitesseDroit = sequenceVitesseDroit[compteurSequence];
      vitesseGauche = sequenceVitesseGauche[compteurSequence];
      nbBitMouvement = sequenceNbBitMouvement[compteurSequence];

      actionChoisie = true;
  }
  else
  {
     etat = 0; //L'état par défaut est l'arrêt
  }
}

//ATTENTION: Il y a un maximum de 10 actions dans une séquence
//Format d'une action à 5 paramètres: (état, vitesseRotation ou vide, vitesseDroit ou vide, vitesseGauche ou vide, nbBitMouvement)
void creerSequence(float nouvelleSequence[10][5], int nbActions)
{
   if(!sequenceEnCours)
   {
      for(int i = 0; i < nbActions; i++)
      {
          //Défini l'état
          sequenceEtats[i] = (int)nouvelleSequence[i][0];

          //Défini la vitesse de rotation
          sequenceVitesseRotation[i] = nouvelleSequence[i][1];

          //Défini les vitesses de chaque moteur pour avancer
          sequenceVitesseDroit[i] = nouvelleSequence[i][2];
          sequenceVitesseGauche[i] = nouvelleSequence[i][3];

          //Défini le nombre de bits comptés par les encodeurs pour un mouvement
          sequenceNbBitMouvement[i] = nouvelleSequence[i][4];
      }
      compteurSequence = 0;
      nbActionsSequence = nbActions;
      sequenceEnCours = true;
   }
}

//Se fait à la fin de chaque action
void actualisationSequence()
{
   if(compteurSequence == nbActionsSequence - 1)
   {
      sequenceEnCours = false;
   }
   else
   {
      compteurSequence++;
   }
}

void modifierSequenceEnCours(float nouvelleSequence[10][5], int nbActions)
{
    sequenceEnCours = false;
    creerSequence(nouvelleSequence, nbActions);
    ChoisirAction();
    ENCODER_Reset(0);
    ENCODER_Reset(1);
}

void arret(bool resetAction){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);

  if(resetAction)
  {
    if(positionnementDeCapture)
    {
      positionnementDeCapture = false;
      captureObjet = true;
    }

    if(positionnementDeDepot && etat != 6)
    {
      positionnementDeDepot = false;
      depotObjet = true;
    }

    if(positionnementRetourMilieu)
    {
      positionnementRetourMilieu = false;
      retourVersMilieu = true;
    }

    actionChoisie = false;
    actualisationSequence();
  }
};

void sequenceTest()
{
    float nouvelleSequence[10][5] = {{5, vide, vide, vide, vide}
                                     };
    creerSequence(nouvelleSequence, 1);
}

void DefinirSequenceDepart(int couleurDepart)
{
    if (couleurDepart == 1) // Si rouge, alors avance un peu, suit la ligne et tourne à gauche de 180 degrés rendu au milieu
    {
        float nouvelleSequence[10][5] = {{1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                         {5, vide, vide, vide, vide},
                                         {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                         {4, vitesseRotationRapide, vide, vide, nbBitGauche180}};
        creerSequence(nouvelleSequence, 4);
    }
    else if (couleurDepart == 2) // Si jaune, alors avance un peu, suit la ligne et tourne à gauche de 90 degrés rendu au milieu
    {
        float nouvelleSequence[10][5] = {{1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                         {5, vide, vide, vide, vide},
                                         {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                         {4, vitesseRotationRapide, vide, vide, nbBitGauche90}};
        creerSequence(nouvelleSequence, 4);
    }
    else if (couleurDepart == 3) // Si vert, alors avance un peu, suit la ligne et tourne à droite de 90 degrés rendu au milieu
    {
        float nouvelleSequence[10][5] = {{1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                         {5, vide, vide, vide, vide},
                                         {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                         {3, vitesseRotationRapide, vide, vide, nbBitDroite90}};
        creerSequence(nouvelleSequence, 4);
    }
    else if (couleurDepart == 4) // Si bleu, alors avance un peu et suit la ligne jusqu'au milieu
    {
        float nouvelleSequence[10][5] = {{1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                         {5, vide, vide, vide, vide},
                                         {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu}};
        creerSequence(nouvelleSequence, 3);
    }
    else
    {
        etat = 0; // État par défaut
    }
}

/*void DefinirSequenceDepart(int couleurDepart)
{
    float nouvelleSequence[10][5];
    int tailleSequence = 0;

    switch (couleurDepart)
    {
    case 1: //Si rouge, alors avance un peu, suit la ligne et tourne à gauche de 180 degrés rendu au milieu
        nouvelleSequence[0][0] = 1; nouvelleSequence[0][1] = vide; nouvelleSequence[0][2] = vitesseDroitLent; nouvelleSequence[0][3] = vitesseGaucheLent; nouvelleSequence[0][4] = nbBitRejoindreLigne;
        nouvelleSequence[1][0] = 5; nouvelleSequence[1][1] = vitesseRotationRapide; nouvelleSequence[1][2] = vitesseDroitRapide; nouvelleSequence[1][3] = vitesseGaucheRapide; nouvelleSequence[1][4] = vide;
        nouvelleSequence[2][0] = 1; nouvelleSequence[2][1] = vide; nouvelleSequence[2][2] = vitesseDroitLent; nouvelleSequence[2][3] = vitesseGaucheLent; nouvelleSequence[2][4] = nbBitRejoindreMilieu;
        nouvelleSequence[3][0] = 4; nouvelleSequence[3][1] = vitesseRotationRapide; nouvelleSequence[3][2] = vide; nouvelleSequence[3][3] = vide; nouvelleSequence[3][4] = nbBitGauche180;
        tailleSequence = 4;
        break;
    case 2: //Si jaune, alors avance un peu, suit la ligne et tourne à gauche de 90 degrés rendu au milieu
        nouvelleSequence[0][0] = 1; nouvelleSequence[0][1] = vide; nouvelleSequence[0][2] = vitesseDroitLent; nouvelleSequence[0][3] = vitesseGaucheLent; nouvelleSequence[0][4] = nbBitRejoindreLigne;
        nouvelleSequence[1][0] = 5; nouvelleSequence[1][1] = vitesseRotationRapide; nouvelleSequence[1][2] = vitesseDroitRapide; nouvelleSequence[1][3] = vitesseGaucheRapide; nouvelleSequence[1][4] = vide;
        nouvelleSequence[2][0] = 1; nouvelleSequence[2][1] = vide; nouvelleSequence[2][2] = vitesseDroitLent; nouvelleSequence[2][3] = vitesseGaucheLent; nouvelleSequence[2][4] = nbBitRejoindreMilieu;
        nouvelleSequence[3][0] = 4; nouvelleSequence[3][1] = vitesseRotationRapide; nouvelleSequence[3][2] = vide; nouvelleSequence[3][3] = vide; nouvelleSequence[3][4] = nbBitGauche90;
        tailleSequence = 4;
        break;
    case 3: //Si vert, alors avance un peu, suit la ligne et tourne à droite de 90 degrés rendu au milieu
        nouvelleSequence[0][0] = 1; nouvelleSequence[0][1] = vide; nouvelleSequence[0][2] = vitesseDroitLent; nouvelleSequence[0][3] = vitesseGaucheLent; nouvelleSequence[0][4] = nbBitRejoindreLigne;
        nouvelleSequence[1][0] = 5; nouvelleSequence[1][1] = vitesseRotationRapide; nouvelleSequence[1][2] = vitesseDroitRapide; nouvelleSequence[1][3] = vitesseGaucheRapide; nouvelleSequence[1][4] = vide;
        nouvelleSequence[2][0] = 1; nouvelleSequence[2][1] = vide; nouvelleSequence[2][2] = vitesseDroitLent; nouvelleSequence[2][3] = vitesseGaucheLent; nouvelleSequence[2][4] = nbBitRejoindreMilieu;
        nouvelleSequence[3][0] = 3; nouvelleSequence[3][1] = vitesseRotationRapide; nouvelleSequence[3][2] = vide; nouvelleSequence[3][3] = vide; nouvelleSequence[3][4] = nbBitDroite90;
        tailleSequence = 4;
        break;
    case 4: //Si bleu, alors avance un peu et suit la ligne jusqu'au milieu
        nouvelleSequence[0][0] = 1; nouvelleSequence[0][1] = vide; nouvelleSequence[0][2] = vitesseDroitLent; nouvelleSequence[0][3] = vitesseGaucheLent; nouvelleSequence[0][4] = nbBitRejoindreLigne;
        nouvelleSequence[1][0] = 5; nouvelleSequence[1][1] = vitesseRotationRapide; nouvelleSequence[1][2] = vitesseDroitRapide; nouvelleSequence[1][3] = vitesseGaucheRapide; nouvelleSequence[1][4] = vide;
        nouvelleSequence[2][0] = 1; nouvelleSequence[2][1] = vide; nouvelleSequence[2][2] = vitesseDroitLent; nouvelleSequence[2][3] = vitesseGaucheLent; nouvelleSequence[2][4] = nbBitRejoindreMilieu;
        tailleSequence = 3;
        break;
    default:
        etat = 0;
        return;
    }

    creerSequence(nouvelleSequence, tailleSequence); //appel après le switch avec la bonne taille
}*/

//Le robot scan seulement le triangle de gauche
void lancerScan()
{
    delay(400);
    float nouvelleSequence[10][5] = {{4, vitesseRotationLente, vide, vide, nbBitGauche90},
                                     {3, vitesseRotationLente, vide, vide, nbBitDroite90},
                                     {4, vitesseRotationLente, vide, vide, nbBitGauche90},
                                     {4, vitesseRotationLente, vide, vide, nbBitGauche90},
                                     {3, vitesseRotationLente, vide, vide, nbBitDroite90}};
    creerSequence(nouvelleSequence, 5);
    
    enScan = true;
}

void scanObjet()
{
    if(detecteur.getDistance() <= 55) //Si on détecte quelque chose à une distance de 40 cm ou moins (à tester), c'est un objet
    {
        arret(false);
        delay(200);

        if(etat == 4)
        {
            directionRotationLivraison = 3;
        }
        else
        {
            directionRotationLivraison = 4;
        }

        //modification de la séquence en cours pour que le robot avance lentement vers l'objet
        float nouvelleSequence[10][5] = {{1, vide, vitesseDroitLent, vitesseGaucheLent, 10000}};
        modifierSequenceEnCours(nouvelleSequence, 1);


        enScan = false;
        rouleVersObjet = true;
    }
}

void deplacementVersObjet()
{
    if(detecteur.getDistance() <= 25) //Si l'objet est à une distance de 10 cm ou moins
    {
          //modification de la séquence en cours pour que le robot avance un peu vers l'avant (nécessaire?) et capture l'objet

        float nouvelleSequence[10][5] = {{0, vide, vide, vide, vide}};
        modifierSequenceEnCours(nouvelleSequence, 1);
          rouleVersObjet = false;
          captureObjet = true;
          positionnementDeCapture = true;
    }
}

void capturerObjet()
{
      //Allumer les servomoteurs
      SERVO_Enable(0);
      SERVO_Enable(1);

      delay(1000);

      //Fermetture des portes
      SERVO_SetAngle(0,angleFermeServoDroite);
      SERVO_SetAngle(1,angleFermeServoGauche);
      

      captureObjet = false;
      livreObjet = true;
}

void livraisonObjet()
{
  Serial.println(nbBitTranslationVersObjet);
  Serial.println(nbBitRotationScan);
   float nouvelleSequence[10][5] = {{6, vide, vide, vide, vide},
                                    {3, vitesseRotationLente, vide, vide, 900},
                                    // {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                    // {5, vide, vide, vide, vide},
                                    // {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreBut},
                                    {1, vide, vitesseDroitLent, vitesseGaucheLent, 14000},
                                    {0, vide, vide, vide, vide}};
    modifierSequenceEnCours(nouvelleSequence, 4);

    livreObjet = false;
    positionnementDeDepot = true;
}

void deposerObjet()
{
//Reset des paramètres de livraison
    nbBitRotationScan = 0;
    nbBitTranslationVersObjet = 0;
    directionRotationLivraison = 0;

    //Ouverture des portes
    SERVO_SetAngle(0,angleOuvertServoDroite);
    SERVO_SetAngle(1,angleOuvertServoGauche);

    delay(400);

    //Étteindre les servomoteurs
    SERVO_Disable(0);
    SERVO_Disable(1);

    float nouvelleSequence[10][5] = {{2, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreBut},
                                     {0, vide, vide, vide, vide}};
    modifierSequenceEnCours(nouvelleSequence, 2);

    depotObjet = false;
    positionnementRetourMilieu = true;
}

void retournerAuMilieu()
{
  ENCODER_Reset(0);
  ENCODER_Reset(1);

  Serial.println("retour");
      if(couleurEnCours == 1) //Si est sur rouge
      {
        Serial.println("rouge1");
          float nouvelleSequence[10][5] = {{4, vitesseRotationRapide, vide, vide, nbBitGauche180},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                       {5, vide, vide, vide, vide},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                       {3, vitesseRotationRapide, vide, vide, nbBitDroite90},
                                       {0, vide, vide, vide, vide}};
          modifierSequenceEnCours(nouvelleSequence, 6);
      }
      else if(couleurEnCours == 3) //Si est sur vert
      {
          float nouvelleSequence[10][5] = {{4, vitesseRotationRapide, vide, vide, nbBitGauche180},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                       {5, vide, vide, vide, vide},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                       {4, vitesseRotationRapide, vide, vide, nbBitGauche90},
                                       {0, vide, vide, vide, vide}};
          modifierSequenceEnCours(nouvelleSequence, 6);
      }
      else //Si est sur jaune ou bleu
      {
        Serial.println("else1");
          float nouvelleSequence[10][5] = {{4, vitesseRotationRapide, vide, vide, nbBitGauche180},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreLigne},
                                       {5, vide, vide, vide, vide},
                                       {1, vide, vitesseDroitLent, vitesseGaucheLent, nbBitRejoindreMilieu},
                                       {0, vide, vide, vide, vide}};
          modifierSequenceEnCours(nouvelleSequence, 5);
      }
      
      retourVersMilieu = false;
}

void verifierConditions()
{
  if(estAuMilieu && !enScan && !actionChoisie)
  {
    lancerScan();
  }

  if(enScan)
  {
    scanObjet();
  }

  if(rouleVersObjet)
  {
    deplacementVersObjet();
  }

  if(captureObjet)
  {
    capturerObjet();
  }

  if(livreObjet)
  {
    livraisonObjet();
  }

  if(depotObjet)
  {
    deposerObjet();
  }

  if(retourVersMilieu)
  {
    retournerAuMilieu();
  }
}

void avance(){
  MOTOR_SetSpeed(RIGHT, vitesseDroit);
  MOTOR_SetSpeed(LEFT, vitesseGauche);

  encodeurDroite = ENCODER_Read(1);

  if(rouleVersObjet||captureObjet)
  {
    nbBitTranslationVersObjet = encodeurDroite;
  }

  if(encodeurDroite > nbBitMouvement && etat != 5)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret(false);
    delay(100);
    actionChoisie = false;
    actualisationSequence();
  }
};

void recule(){
 
  MOTOR_SetSpeed(RIGHT, -vitesseDroit);
  MOTOR_SetSpeed(LEFT, -vitesseGauche);

  encodeurDroite = ENCODER_Read(1);

  if(encodeurDroite < -nbBitMouvement && etat != 5 && etat != 6)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret(false);
    delay(100);
    actionChoisie = false;
    actualisationSequence();
  }
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -vitesseRotation);
  MOTOR_SetSpeed(LEFT, vitesseRotation);

  encodeurGauche = ENCODER_Read(0);

  if(enScan)
  {
    nbBitRotationScan = encodeurGauche;
  }

  if(encodeurGauche > nbBitMouvement && etat != 5)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret(false);
    delay(100);
    nbBitRotationScan = 0;
    actionChoisie = false;
    actualisationSequence();
  }
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, vitesseRotation);
  MOTOR_SetSpeed(LEFT, -vitesseRotation);

  encodeurDroite = ENCODER_Read(1);

  if(enScan)
  {
    nbBitRotationScan = encodeurDroite;
  }

  if(encodeurDroite > nbBitMouvement && etat != 5)
  {
    ENCODER_Reset(0);
    ENCODER_Reset(1);
    arret(false);
    delay(100);
    nbBitRotationScan = 0;
    actionChoisie = false;
    actualisationSequence();
  }
};


int detecterCouleur()
{
  uint16_t clear, red, green, blue;
  int couleur = 0;

 Serial.begin(9600);
    Serial.println("Color View Test!");
    int sonPin = A0;
    pinMode(sonPin, INPUT);
    if (tcs.begin()) {
        Serial.println("Found sensor");
    } else {
        Serial.println("No TCS34725 found ... check your connections");
        while (1); // halt!
    }

    tcs.setInterrupt(false);  // turn on LED

    delay(60);  // takes 50ms to read

    tcs.getRawData(&red, &green, &blue, &clear);

    tcs.setInterrupt(true);  // turn off LED

    /*Serial.print("C:\t"); Serial.print(clear);
    Serial.print("\tR:\t"); Serial.print(red);
    Serial.print("\tG:\t"); Serial.print(green);
    Serial.print("\tB:\t"); Serial.print(blue);*/

    // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    Serial.print("\t");
    //Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
    Serial.println(r);
    Serial.println(g);
    Serial.println(b);
    Serial.println();

    if (r>100&&g>85){
    couleur = 2;
    Serial.print("jaune");
    //jaune
    }
    else if (b>75){
    couleur= 4;
    Serial.print("bleu");
    //bleu
    }
    else if (g>100){
    couleur= 3;
    Serial.print("vert");
    //vert
    }
    else if (r>100){
    couleur = 1;
    Serial.print("rouge");
    //rouge
    }
    else if (r>80&&g>80&&b>60){
    couleur = 5;
    Serial.print("blanc");
    //blanc
    }
    else {
    couleur = 0;
    Serial.print("noir");
    //noir
    }

    return couleur;
}

void detecterCercle()
{
   etatLigneDroit = analogRead(PinLigneDroit);
   etatLigneMilieu = analogRead(PinLigneMilieu);
   etatLigneGauche = analogRead(PinLigneGauche);

    vitesseDroit = vitesseDroitLent;
    vitesseGauche = vitesseGaucheLent;
    recule();

   if(etatLigneGauche > limiteDetectionCouleur && etatLigneMilieu > limiteDetectionCouleur && etatLigneDroit > limiteDetectionCouleur)
   {//Noir, Noir, Noir: arrête

       arret(true);
       delay(150);      
       ENCODER_Reset(0);
       ENCODER_Reset(1);
   }
}

void suivreLigne()
{
    etatLigneDroit = analogRead(PinLigneDroit);
    etatLigneMilieu = analogRead(PinLigneMilieu);
    etatLigneGauche = analogRead(PinLigneGauche);
        if(etatLigneGauche > limiteDetectionCouleur && etatLigneDroit < limiteDetectionCouleur)
        {//Noir, X, Blanc: tourne à gauche
            vitesseDroit = vitesseDroitLent;
            vitesseGauche = vitesseGaucheTresLent;
            avance();
        }
        else if(etatLigneGauche < limiteDetectionCouleur && etatLigneDroit > limiteDetectionCouleur)
        {//Blanc, X, Noir: tourne à droite
            vitesseDroit = vitesseDroitTresLent;
            vitesseGauche = vitesseGaucheLent;
            avance();
        }
        else if(etatLigneGauche > limiteDetectionCouleur && etatLigneMilieu > limiteDetectionCouleur
                && etatLigneDroit > limiteDetectionCouleur)
        {//Noir, Noir, Noir: arrête
             delay(150);
             arret(true);
             ENCODER_Reset(0);
             ENCODER_Reset(1);
             estAuMilieu = true;
        }
        else
        {//Blanc, Noir, Blanc
            vitesseDroit = vitesseDroitLent;
            vitesseGauche = vitesseGaucheLent;
            avance();
        }

    int couleurPresente = detecterCouleur();
    if(couleurPresente != 5 && couleurPresente != 0)
    {
        /*if(couleurPresente == 0)
        {
            estAuMilieu = true;
        }*/
        arret(true);
        delay(150);
        ENCODER_Reset(0);
        ENCODER_Reset(1);
    }
}

void test()
{
   float nouvelleSequence[10][5] = {{3, vitesseRotationLente, vide, vide, 900},
                                   {1, vide, vitesseDroitLent, vitesseGaucheLent, 15000},
                                     {0, vide, vide, vide, vide}};
    creerSequence(nouvelleSequence, 2);
}

void setup()
{
  BoardInit();
  pinMode(PinSon, INPUT);
  pinMode(PinLigneGauche, INPUT);
  pinMode(PinLigneMilieu, INPUT);
  pinMode(PinLigneDroit, INPUT);
  pinMode(PinDistance, INPUT);

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

  delay(100);
  beep(3);
}

void loop()
{
  etatPast = etat;
  if (estAuDepart && (analogRead(PinSon) >= 550 || ROBUS_IsBumper(3))){
      beep(2);
      estAuDepart = false;
      DefinirSequenceDepart(detecterCouleur());
      //DefinirSequenceDepart(3);
      // test();

      //Ouverture des portes initiale
      SERVO_SetAngle(0, 60);
      delay(400);

      //Étteindre les servomoteurs
      SERVO_Disable(0);
      SERVO_Disable(1);

      
  }

  if(!actionChoisie)
  {
    ChoisirAction();
  }

  verifierConditions();

  if (etatPast != etat){ //fait une pause minimale entre les changements d'état
    arret(false);
    delay(50);
  }
  else{
    switch (etat)
    {
    case 0:
      arret(true);
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
      detecterCercle();
      break;
    default:
      avance();
      etat = 1;
    break;
    }
  }

  delay(200); 
}