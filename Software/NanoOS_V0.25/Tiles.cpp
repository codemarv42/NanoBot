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

unsigned int TilesCase = 0; //0 - Follow line, 1 - Left, 2 - Right, 3 - Gap, 4 - Left Green, 5 - Right Green, 6 - Turn, 7 -End

/////////////////////////////////////////////

void CaseDistinction(){
  if (CAL_L[L_A] < Black){
    TilesCase = 1;
    LeftBlack();
  }else if (CAL_L[R_A] < Black){
    TilesCase = 2;
    RightBlack();
  }else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)){
    TilesCase = 3;
    Gap();
  }
}

void LeftBlack(){
  DriveTime(250);
  while (CAL_L[R_I] > Black){
    MeasureLight();
    Left(Slow);
  }
  while (CAL_L[L_I] > Black){
    MeasureLight();
    Right(Slow);
  }
  
}

void RightBlack(){
  DriveTime(250);
  while (CAL_L[L_I] > Black){
    MeasureLight();
    Right(Slow);
  }
  while (CAL_L[R_I] > Black){
    MeasureLight();
    Left(Slow);
  }
}

void Gap(){
  ResetZAngle();
  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();
    if ((abs(ZAngle) < 1)){
      Forward(OUT_B, Slow);
      Forward(OUT_A, Slow);
    }else{
      while((abs(ZAngle) > 1)){
        if (ZAngle < 0){
          Left(Slow);
        }else{
          Right(Slow);
        }
      }
    }

  }
}