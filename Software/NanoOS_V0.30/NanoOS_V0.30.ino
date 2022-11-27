/////////////////////////////////////////////
//          NanoOS by Marvin Ramm          //
//                REV: V0.30               //
//     https://github.com/codemarv42/      //
//        Last modified: 26.11.2022        //
//      © Copyright 2022 CC-BY-NC-SA       //
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
   Improvements:
   -Debug Modus anzeigen mit USB Symbol
   -Wenn auf schräge, Bitmap mit Speed anzeigen
   -Automatisches kalibrieren mit vor und zurück fahren

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

  while (ENMainProgram == 0) {
    ReadEncoder();

    if (ENCCounter > 6) {
      ENCCounter = 6;
    }
    MenuPage = ENCCounter;

    MenuActions();
  }

  digitalWrite(STAT_LED_GREEN, HIGH);

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {
  if (DEBUGMODE == 1) {

    MeasureLight();
    PrintDataSerial(1);

  } else {

    LoopState = 1;
    TilesCase = 0;

    digitalWrite(STAT_LED_RED, HIGH);
    digitalWrite(STAT_LED_GREEN, HIGH);
    digitalWrite(STAT_LED_BLUE, HIGH);

    //while ((abs(XAngle) > 60) || (abs(YAngle) > 60)) {  //Pause Menu
    //  LoopState = 0;
    //  Stop();
    //}

    MeasureLight();

    CaseDistinction();

    FollowLine(Normal);

  }
}

/////////////////////////////////////////////