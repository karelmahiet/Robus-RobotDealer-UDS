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
#include "LiquidCrystal.h"

//variables UI
//pins
LiquidCrystal lcd(8, 9, 2, 3, 4, 5);

char test[] ="312 cercle";
char test2[] = "leger";
char nbJoueurs1[] = "1";
char nbJoueurs2[] = "2";
char nbJoueurs3[] = "3";
char nbJoueurs4[] = "4";

void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void messageNbJoueurs(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(test);
  lcd.setCursor(0, 1); //ligne suivante
  lcd.print(test2);
  lcd.setCursor(10,1);
  //lcd.print(nbJoueurs1);
}


void setup()
{
  //initialisation LCD
  lcd.begin(16, 2);
  lcd.clear();
  messageNbJoueurs();
}

void loop()
{ 

}