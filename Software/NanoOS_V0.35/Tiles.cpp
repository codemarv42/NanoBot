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

unsigned int TilesCase = 0;  //0 - Follow line, 1 - Left, 2 - Right, 3 - Gap, 4 - Left Green, 5 - Right Green, 6 - Turn, 7 - End, 8 - Obstacle

/////////////////////////////////////////////

void CaseDistinction() { //Tiles case distinction

  if (TCSL_Green()) {  //When green is detected on the left  
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Stop();
    delay(100);
    if (TCSR_Green()) {
      TilesCase = 6;
      Turn();

    } else {
      while ((CAL_L[L_A] > Black) && (CAL_L[L_A] < White)){
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      Stop();

      delay(100);

      if (CAL_L[L_A] < Black){
        TilesCase = 4;
        while (CAL_L[L_A] < White) {
          MeasureLight();
          Forward(OUT_AB, Slow); 
        }

        Forward(OUT_AB, Slow);
        delay(150);

        RotateAngle(90);

        Forward(OUT_AB, Slow);
        delay(150);

/*        

        while (CAL_L[R_A] > Black + 175) {
          MeasureLight();
          Forward(OUT_B, Slow);
          Backward(OUT_A, Slow);
        }

        */

        Align();
        

/*        
        RotateAngle(80);
        Forward(OUT_AB, Slow);
        delay(600);
        LeftBlack();
        */
      }
    }
  } else if (TCSR_Green()) {  //When green is detected on the right
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Stop();
    delay(100);
    if (TCSL_Green()) {
      TilesCase = 6;
      Turn();

    } else {
      while ((CAL_L[R_A] > Black) && (CAL_L[R_A] < White)){
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      Stop();

      delay(100);

      if (CAL_L[R_A] < Black){
        TilesCase = 5;
        
        while (CAL_L[R_A] < White) {
          MeasureLight();
          Forward(OUT_AB, Slow); 
        }

        Forward(OUT_AB, Slow);
        delay(150);

        RotateAngle(-90);

        Forward(OUT_AB, Slow);
        delay(150);

        /*

        while (CAL_L[L_A] > Black + 175) {
          MeasureLight();

          Forward(OUT_A, Slow);
          Backward(OUT_B, Slow);
        }

        /*

        Align();

        /*
        DriveTime(500);
        RotateAngle(-80);
        Forward(OUT_AB, Slow);
        delay(600);
        RightBlack();
        */
      }
    }
  } else if (CAL_L[L_A] < Black) { //When the left outer sensor sees black
    TilesCase = 1;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    LeftBlack();
  } else if (CAL_L[R_A] < Black) { //When the right outer sensor sees black
    TilesCase = 2;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    RightBlack();
  } else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) { //When all sensors see white
    TilesCase = 3;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);

    Gap();
  } else if (DistM < 40) {  //When the distance sensor measures a certain distance in mm
    TilesCase = 8;
    digitalWrite(STAT_LED_RED, LOW);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Stop();
    delay(1000);
  } 

}

void LeftBlack() {
  while (CAL_L[M] < Black) {
    MeasureLight();
    Forward(OUT_AB, Slow); 
  }

  while (CAL_L[R_A] > Black + 175) {
    MeasureLight();
    Forward(OUT_B, Slow);
    Backward(OUT_A, Slow);
  }

  Align();
}

void RightBlack() {
  while (CAL_L[M] < Black) {
    MeasureLight();
    Forward(OUT_AB, Slow); 
  }

  while (CAL_L[L_A] > Black + 175) {
    MeasureLight();

    Forward(OUT_A, Slow);
    Backward(OUT_B, Slow);
  }

  Align();
}


void Gap() {

  unsigned long timer = millis();
  unsigned long time = millis();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();

    time = millis();

    Forward(OUT_AB, Slow);

    if (timer + 2300 < time) {
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);
      SearchLine();
      break;
    }
  }
}

void SearchLine() { //Systematically search the black line
  unsigned long timer = millis();
  Stop();
  delay(300);
  ResetZAngle();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
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

void Turn() {
  
  Backward(OUT_AB, Slow);
  delay(500);
  RotateAngle(180);
  /*
  while (CAL_L[R_I] > Black + 175) {
    MeasureLight();
    Forward(OUT_B, Slow);
    Backward(OUT_A, Slow);
  }
  */
  Align();
}