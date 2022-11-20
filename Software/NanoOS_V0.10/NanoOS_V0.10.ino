/////////////////////////////////////////////
//          NanoOS by Marvin Ramm          //
//                REV: V0.10               //
//     https://github.com/codemarv42/      //
//        Last modified: 17.11.2022        //
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
   -Voltage Alarm, when Battery is low
   -Widerstände gegen höhere ersetzen der Fototransistoren

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

  MenuPage = 0;
  Menu();
  delay(1000);
  MenuPage = 1;
  Menu();
  delay(1000);
  MenuPage = 2;
  Menu();
  delay(1000);
  MenuPage = 3;
  Menu();
  delay(1000);
  MenuPage = 4;
  Menu();
  delay(1000);
  MenuPage = 5;
  Menu();
  delay(1000);
  MenuPage = 6;
  Menu();
  delay(1000);

  ShiftRegisterWrite(SR_LED_PT, HIGH);

}

/////////////////////////////////////////////

////////////////////Loop/////////////////////

void loop() {

  UpdateMPU6050();

  ReadEncoder();

  Serial.println(ENCCounter);

  //FollowLine(Slow);
  
}

/////////////////////////////////////////////