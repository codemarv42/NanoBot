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

unsigned long GreenTimeout;

/////////////////////////////////////////////

void CaseDistinction() {  //Tiles case distinction

  if ((TCSL_Green() || TCSR_Green()) && ((GreenTimeout + 1000) < millis())) {
    Backward(OUT_AB, Slow);
    delay(20);
    Stop();
    delay(150);
    if (TCSL_Green() && TCSR_Green()) {

      while ((CAL_L[L_I] > Black) && (CAL_L[L_I] < White)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (CAL_L[L_I] <= Black) {  //Black after green when white, green case detection is not valid
        RGBOff();
        SetRGB(2, G);

        TilesCase = 6;
        ForceDispUpdate = true;

        Turn();
      }

    } else if (TCSL_Green()) {

      RotateAngle(-10);

      while ((CAL_L[L_A] > Black) && (CAL_L[L_A] < White)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (CAL_L[L_A] <= Black) {  //Black after green when white, green case detection is not valid
        RGBOff();
        SetRGB(0, G);

        TilesCase = 4;
        ForceDispUpdate = true;

        while (CAL_L[L_A] < Black + 40) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        Forward(OUT_AB, Slow);
        delay(300);

        RotateAngle(85);

        Forward(OUT_AB, Slow);
        delay(325);
      }

      GreenTimeout = millis();

    } else if (TCSR_Green()) {

      RotateAngle(10);

      while ((CAL_L[R_A] > Black) && (CAL_L[R_A] < White)) {
        MeasureLight();
        Forward(OUT_AB, Slow);
      }

      if (CAL_L[R_A] <= Black) {
        RGBOff();
        SetRGB(1, G);

        TilesCase = 5;
        ForceDispUpdate = true;

        while (CAL_L[R_A] < Black + 40) {
          MeasureLight();
          Forward(OUT_AB, Slow);
        }

        Forward(OUT_AB, Slow);
        delay(300);

        RotateAngle(-85);

        Forward(OUT_AB, Slow);
        delay(325);
      }

      GreenTimeout = millis();
    }
  } else if ((CAL_L[L_A2] < Black - 55) || (CAL_L[R_A2] < Black - 55)) {

    if ((CAL_L[L_A2] < Black + 175) && (CAL_L[R_A2] < Black + 175)) {
      RGBOff();
      SetRGB(2, B);

      TilesCase = 13;
      ForceDispUpdate = true;

      long tmptime = millis();

      while((tmptime + 500 > millis()) && !TCSL_Red() && !TCSR_Red()){
        Forward(OUT_AB, Slow);
      }

      if (TCSL_Red() || TCSR_Red()) {
      RGBOff();
      SetRGB(2, R);

      TilesCase = 7;
      ForceDispUpdate = true;

      Stop();

      LightTest();

      delay(6000);

      ShiftRegisterWrite(SR_LED_RGB, HIGH);
      ShiftRegisterWrite(SR_LED_PT, HIGH);

    }

      GreenTimeout = millis();

    } else if (CAL_L[L_A2] < Black - 55) {
      RGBOff();
      SetRGB(0, B);

      TilesCase = 1;
      ForceDispUpdate = true;

      DriveTime(50);

      LeftBlack();
    } else if (CAL_L[R_A2] < Black - 55) {
      RGBOff();
      SetRGB(1, B);

      TilesCase = 2;
      ForceDispUpdate = true;

      DriveTime(50);

      RightBlack();
    }

  } else if (TCSL_Red() || TCSR_Red()) {  //When red is detected
    Backward(OUT_AB, Slow);
    delay(20);
    Stop();
    delay(100);
    if (TCSL_Red() || TCSR_Red()) {
      RGBOff();
      SetRGB(2, R);

      TilesCase = 7;
      ForceDispUpdate = true;

      Stop();

      LightTest();

      delay(6000);

      ShiftRegisterWrite(SR_LED_RGB, HIGH);
      ShiftRegisterWrite(SR_LED_PT, HIGH);

      //PlayMarioTheme();

    }
  } else if ((CAL_L[L_A] > White) && (CAL_L[M] > White) && (CAL_L[R_A] > White) && (CAL_L[L_A2] > White) && (CAL_L[R_A2] > White)) {  //When all sensors see white
    Stop();
    delay(100);
    MeasureLight();
    if (RAW_L[REF] > 550) {
      RGBOff();
      SetRGB(2, R);

      //ENShutdownBT = false;

      FindExit();

      //ENShutdownBT = true;
    } else {

      TilesCase = 3;
      ForceDispUpdate = true;

      RGBOff();
      SetRGB(2, B);

      Gap();
    }
  } else if (DistM < 40) {  //When the distance sensor measures a certain distance in mm

    if (XAngle > -15) {  //Disable Obstacle detection on ramp
      TilesCase = 8;
      ForceDispUpdate = true;

      RGBOff();
      SetRGB(2, R);

      Stop();
      delay(1000);

      if (DistM < 40) {
        delay(1000);
        if (DistM < 40) {
          Obstacle(ObstacleDir);
        }
      }
    }
  }
}

void LeftBlack() {  //Turn left
while (CAL_L[L_A2] < Black + 55) {
    MeasureLight();
    Forward(OUT_AB, Slow);
  }

  while ((CAL_L[R_A] > Black + 175) && (CAL_L[R_A2] > Black + 55)) {
    MeasureLight();
    Forward(OUT_B, Slow);
    Backward(OUT_A, Slow);
  }

  if (CAL_L[R_A2] <= Black + 55) {
    RotateAngle(-25);
    DriveTime(250);
  }
}

void RightBlack() {  //Turn right
  while (CAL_L[R_A2] < Black + 55) {
    MeasureLight();
    Forward(OUT_AB, Slow);
  }

  while ((CAL_L[L_A] > Black + 175) && (CAL_L[L_A2] > Black + 55)) {
    MeasureLight();

    Forward(OUT_A, Slow);
    Backward(OUT_B, Slow);
  }

  if (CAL_L[L_A2] <= Black + 55) {
    RotateAngle(25);
    DriveTime(250);
  }
}

void Gap() {  //Things that are checked in case of a gap

  unsigned long timer = millis();
  unsigned long time = millis();

  while ((CAL_L[L_A] > White - 50) && (CAL_L[M] > White - 50) && (CAL_L[R_A] > White - 50) && (CAL_L[L_A2] > White - 100) && (CAL_L[R_A2] > White - 100)) {
    MeasureLight();

    time = millis();

    Forward(OUT_AB, Slow);

    if (timer + 2650 < time) {  //If the line is not found again after a certain time, search for this
      RGBOff();
      SetRGB(2, R);
      SearchLine();
    }
  }

  if (CAL_L[L_A2] <= White - 100) {
    RotateAngle(45);
    DriveTime(300);
  }

  if (CAL_L[R_A2] <= White - 100) {
    RotateAngle(-45);
    DriveTime(300);
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

  //VL53l1xInit(3, 2);

  while (DistM < 50) {
    Backward(OUT_AB, Slow);
  }

  if (!dir) {  //left
    RotateAngle(90);

    while (DistR < 160) {
      Forward(OUT_AB, Slow);
    }

    DriveTime(1400);

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
        DriveTime(1450);
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
        DriveTime(1450);
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

  //VL53l1xInit(3, 0);
}

void FindExit() {  //Find the exit in Rescue area
  TilesCase = 12;
  ForceDispUpdate = true;

  Stop();
  delay(500);

  DriveTime(2400);

  if (DistL < DistR) {  //turn right
    RotateAngle(-90);
    while ((CAL_L[L_A] > Black + 100) && (CAL_L[M] > Black + 100) && (CAL_L[R_A] > Black + 100)) {
      Forward(OUT_A, Fast);
      Forward(OUT_B, Fast - 30);
      MeasureLight();

      if (DistM < 40) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(45);
        Forward(OUT_AB, Fast);
        delay(200);
        RotateAngle(45);
      }

      if (DistR > 200) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(5);
        DriveTime(100);
        while(DistR >= 200){
          Forward(OUT_AB, Fast);
        }
      }

    }
  } else {  //turn left
    RotateAngle(90);
    while ((CAL_L[L_A] > Black + 100) && (CAL_L[M] > Black + 100) && (CAL_L[R_A] > Black + 100)) {
      Forward(OUT_B, Fast);
      Forward(OUT_A, Fast - 30);
      MeasureLight();

      if (DistM < 40) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(-45);
        Forward(OUT_AB, Fast);
        delay(200);
        RotateAngle(-45);
      }

      if (DistL > 200) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(-5);
        DriveTime(100);
        while(DistL >= 200){
          Forward(OUT_AB, Fast);
        }
      }

    }
  }

  DriveTime(1000);

  Servo2(30);  //Rescue Kit ausladen
  delay(200);
  Servo2(50);
  delay(150);
  Servo2(30);
  delay(150);
  Servo2(50);
  delay(400);
  Servo2(180);

  Backward(OUT_AB, Fast);
  delay(1200);

  //Überprüfen

  if (DistL < DistR) {  //turn right
    RotateAngle(-45);
    Forward(OUT_AB, Fast);
    delay(2000);
    RotateAngle(-45);
    Forward(OUT_AB, Fast);
    delay(1000);

    while (DistL < 160) {
      Forward(OUT_B, Fast);
      Forward(OUT_A, Fast - 30);

      if (DistM < 40) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(-45);
        Forward(OUT_AB, Fast);
        delay(200);
        RotateAngle(-45);
      }

    }

    DriveTime(2000);
    RotateAngle(90);
    DriveTime(2000);

  } else {  //turn left
    RotateAngle(45);
    Forward(OUT_AB, Fast);
    delay(2250);
    RotateAngle(45);
    Forward(OUT_AB, Fast);
    delay(1000);

    while (DistR < 160) {
      Forward(OUT_A, Fast);
      Forward(OUT_B, Fast - 30);

      if (DistM < 40) {  //An Wand angestoßen ohne Rescueplatte zu finden
        RotateAngle(45);
        Forward(OUT_AB, Fast);
        delay(200);
        RotateAngle(45);
      }

    }

    DriveTime(2000);
    RotateAngle(-90);
    DriveTime(2000);
  }
}