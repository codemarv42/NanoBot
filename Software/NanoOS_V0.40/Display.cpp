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
bool LoopState = 0;          //false - Idle, true - run

/////////////////////////////////////////////

void DisplayInit() {  //The display is initalized
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADRESS)) {
    Serial.println(F("SSD1306 allocation failed!"));
  }
  display.display();
  display.clearDisplay();
}

void BootPage() { //The boot overlay is displayed
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(28, 19);
  display.println(F("NanoOS"));

  display.setTextSize(1);
  display.setCursor(0, 55);
  display.println(F("Dev. by Marvin Ramm"));

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("REV: "));
  display.print(VERSION);
  display.print(F(" NR: DEV0"));
  display.print(MODELLNR);
  display.display();

  PlayTone(1);

  delay(1800);
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
    display.println(F("  Calibrate PT"));

    display.setCursor(0, 40);
    display.println(F("  Calibrate RGB"));

    display.setCursor(0, 55);
    display.println(F("  Barrier direction"));

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
    display.println(F("> Calibrate PT"));

    display.setCursor(0, 25);
    display.println(F("  Calibrate RGB"));

    display.setCursor(0, 40);
    display.println(F("  Barrier direction"));

    display.setCursor(0, 55);
    display.println(F("  Light values"));

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
    display.println(F("> Calibrate RGB"));

    display.setCursor(0, 25);
    display.println(F("  Barrier direction"));

    display.setCursor(0, 40);
    display.println(F("  Light values"));

    display.setCursor(0, 55);
    display.println(F("  System check"));

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
    display.println(F("> Barrier direction"));

    display.setCursor(0, 25);
    display.println(F("  Light values"));

    display.setCursor(0, 40);
    display.println(F("  System check"));

    display.setCursor(0, 55);
    display.println(F("  Read MPU6050"));

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
    display.println(F("> Light values"));

    display.setCursor(0, 25);
    display.println(F("  System check"));

    display.setCursor(0, 40);
    display.println(F("  Read MPU6050"));

    display.setCursor(0, 55);
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
    display.println(F("> System check"));

    display.setCursor(0, 25);
    display.println(F("  Read MPU6050"));

    display.setCursor(0, 40);
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
    display.println(F("> Read MPU6050"));

    display.setCursor(0, 25);
    display.println(F("  TOF values"));

    display.display();
  } else if (MenuPage == 7) {
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

void MenuActions() {
  if (ENCButtonState == 1) {
    ENCButtonState = 0;
    delay(300);

    if (MenuPage == 0) {
      ENMainProgram = 1;
    } else if (MenuPage == 1) {
      Calibrate(3000);
      PlayTone(0);
    } else if (MenuPage == 2) {
      TCS_Calibrate_EN = true;
      CalibrateTCS34725(500, 2);
      TCS_Calibrate_EN = false;
      PlayTone(0);
    } else if (MenuPage == 3) {

    } else if (MenuPage == 4) {

    } else if (MenuPage == 5) {

    } else if (MenuPage == 6) {
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

  delay(1000);

  digitalWrite(STAT_LED_BLUE, HIGH);
}

void BatteryWarning() {

  PlayTone(2);

  digitalWrite(STAT_LED_RED, LOW);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(18, 15);
  display.print(F("Warning!"));

  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print(F("Battery is under 3.7V"));

  display.display();

  delay(4000);

  digitalWrite(STAT_LED_RED, HIGH);
}

void DisplayTilesCase() { //Line follower status is displayed

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

  } else if (TilesCase == 7) {  //END
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

  } else if (TilesCase == 8) {  //Obstacle
    display.clearDisplay();

    display.drawBitmap(0, 0, ObstacleBitmap, 128, 64, SSD1306_WHITE);

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
  } else if (TilesCase == 9) {  //Follow line up
    display.clearDisplay();

    display.drawBitmap(0, 0, FollowLineUpBitmap, 128, 64, SSD1306_WHITE);

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
  } else if (TilesCase == 10) {  //Follow line down
    display.clearDisplay();

    display.drawBitmap(0, 0, FollowLineDownBitmap, 128, 64, SSD1306_WHITE);

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