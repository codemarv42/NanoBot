       /////////////////////////////////////////////
      //                 NanoOS                  //
     //               Marvin Ramm               //
    //                REV: V0.85               //
   //     https://github.com/codemarv42/      //
  //        Last modified: 11.03.2023        //
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
   Erase all flash: "Disabled"
*/

/*
   Improvements:
   -Save cal Gyro data
   -Menupunkt zum herunterfahren des Roboters
   -Standby modus im Start menu
   -Contest Mode Überprüfen
   -Bezeichnungen der Abkürzungen für kal Werte ändern
   -Speicherung der Sensorwere MIN MAX außen + Prüfen ob kalibrierung funktioniert
   -Debug Modus anzeigen mit USB Symbol
   -Zurücksetzen des Programms mit einen Knopfdruck bsp wenn in einer Loop gefangen bsp im letztem Raum
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
      OTALOOP();
    }
  } else {
    ENMainProgram = 1;
  }

  /*

  if (CalState == false) {
    TilesCase = 11;
    ForceDispUpdate = true;

    RGBOff();
    SetRGB(2,B);
    delay(3500);
    if (ENCButtonState == 1) {
      ENCButtonState = 0;
    } else {
      Calibrate(2500);
      PlayTone(0);
      delay(3000);
    }
    RGBOff();
  }

  */

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  VL53l1xInit(3, 2);
  delay(1500);
  RGBOff();
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {
  if (DEBUGMODE == 1) {

    MeasureLight();
    PrintDataSerial(0);
    Serial.print("Core0 time:");
    Serial.print(Core0Time);
    Serial.println("ms");

  } else {

    if (!DEBUGRESCUE) {

      LoopState = 1;

      MeasureLight();

      CaseDistinction();

      if (XAngle > 15) {  //Ramp up
        TilesCase = 9;
        RGBOff();
        SetRGB(2,G);
        SetRGB(2,B);

        FollowLine(Up);
      } else if (XAngle < -15) {  //Ramp down
        TilesCase = 10;
        RGBOff();
        SetRGB(2,R);
        SetRGB(2,B);

        FollowLine(Down);
      } else {  //Flat ground
        TilesCase = 0;
        RGBOff();
        SetRGB(2,B);

        FollowLine(Normal);
      }

    } else {
      FindExit();
    }
  }
}

/////////////////////////////////////////////