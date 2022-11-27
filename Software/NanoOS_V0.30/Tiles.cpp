///////////////////Includes//////////////////

#include <Arduino.h>

#include "Bitmap.h"
#include "Bluetooth.h"
#include "Display.h"
#include "ESP32_WiFi.h"
#include "Functions.h"
#include "Measure.h"
#include "Pins.h"
#include "Pitches.h"
#include "Settings.h"
#include "Tests.h"
#include "Tiles.h"

/////////////////////////////////////////////

//////////////////Variables//////////////////

unsigned int TilesCase = 0; //0 - Follow line, 1 - Left, 2 - Right, 3 - Gap, 4 - Left Green, 5 - Right Green, 6 - Turn, 7 - End, 8 - Obstacle

/////////////////////////////////////////////

void CaseDistinction(){
  if (CAL_L[L_A] < Black){
    TilesCase = 1;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    LeftBlack();
  }else if (CAL_L[R_A] < Black){
    TilesCase = 2;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    RightBlack();
  }else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)){
    TilesCase = 3;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);

    Gap();
  }else if (DistM < 50){ //in mm
    TilesCase = 8;
    digitalWrite(STAT_LED_RED, LOW);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Stop();
    delay(300);
  }
  
}

void LeftBlack(){
  DriveTime(250);
  while (CAL_L[R_I] > Black){
    MeasureLight();
    Forward(OUT_B, Slow);
    Backward(OUT_A, Slow);
  }

  Align();
}

void RightBlack(){
  DriveTime(250);
  while (CAL_L[L_I] > Black){
    MeasureLight();

    Forward(OUT_A, Slow);
    Backward(OUT_B, Slow);
  }

  Align();
}


void Gap(){

  unsigned long timer = millis();
  unsigned long time = millis();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();

    time = millis();

    Forward(OUT_AB, Slow);

    if (timer + 2300 < time){
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);
      SearchLine();
      break;
    }
  }

}

void SearchLine(){
  unsigned long timer = millis();
  Stop();
  delay(300);
  ResetZAngle();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)){
    MeasureLight(); 
    ResetZAngle();

    while ((ZAngle < 40) && (CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
      MeasureLight();
      Left(Slow);
    }

    while ((ZAngle > -40) && (CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
      MeasureLight();
      Right(Slow);
    }

    while ((ZAngle < 0) && (CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
      MeasureLight();
      Left(Slow);
    }

    timer = millis();
    while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White) && (millis() - timer < 1300)) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }   

  }

}