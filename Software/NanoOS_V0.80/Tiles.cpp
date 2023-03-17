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

unsigned int TilesCase = 0;  //0 - Follow line, 1 - Left, 2 - Right, 3 - Gap, 4 - Left Green, 5 - Right Green, 6 - Turn, 7 - End, 8 - Obstacle, 9 - Follow line up, 10 - Follow line down, 12 - Rescue, 13 - Crossing

/////////////////////////////////////////////

void CaseDistinction() {  //Tiles case distinction

  if (TCSL_Green()) {  //When green is detected on the left
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Right(Slow);
    delay(40);
    Forward(OUT_AB, Slow);
    delay(10);
    Stop();
    delay(300);

    if (TCSL_Green()) {

      if (TCSR_Green()) {
        TilesCase = 6;
        ForceDispUpdate = true;

        Turn();

      } else {
        while ((CAL_L[L_A] > Black) && (CAL_L[L_A] < White)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        if (CAL_L[L_A] < Black) {  //Black after green when white, green case detection is not valid
          TilesCase = 4;
          ForceDispUpdate = true;

          while (CAL_L[L_A] < Black + 75) {
            MeasureLight();
            Forward(OUT_AB, Slow);
          }

          Forward(OUT_AB, Slow);
          delay(300);

          RotateAngle(83);

          Forward(OUT_AB, Slow);
          delay(225);
        }
      }
    }
  } else if (TCSR_Green()) {  //When green is detected on the right
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Left(Slow);
    delay(40);
    Forward(OUT_AB, Slow);
    delay(10);
    Stop();
    delay(300);

    if (TCSR_Green()) {

      if (TCSL_Green()) {
        TilesCase = 6;
        ForceDispUpdate = true;
        Turn();

      } else {
        while ((CAL_L[R_A] > Black) && (CAL_L[R_A] < White)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        if (CAL_L[R_A] < Black) {
          TilesCase = 5;
          ForceDispUpdate = true;

          while (CAL_L[R_A] < Black + 75) {
            MeasureLight();
            Forward(OUT_AB, Slow);
          }

          Forward(OUT_AB, Slow);
          delay(300);

          RotateAngle(-83);

          Forward(OUT_AB, Slow);
          delay(225);
        }
      }
    }
  } else if (TCSL_Red() || TCSR_Red()) {  //When red is detected
    digitalWrite(STAT_LED_RED, LOW);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Backward(OUT_AB, Slow);
    delay(5);
    Stop();
    delay(300);

    if (TCSL_Red() && TCSR_Red()) {
      TilesCase = 7;
      ForceDispUpdate = true;

      ShiftRegisterWrite(SR_LED_PT, LOW);
      ShiftRegisterWrite(SR_LED_RGB, LOW);
      PlayTone(2);
      delay(2000);
      PlayTone(2);
      while (ENCButtonState == 0) {
        ReadEncoder();
      }
      ENCButtonState = 0;
      delay(500);
      ShiftRegisterWrite(SR_LED_PT, HIGH);
      ShiftRegisterWrite(SR_LED_RGB, HIGH);
    }
  } else if (CAL_L[L_A] < Black) {  //When the left outer sensor sees black
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    MeasureLight();
    if (CAL_L[R_A] < Black) {
      TilesCase = 13;
      ForceDispUpdate = true;
      DriveTime(150);
      Stop();
      delay(300);
    } else {
      TilesCase = 1;
      ForceDispUpdate = true;
      LeftBlack();
    }

  } else if (CAL_L[R_A] < Black) {  //When the right outer sensor sees black
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    MeasureLight();
    if (CAL_L[L_A] < Black) {
      TilesCase = 13;
      ForceDispUpdate = true;
      DriveTime(150);
      Stop();
      delay(300);
    } else {
      TilesCase = 2;
      ForceDispUpdate = true;
      RightBlack();
    }

  } else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {  //When all sensors see white

    DriveTime(150);
    MeasureLight();
    if (RAW_L_E > 130) {
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);
      FindExit();
    } else {

      TilesCase = 3;
      ForceDispUpdate = true;

      digitalWrite(STAT_LED_RED, HIGH);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, LOW);

      Gap();
    }
  } else if (DistM < 40) {  //When the distance sensor measures a certain distance in mm

    if (XAngle > -18) { //Disable Obstacle detection on ramp
      TilesCase = 8;
      ForceDispUpdate = true;

      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);

      Stop();
      delay(1500);

      if (DistM < 40) {
        delay(1500);
        if (DistM < 40) {
          Obstacle(ObstacleDir);
        }
      }
    }
  }
}

