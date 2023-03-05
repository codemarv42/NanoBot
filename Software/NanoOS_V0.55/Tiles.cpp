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

unsigned int TilesCase = 0;  //0 - Follow line, 1 - Left, 2 - Right, 3 - Gap, 4 - Left Green, 5 - Right Green, 6 - Turn, 7 - End, 8 - Obstacle, 9 - Follow line up, 10 - Follow line down

/////////////////////////////////////////////

void CaseDistinction() {  //Tiles case distinction

  if (TCSL_Green()) {  //When green is detected on the left
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Backward(OUT_AB, Slow);
    delay(5);
    Stop();
    delay(300);

    if (TCSL_Green()) {

      if (TCSR_Green()) {
        TilesCase = 6;
        Turn();

      } else {
        while ((CAL_L[L_A] > Black) && (CAL_L[L_A] < White)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        if (CAL_L[L_A] < Black) {  //Black after green when white, green case detection is not valid
          TilesCase = 4;
          while (CAL_L[L_A] < White) {
            MeasureLight();
            Forward(OUT_AB, Slow);
          }

          Forward(OUT_AB, Slow);
          delay(400);

          RotateAngle(85);

          Forward(OUT_AB, Slow);
          delay(225);
        }
      }
    }
  } else if (TCSR_Green()) {  //When green is detected on the right
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, LOW);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Backward(OUT_AB, Slow);
    delay(5);
    Stop();
    delay(300);

    if (TCSR_Green()) {


      if (TCSL_Green()) {
        TilesCase = 6;
        Turn();

      } else {
        while ((CAL_L[R_A] > Black) && (CAL_L[R_A] < White)) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        if (CAL_L[R_A] < Black) {
          TilesCase = 5;

          while (CAL_L[R_A] < White) {
            MeasureLight();
            Forward(OUT_AB, Slow);
          }

          Forward(OUT_AB, Slow);
          delay(400);

          RotateAngle(-85);

          Forward(OUT_AB, Slow);
          delay(225);
        }
      }
    }
  } else if (TCSL_Red() || TCSR_Red()) { //When red is detected
    digitalWrite(STAT_LED_RED, LOW);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, HIGH);

    Backward(OUT_AB, Slow);
    delay(5);
    Stop();
    delay(300);

    if (TCSL_Red() && TCSR_Red()) {
      TilesCase = 7;
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
    TilesCase = 1;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    LeftBlack();
  } else if (CAL_L[R_A] < Black) {  //When the right outer sensor sees black
    TilesCase = 2;
    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    RightBlack();
  } else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {  //When all sensors see white
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
    delay(1500);

    if (DistM < 40) {
      delay(1500);
      if (DistM < 40) {
        Obstacle(ObstacleDir);
      }
    }
  }
}

void LeftBlack() { //Turn left
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

void RightBlack() { //Turn right
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


void Gap() { //Things that are checked in case of a gap

  unsigned long timer = millis();
  unsigned long time = millis();

  while ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White)) {
    MeasureLight();

    time = millis();

    Forward(OUT_AB, Slow);

    if (timer + 2100 < time) { //If the line is not found again after a certain time, search for this
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

void Turn() { //Turn 180 degrees

  Backward(OUT_AB, Slow);
  delay(500);
  RotateAngle(180);
}

void Obstacle(bool dir) {  //dir - direction: 0 - left, 1 - right (are defined in the ui) //Go around an obstacle on the line

  while (DistM < 60) {
    Backward(OUT_AB, Slow);
  }

  if (!dir) {  //left
    RotateAngle(90);

    while (DistR < 125) {
      Forward(OUT_AB, Slow);
    }

    DriveTime(1300);

    RotateAngle(-90);

    while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistR > 125)) {
      MeasureLight();
      Forward(OUT_AB, Slow);
    }

    if (DistR < 125) {
      while ((CAL_L[L_A] > Black) && (CAL_L[R_A] > Black) && (DistR < 125)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (DistR > 125) {
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

  } else { //right
  //TODO//
  }
}