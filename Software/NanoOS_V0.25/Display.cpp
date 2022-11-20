///////////////////Includes//////////////////

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/////////////////////////////////////////////

//////////////////Variables//////////////////

int MenuPage = 0;
bool ENMainProgram = false;  //false - Idle, true - run
bool LoopState = 0; //false - Idle, true - run

/////////////////////////////////////////////

void DisplayInit() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS)) {
    Serial.println(F("SSD1306 allocation failed!"));
  }
  display.display();
  display.clearDisplay();

  /*
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));
  display.display();
  */
}

void BootPage() {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 25);
  display.println(F("NanoOS"));

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println(F("Dev. by Marvin Ramm"));

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("REV: "));
  display.println(VERSION);
  display.display();

  PlayTone(1);

  delay(2500);
}

void Menu() {

  if (MenuPage == 0) {  //Pointer on
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> Start program"));

    display.setCursor(0, 25);
    display.println(F("  Calibrate"));  //Gyro and Phototransistors

    display.setCursor(0, 40);
    display.println(F("  Barrier direction"));

    display.setCursor(0, 55);
    display.println(F("  Light values"));

    display.display();

  } else if (MenuPage == 1) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> Calibrate"));

    display.setCursor(0, 25);
    display.println(F("  Barrier direction"));

    display.setCursor(0, 40);
    display.println(F("  Light values"));

    display.setCursor(0, 55);
    display.println(F("  System check"));

    display.display();

  } else if (MenuPage == 2) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> Barrier direction"));

    display.setCursor(0, 25);
    display.println(F("  Light values"));

    display.setCursor(0, 40);
    display.println(F("  System check"));

    display.setCursor(0, 55);
    display.println(F("  Read MPU6050"));

    display.display();

  } else if (MenuPage == 3) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> Light values"));

    display.setCursor(0, 25);
    display.println(F("  System check"));

    display.setCursor(0, 40);
    display.println(F("  Read MPU6050"));

    display.setCursor(0, 55);
    display.println(F("  TOF values"));

    display.display();

  } else if (MenuPage == 4) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> System check"));

    display.setCursor(0, 25);
    display.println(F("  Read MPU6050"));

    display.setCursor(0, 40);
    display.println(F("  TOF values"));

    display.display();

  } else if (MenuPage == 5) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> Read MPU6050"));

    display.setCursor(0, 25);
    display.println(F("  TOF values"));

    display.display();

  } else if (MenuPage == 6) {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }

    display.setCursor(0, 10);
    display.println(F("> TOF values"));

    display.display();
  }
}

void MenuActions(){
    if (ENCButtonState == 1){
      ENCButtonState = 0;
      delay(300);
      Serial.println("Button Pressed!");

      if (MenuPage == 0){
        ENMainProgram = 1;
      }else if (MenuPage == 1){
        Calibrate(6000);
        PlayTone(0);
      }else if (MenuPage == 2){
        
      }else if (MenuPage == 3){
        
      }else if (MenuPage == 4){
        
      }else if (MenuPage == 5){
        
      }else if (MenuPage == 6){
        
      }
      
    }
}

void MPUCaliPage() {
  display.clearDisplay();

  digitalWrite(STAT_LED_BLUE, LOW);

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(22, 15);
  display.print(F("Notice!"));

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(F("Do not move the robot"));

  display.display();

  PlayTone(0);

  delay(1500);

  digitalWrite(STAT_LED_BLUE, HIGH);
}

void BatteryWarning(){

  PlayTone(2);

  digitalWrite(STAT_LED_RED, LOW);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(18, 15);
  display.print(F("Warning!"));

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(F("Battery is under 3.5V"));

  display.display();

  delay(1500);  

  digitalWrite(STAT_LED_RED, HIGH);
}

void DisplayTilesCase() {

  if (TilesCase == 0) {  //Pointer on
    display.clearDisplay();

    display.drawBitmap(0, 0, FollowLineBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 1) {
    display.clearDisplay();

    display.drawBitmap(0, 0, LeftBlackBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 2) {
    display.clearDisplay();

    display.drawBitmap(0, 0, RightBlackBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 3) {
    display.clearDisplay();

    display.drawBitmap(0, 0, GapBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 4) {
    display.clearDisplay();

    display.drawBitmap(0, 0, LeftGreenBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 5) {
    display.clearDisplay();

    display.drawBitmap(0, 0, RightGreenBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 6) {
    display.clearDisplay();

    display.drawBitmap(0, 0, TurnBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  } else if (TilesCase == 7) { //END
    display.clearDisplay();

    display.drawBitmap(0, 0, EndBitmap, 128, 64, SSD1306_WHITE);

    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);    
    display.setTextSize(1);

    if (BATVoltage() < 2) {
      display.setCursor(100, 0);
      display.print("USB");
    } else {
      display.setCursor(90, 0);
      display.print(BATVoltage());
      display.print("V");
    }
    
    display.display();

  }
}