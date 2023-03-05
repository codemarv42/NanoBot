       /////////////////////////////////////////////
      //                 NanoOS                  //
     //               Marvin Ramm               //
    //                REV: V0.55               //
   //     https://github.com/codemarv42/      //
  //        Last modified: 05.03.2023        //
 //      © Copyright 2023 CC-BY-NC-SA       //
/////////////////////////////////////////////

/*
   Arduino ESP32 Settings:
   Board: "ESP32 Dev Module"
   Upload Speed: "921600"
   CPU Frequency: "240MHz (WiFi/BT)"
   Flash Frequency: "80MHz"
   Flash Mode: "QIO"
   Flash Size: "16MB (128Mb)"
   Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
   Programmer: "AVRISP mkll"
*/

/*
   ~Notice before using the program~

   -After reupload to the robot, all data with light values are gone!
*/

/*
   Improvements:
   -Debug Modus anzeigen mit USB Symbol
   -Räderschutz
   -Zurücksetzen des Programms mit einen Knopfdruck bsp wenn in einer Loop gefangen bsp im letztem Raum
   -Tune Geschwindigkeit nach spannung des Akkus
   -Silbernde Line erkennen und danach das Rescue Programm starten und am ende den Ausgang finden
*/

/*
   Stopped last:

*/

///////////////////Includes//////////////////

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

//////////////////Variables//////////////////


/////////////////////////////////////////////

////////////////////Setup////////////////////

void setup() {
  HardwareInit();

  //MotorTest();

  //Display Menu//

  if (CONTESTMODE == 0) {
    while (ENMainProgram == 0) {
      ReadEncoder();

      if (ENCCounter > 8) {
        ENCCounter = 8;
      }

      if (ENCCounter == 8) {
        MenuPage = 11;
      } else {
        MenuPage = ENCCounter;
      }

      MenuActions();
    }
  } else {
    ENMainProgram = 1;
  }

  if (CalState == false) {
    TilesCase = 11;
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, LOW);
    delay(3500);
    if (ENCButtonState == 1) {
      ENCButtonState = 0;
    } else {
      Calibrate(2500);
      PlayTone(0);
    }
    digitalWrite(STAT_LED_BLUE, HIGH);
  }

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {
  if (DEBUGMODE == 1) {

    MeasureLight();
    PrintDataSerial(0);

  } else {

    LoopState = 1;

    MeasureLight();

    CaseDistinction();

    if (XAngle > 18) {  //Ramp up
      TilesCase = 9;
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, LOW);
      digitalWrite(STAT_LED_BLUE, HIGH);
      FollowLine(Up);
    } else if (XAngle < -18) {  //Ramp down
      TilesCase = 10;
      digitalWrite(STAT_LED_RED, HIGH);
      digitalWrite(STAT_LED_GREEN, LOW);
      digitalWrite(STAT_LED_BLUE, LOW);
      FollowLine(Down);
    } else {  //Flat ground
      TilesCase = 0;
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, LOW);
      FollowLine(Normal);
    }
  }
}

/////////////////////////////////////////////