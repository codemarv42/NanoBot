/////////////////////////////////////////////
//          NanoOS by Marvin Ramm          //
//                REV: V0.15               //
//     https://github.com/codemarv42/      //
//        Last modified: 18.11.2022        //
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
   -Widerstände gegen höhere ersetzen der Fototransistoren
   -Debug Modus anzeigen mit USB Symbol

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
}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {

  MeasureLight();
  
  FollowLine(Normal);

}

/////////////////////////////////////////////