void LeftBlack() {  //Turn left
  if (TilesCase == 13) {
    while (CAL_L[R_A] < Black) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }
  } else {
    while (CAL_L[M] < Black) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }
  }

  while (CAL_L[R_A] > Black + 175) {
    MeasureLight();
    Forward(OUT_B, Slow);
    Backward(OUT_A, Slow);
  }

  Align();
}

void RightBlack() {  //Turn right
  if (TilesCase == 13) {
    while (CAL_L[L_A] < Black) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }
  } else {
    while (CAL_L[M] < Black) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }
  }

  while (CAL_L[L_A] > Black + 175) {
    MeasureLight();

    Forward(OUT_A, Slow);
    Backward(OUT_B, Slow);
  }

  Align();
}

void Gap() {  //Things that are checked in case of a gap

  unsigned long timer = millis();
  unsigned long time = millis();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();

    time = millis();

    Forward(OUT_AB, Slow);

    if (timer + 2450 < time) {  //If the line is not found again after a certain time, search for this
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);
      SearchLine();
    }
  }
}

void SearchLine() {  //Search the black line
  Backward(OUT_AB, Slow);
  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();
  }

  Backward(OUT_AB, Slow);
  delay(400);
  Stop();
  Align();
}

void Turn() {  //Turn 180 degrees

  Backward(OUT_AB, Slow);
  delay(500);
  RotateAngle(190);
}

void Obstacle(bool dir) {  //dir - direction: 0 - left, 1 - right (are defined in the ui) //Go around an obstacle on the line

  while (DistM < 60) {
    Backward(OUT_AB, Slow);
  }

  if (!dir) {  //left
    RotateAngle(90);

    while (DistR < 160) {
      Forward(OUT_AB, Slow);
    }

    DriveTime(1300);

    RotateAngle(-90);

    while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistR > 160)) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }

    if (DistR < 160) {
      while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistR < 160)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (DistR > 160) {
        DriveTime(1150);
        RotateAngle(-90);

        while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }
        DriveTime(400);
        RotateAngle(90);
      } else {
        DriveTime(400);
        RotateAngle(90);
      }
    }

  } else {  //right////////////////

    RotateAngle(-90);

    while (DistL < 160) {
      Forward(OUT_AB, Slow);
    }

    DriveTime(1400);

    RotateAngle(90);

    while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistL > 160)) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }

    if (DistL < 160) {
      while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistL < 160)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (DistL > 160) {
        DriveTime(1250);
        RotateAngle(90);

        while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }
        DriveTime(400);
        RotateAngle(-90);
      } else {
        DriveTime(400);
        RotateAngle(-90);
      }
    }
    
  }
}

void FindExit() {  //Find the exit in Rescue area
  TilesCase = 12;
  ForceDispUpdate = true;

  Stop();
  delay(2000);

  DriveTime(2200);

  RotateAngle(-90);

  while (DistM > 120) {
    Forward(OUT_AB, Normal);
  }

  Stop();
  delay(2000);

  RotateAngle(90);

  while (DistR < 120) {
    Forward(OUT_AB, Normal);
  }

  Stop();
  delay(2000);

  DriveTime(1300);
  Stop();
  delay(500);
  RotateAngle(-90);
  DriveTime(3000);

  /*
  while ((DistL > 80) && (DistR > 80)) {
    Forward(OUT_AB, Slow);
  }

  */

  /* //Algorythmus muss überarbeitet werden, da Ausgang auch auf der Selben Seite wie der Eingang sein kann
  Stop();
  delay(1000);

  DriveTime(500);

  if (DistL < 60){
    while((DistL < 60) || (DistM < 60)){
      Forward(OUT_AB, Normal);
    }

    if (DistL > 60){
      DriveTime(600);
      Stop();
      delay(500);
      RotateAngle(90);
      DriveTime(2000);
    }else{
      //TODO
    }
  }else{
    while((DistR < 60) || (DistM < 60)){
      Forward(OUT_AB, Normal);
    }

    if (DistR > 60){
      DriveTime(600);
      Stop();
      delay(500);
      RotateAngle(-90);
      DriveTime(2000);      
    }else{
      //TODO
    }
  }

  */
}