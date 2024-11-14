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

//variables UI



void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}

void setup()
{

}

void loop()
{
  
}