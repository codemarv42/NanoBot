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

void BootPage() {  //The boot overlay is displayed
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
  display.print(F("  "));
  display.print(MODELLNR);
  display.display();

  PlayTone(1);

  delay(1800);
}

void Menu() { //Creates the display menu

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Start program"));

    display.setCursor(0, 25);
    display.println(F("  Calibrate PT"));

    display.setCursor(0, 40);
    display.println(F("  Calibrate Green"));

    display.setCursor(0, 55);
    display.println(F("  Bluetooth"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Calibrate PT"));

    display.setCursor(0, 25);
    display.println(F("  Calibrate Green"));

    display.setCursor(0, 40);
    display.println(F("  Bluetooth"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Calibrate Green"));

    display.setCursor(0, 25);
    display.println(F("  Bluetooth"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Bluetooth"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> System check"));

    display.setCursor(0, 25);
    display.println(F("  Read MPU6050"));

    display.setCursor(0, 40);
    display.println(F("  TOF values"));

    display.setCursor(0, 55);
    display.println(F("  Calibrate Red"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Read MPU6050"));

    display.setCursor(0, 25);
    display.println(F("  TOF values"));

    display.setCursor(0, 40);
    display.println(F("  Calibrate Red"));

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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> TOF values"));

    display.setCursor(0, 25);
    display.println(F("  Calibrate Red"));

    display.display();
  } else if (MenuPage == 8) {  //Light values page
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

    display.setCursor(0, 0);
    display.println(F("Light values"));

    display.setCursor(0, 20);
    display.print("L_A: ");
    display.print(CAL_L[L_A]);

    display.print(" L_I: ");
    display.print(CAL_L[L_I]);

    display.setCursor(0, 35);
    display.print("M: ");
    display.print(CAL_L[M]);

    display.setCursor(0, 50);
    display.print("R_I: ");
    display.print(CAL_L[R_I]);

    display.print(" R_A: ");
    display.print(CAL_L[R_A]);

    display.display();

    Serial.println("");
    Serial.print("L_2:");
    Serial.println(CAL_L[L_A2]);
    Serial.print("L_1:");
    Serial.println(CAL_L[L_A]);
    Serial.print("L_0:");
    Serial.println(CAL_L[L_I]);
    Serial.print("M:");
    Serial.println(CAL_L[M]);
    Serial.print("R_0:");
    Serial.println(CAL_L[R_I]);
    Serial.print("R_1:");
    Serial.println(CAL_L[R_A]);
    Serial.print("R_2:");
    Serial.println(CAL_L[R_A2]);

    Serial.print("REF:");
    Serial.println(RAW_L[REF]);
    Serial.println("");
    Serial.println("");

    delay(200);
  } else if (MenuPage == 9) {  //Gyro values page
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

    display.setCursor(0, 0);
    display.println(F("MPU6050"));

    display.setCursor(0, 20);
    display.print("X: ");
    display.print(XAngle);

    display.setCursor(0, 35);
    display.print("Y: ");
    display.print(YAngle);

    display.setCursor(0, 50);
    display.print("Z: ");
    display.print(ZAngle);

    display.display();

    Serial.println("");
    Serial.print("X:");
    Serial.println(XAngle);
    Serial.print("Y:");
    Serial.println(YAngle);
    Serial.print("Z:");
    Serial.println(ZAngle);
    Serial.println("");
    Serial.println("");

  } else if (MenuPage == 10) {  //TOF values page
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

    display.setCursor(0, 0);
    display.println(F("TOF values"));

    display.setCursor(0, 20);
    display.print("L: ");
    display.print(DistL);
    display.print("mm");

    display.setCursor(0, 35);
    display.print("M: ");
    display.print(DistM);
    display.print("mm");

    display.setCursor(0, 50);
    display.print("R: ");
    display.print(DistR);
    display.print("mm");

    display.display();

    Serial.println("");
    Serial.print("L:");
    Serial.print(DistL);
    Serial.println("mm");
    Serial.print("M:");
    Serial.print(DistM);
    Serial.println("mm");
    Serial.print("R:");
    Serial.print(DistR);
    Serial.println("mm");
    Serial.println("");
    Serial.println("");

  } else if (MenuPage == 11) {
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

    if (WiFiState){
      display.setCursor(0, 0);
      display.print("WIFI: ON");
    }else{
      display.setCursor(0, 0);
      display.print("WIFI: OFF");
    }

    display.setCursor(0, 10);
    display.println(F("> Calibrate Red"));

    display.display();
  }
}

void MenuActions() { //Execute the individual options in the display menu
  if (ENCButtonState == 1) {
    ENCButtonState = 0;
    delay(300);

    if (MenuPage == 0) {
      ENMainProgram = 1;
    } else if (MenuPage == 1) {
      Calibrate(2700);
      PlayTone(0);
    } else if (MenuPage == 2) {
      TCS_Calibrate_EN = true;
      CalibrateTCS34725Green(500, 2);
      TCS_Calibrate_EN = false;
      PlayTone(0);
    } else if (MenuPage == 3) {

    } else if (MenuPage == 4) {
      ShiftRegisterWrite(SR_LED_RGB, HIGH);
      ShiftRegisterWrite(SR_LED_PT, HIGH);
      delay(500);
      MenuPage = 8;
      RGBOff();
      while (ENCButtonState == 0) {
        ReadEncoder();
        MeasureLight();
      }
      ENCButtonState = 0;
      ShiftRegisterWrite(SR_LED_RGB, LOW);
      ShiftRegisterWrite(SR_LED_PT, LOW);
      RGBOff();
      delay(500);
      MenuPage = 4;
    } else if (MenuPage == 5) {

    } else if (MenuPage == 6) {
      RGBOff();
      SetRGB(2,B);
      delay(500);
      MenuPage = 9;
      while (ENCButtonState == 0) {
        ReadEncoder();
      }
      ENCButtonState = 0;
      RGBOff();
      delay(500);
      MenuPage = 6;
    } else if (MenuPage == 7) {
      RGBOff();
      SetRGB(2,B);
      delay(500);
      MenuPage = 10;
      VL53l1xInit(3, 2);
      while (ENCButtonState == 0) {
        ReadEncoder();
      }
      VL53l1xInit(3, 0);
      ENCButtonState = 0;
      RGBOff();
      delay(500);
      MenuPage = 7;
    } else if (MenuPage == 11) {
      TCS_Calibrate_EN = true;
      CalibrateTCS34725Red(500, 2);
      TCS_Calibrate_EN = false;
      PlayTone(0);
    }
  }
}

void MPUCaliPage() { //Note that the gyro is being calibrated is shown on the display
  display.clearDisplay();

  RGBOff();
  SetRGB(2,B);

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

  RGBOff();
}

void BatteryWarning() { //Warning that the battery is low is shown on the display

  PlayTone(2);

  RGBOff();
  SetRGB(2,R);

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

  RGBOff();
}

void EEPROMWarning() { //Warning, that the EEPROM has unvalid values in it

  PlayTone(2);

  RGBOff();
  SetRGB(2,R);

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(18, 15);
  display.print(F("Warning!"));

  display.setTextSize(1);
  display.setCursor(3, 40);
  display.print(F("EEPROM is not valid!"));

  display.display();

  delay(4000);

  RGBOff();
}

void DisplayTilesCase() {  //Line follower status is displayed

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
  } else if (TilesCase == 11) {  //Auto calibrate
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(22, 15);
    display.print(F("Notice!"));

    display.setTextSize(1);
    display.setCursor(0, 40);
    display.print(F("Press button to exit auto calibrate!"));

    display.display();
  } else if (TilesCase == 12) {  //Rescue Bitmap
    display.clearDisplay();

    display.drawBitmap(0, 0, RescueBitmap, 128, 64, SSD1306_WHITE);

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
  } else if (TilesCase == 13) {  //Crossing Bitmap
    display.clearDisplay();

    display.drawBitmap(0, 0, CrossingBitmap, 128, 64, SSD1306_WHITE);

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