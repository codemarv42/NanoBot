       /////////////////////////////////////////////
      //                 NanoOS                  //
     //               Marvin Ramm               //
    //                REV: V0.50               //
   //     https://github.com/codemarv42/      //
  //        Last modified: 03.03.2023        //
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
   Improvements:
   -Debug Modus anzeigen mit USB Symbol
   -Räderschutz
   -Beim starten nur einen Knopf drücken wenn gedrückt Programm starten und wenn nicht kalibriert kaliebrieren automatisch
   -Zurücksetzen des Programms mit einen Knopfdruck bsp wenn in einer Loop gefangen
   -Save Green values on PC for tune or more green tones
   -Grünwerte werden nicht richtig gespeichert anscheinend
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

    if (ENCCounter > 7) {
      ENCCounter = 7;
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
    PrintDataSerial(0);

  } else {

    LoopState = 1;

    //while ((abs(XAngle) > 60) || (abs(YAngle) > 60)) {  //Pause Menu
    //  LoopState = 0;
    //  Stop();
    //}

    MeasureLight();

    CaseDistinction();

    if (XAngle > 18){
      TilesCase = 9;
      digitalWrite(STAT_LED_RED, LOW);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, LOW);
      FollowLine(Up);      
    }else if (XAngle < -18){
      TilesCase = 10;
      digitalWrite(STAT_LED_RED, HIGH);
      digitalWrite(STAT_LED_GREEN, LOW);
      digitalWrite(STAT_LED_BLUE, LOW);
      FollowLine(Down);
    }else{
      TilesCase = 0;
      digitalWrite(STAT_LED_RED, HIGH);
      digitalWrite(STAT_LED_GREEN, HIGH);
      digitalWrite(STAT_LED_BLUE, HIGH);
      FollowLine(Normal);
    }

  }
}

/////////////////////////////////////////////