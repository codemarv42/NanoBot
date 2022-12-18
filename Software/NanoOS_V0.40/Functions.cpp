///////////////////Includes//////////////////

#include <Arduino.h>

#include <Wire.h>
#include <ESP32_Servo.h>
#include <EEPROM.h>

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

Servo ServoOne;
Servo ServoTwo;

TaskHandle_t Loop1;

#define EEPROM_SIZE 32

/////////////////////////////////////////////

//////////////////Variables//////////////////

bool ShiftRegisterBits[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

/////////////////////////////////////////////

////////////////Hardware init////////////////

void HardwareInit() {
  Serial.begin(115200);

  //Output
  pinMode(STAT_LED_RED, OUTPUT);
  pinMode(STAT_LED_GREEN, OUTPUT);
  pinMode(STAT_LED_BLUE, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(SERVO1, OUTPUT);
  pinMode(SERVO2, OUTPUT);

  pinMode(PWMA, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(SHCP, OUTPUT);
  pinMode(STCP, OUTPUT);
  pinMode(DS, OUTPUT);

  //Input
  pinMode(ADC_MULTI, INPUT);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_SW, INPUT);

  ShiftRegisterReset();

  LightTest();

  ServoInit();

  Wire.begin();

  DisplayInit();

  BootPage();

  VL53l0xInit(2);
  VL53l0xInit(3);
  VL53l0xInit(4);

  if ((BATVoltage() < 3.7) && (BATVoltage() > 2)) {
    BatteryWarning();
  }

  MPUCaliPage();

  MPU6060Init();

  TCS34725Init();

  digitalWrite(STAT_LED_GREEN, LOW);

  EEPROM.begin(EEPROM_SIZE);

  EepromLoad();

  xTaskCreatePinnedToCore( //Task is assigned to a processor core
    Loop1Code, /* Task function. */
    "Loop1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Loop1,    /* Task handle to keep track of created task */
    0);        /* pin task to core 0 */
}

/////////////////////////////////////////////

///////////////////LOOPs/////////////////////

void Loop1Code(void* pvParameters) { //Task of a processor core
  Serial.print("Loop1Code running on core ");
  Serial.println(xPortGetCoreID());
  unsigned long timer1 = 0;
  bool DisplayEN = true;
  for (;;) {    
    if (ENMainProgram == 0) {
      if (!TCS_Calibrate_EN) { //When the RGB sensor is calibrated, turn off the display update, otherwise the processor core will crash.
        Menu();
        delay(20);
      } else {
        delay(500);
        Serial.println("CORE 0 is idle...");
      }
    } else { //Main Loop

      if ((TilesCase == 3) || (TilesCase == 6) || (TilesCase == 8) || (TilesCase == 4) || (TilesCase == 5)) {
        if (DisplayEN == true) {
          DisplayTilesCase();
          DisplayEN = false;
        }

        UpdateMPU6050();
      } else if (TilesCase == 7) {  //END
        if (DisplayEN == true) {
          DisplayTilesCase();
          DisplayEN = false;
        }

        UpdateMPU6050();
        UpdateVL53l0x(1);
      } else {

        DisplayEN = true;
        UpdateMPU6050();
        ReadEncoder();
        UpdateVL53l0x(0);
        UpdateTCS34725();

        if ((millis() - timer1) > 200) {
          DisplayTilesCase();
          timer1 = millis();
        }
      }
    }
  }
}

/////////////////////////////////////////////

void ShiftRegisterWrite(int pin, bool state) { //Sets the individual pins of the shift register to HIGH or LOW. Pin table is located in Pins.h

  ShiftRegisterBits[pin] = state;

  digitalWrite(STCP, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, ShiftRegisterBits[i]);
    digitalWrite(SHCP, HIGH);
  }

  digitalWrite(STCP, HIGH);
}

void ShiftRegisterReset() { //Sets all pins of the shift register to LOW
  digitalWrite(STCP, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, 0);
    digitalWrite(SHCP, HIGH);
  }

  digitalWrite(STCP, HIGH);
}

void Forward(byte mot, int v) { //Turns a certain motor forward at a certain speed 
  if (v < -255)
    v = -255;
  else if (v > 255)
    v = 255;
  ShiftRegisterWrite(SR_STBY, HIGH);
  if (v >= 0) {
    if (mot & OUT_A) {
      ShiftRegisterWrite(SR_AIN1, LOW);
      ShiftRegisterWrite(SR_AIN2, HIGH);
      analogWrite(PWMA, v);
    }
    if (mot & OUT_B) {
      ShiftRegisterWrite(SR_BIN1, LOW);
      ShiftRegisterWrite(SR_BIN2, HIGH);
      analogWrite(PWMB, v);
    }
  } else {
    v = -v;
    if (mot & OUT_A) {
      ShiftRegisterWrite(SR_AIN1, HIGH);
      ShiftRegisterWrite(SR_AIN2, LOW);
      analogWrite(PWMA, v);
    }
    if (mot & OUT_B) {
      ShiftRegisterWrite(SR_BIN1, HIGH);
      ShiftRegisterWrite(SR_BIN2, LOW);
      analogWrite(PWMB, v);
    }
  }
}

void Backward(byte mot, int v) { //Turns a certain motor backward at a certain speed
  Forward(mot, -v);
}

void Stop() { //Stops all motors
  ShiftRegisterWrite(SR_STBY, LOW);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
}

void Left(int v) { //The robot is rotated to the left at a certain speed
  Forward(OUT_B, v);
  Backward(OUT_A, v);
}

void Right(int v) { //The robot is rotated to the right at a certain speed
  Forward(OUT_A, v);
  Backward(OUT_B, v);
}

void RotateAngle(int ang) {  //positiv - left, negativ - right //Rotates the robot according to a certain angle
  Stop();
  delay(500);
  ResetZAngle();

  if (ang < 0) {
    while ((ZAngle - CorrectionValueZAngle) > ang) {
      Serial.print(F("zAng: "));
      Serial.println(ZAngle - CorrectionValueZAngle);
      Right(Slow);
    }
  } else {
    while ((ZAngle - CorrectionValueZAngle) < ang) {
      Serial.print(F("zAng: "));
      Serial.println(ZAngle - CorrectionValueZAngle);
      Left(Slow);
    }
  }
}

void ServoInit() { //Ininitalizes all servos
  ServoOne.attach(SERVO1);
  ServoTwo.attach(SERVO2);
}

void Servo1(int pos) {  //0-180 deg //The position of the servo is transferred
  ServoOne.write(pos);
}

void Servo2(int pos) {  //0-180 deg //The position of the servo is transferred
  ServoTwo.write(pos);
}

void FollowLine(int v) { //The robot follows the black line
  int diff = (CAL_L[R_I] - CAL_L[L_I]) * FollowLineDiffFactor;
  if (abs(diff) < 6) {
    Forward(OUT_B, v);
    Forward(OUT_A, v);
  } else {
    int vl, vr;
    if ((CAL_L[L_A] < Green-135) && (CAL_L[L_A] > Black)){
      vl = v + (diff / 7);
      vr = v - (diff / 7);
    }else if ((CAL_L[R_A] < Green-135) && (CAL_L[R_A] > Black)){
      vl = v + (diff / 7);
      vr = v - (diff / 7);
    }else{
      vl = v + (diff / 3);
      vr = v - (diff / 3);
    }
    Forward(OUT_B, vl);
    Forward(OUT_A, vr);
  }
}

void DriveFixedAngle(int time) { //Drive exactly straight with a given time
  Stop();
  delay(500);
  ResetZAngle();
  unsigned long temptime = millis();
  while ((temptime + time) > millis()) {
    if ((abs(ZAngle - CorrectionValueZAngle) < 1)) {
      Forward(OUT_B, Slow);
      Forward(OUT_A, Slow);
    } else {
      while ((abs(ZAngle - CorrectionValueZAngle) > 1)) {
        if (ZAngle - CorrectionValueZAngle < 0) {
          Left(Slow);
        } else {
          Right(Slow);
        }
      }
    }
  }
}

void DriveTime(int time) { //Move forward after a certain time
  unsigned long temptime = millis();
  while ((temptime + time) > millis()) {
    Forward(OUT_AB, Slow);
  }
}

void Align() { //Aligns to the black line
  int diff = 10000;
  while ((diff > 0) && (CAL_L[L_A] > Black) && (CAL_L[R_A] > Black)) {
    MeasureLight();
    diff = (CAL_L[R_A] - CAL_L[L_A]) * FollowLineDiffFactor;
    if (abs(diff) < 80) {
      diff = 0;
    } else {
      int vl = (Slow - 50) + (diff / 3);
      int vr = (Slow - 50) - (diff / 3);
      Forward(OUT_B, vl);
      Forward(OUT_A, vr);
    }
  }
}

void TCASelect(uint8_t i) { //Sets the I2C output / input pin of the I2C distributor
  if (i > 7) return;

  Wire.beginTransmission(I2CMULTI_ADRESS);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void PlayTone(int val) {  //0 - Notification, 1 - Boot, 2 - Warning //Plays an acoustic signal via the buzzer

  if (EN_BUZZER == 1) {
    if (val == 0) {
      tone(BUZZER, NOTE_F5, 400);
    } else if (val == 1) {
      tone(BUZZER, NOTE_F5, 400);
    } else if (val == 2) {
      tone(BUZZER, NOTE_F5, 400);
      noTone(BUZZER);
      delay(1000);
      tone(BUZZER, NOTE_F5, 400);
    }
  } else {
    if (val == 2) {
      tone(BUZZER, NOTE_F5, 400);
      noTone(BUZZER);
      delay(1000);
      tone(BUZZER, NOTE_F5, 400);
    }
  }
}

void EepromSave() { //Saves the values in the EEPROM

  //Light min max//

  for (int i = 0; i < 5; i++) {  //5 Save points, 0-4, EEPROM ADRS. 0-4
    EEPROM.write(i, map(RAW_MIN[i], 0, 4096, 0, 255));
  }

  for (int f = 5; f < 10; f++) {  //5 Save points, 5-9, EEPROM ADRS. 5-9
    EEPROM.write(f, map(RAW_MAX[f - 5], 0, 4096, 0, 255));
  }

  //RGB min max//

  for (int f = 10; f < 15; f++) {  //5 Save points, 10-15, EEPROM ADRS. 10-15
    EEPROM.write(f, map(RAW_TCS_L_G_MAX[f - 10], 0, 1024, 0, 255));
  }

  for (int f = 15; f < 20; f++) {  //5 Save points, 15-20, EEPROM ADRS. 15-20
    EEPROM.write(f, map(RAW_TCS_L_G_MIN[f - 15], 0, 1024, 0, 255));
  }

  for (int f = 20; f < 25; f++) {  //5 Save points, 20-25, EEPROM ADRS. 20-25
    EEPROM.write(f, map(RAW_TCS_R_G_MAX[f - 20], 0, 1024, 0, 255));
  }

  for (int f = 25; f < 30; f++) {  //5 Save points, 25-30, EEPROM ADRS. 25-30
    EEPROM.write(f, map(RAW_TCS_R_G_MIN[f - 25], 0, 1024, 0, 255));
  }

  ///////////////


  EEPROM.commit();  //Saves the changes
}

void EepromLoad() {  //Reads the data from the EEPROM and sets the variables accordingly

  //Light min max//

  for (int i = 0; i < 5; i++) {
    unsigned int a = EEPROM.read(i);
    RAW_MIN[i] = map(a, 0, 255, 0, 4096);
  }

  for (int f = 5; f < 10; f++) {
    unsigned int a = EEPROM.read(f);
    RAW_MAX[f - 5] = map(a, 0, 255, 0, 4096);
  }

  //RGB min max//

  for (int f = 10; f < 15; f++) {
    unsigned int a = EEPROM.read(f);
    RAW_TCS_L_G_MAX[f - 10] = map(a, 0, 255, 0, 1024);
  }

  for (int f = 15; f < 20; f++) {
    unsigned int a = EEPROM.read(f);
    RAW_TCS_L_G_MIN[f - 15] = map(a, 0, 255, 0, 1024);
  }

  for (int f = 20; f < 25; f++) {
    unsigned int a = EEPROM.read(f);
    RAW_TCS_R_G_MAX[f - 20] = map(a, 0, 255, 0, 1024);
  }

  for (int f = 25; f < 30; f++) {
    unsigned int a = EEPROM.read(f);
    RAW_TCS_R_G_MIN[f - 25] = map(a, 0, 255, 0, 1024);
  }

  ///////////////
}

void PrintDataSerial(bool mode) {  //Mode: 0 - Terminal, 1 - Plotter //Outputs the data Serial via the USB port

  if (mode == 0) {

  for (int i = 0; i < 5; i++){
    Serial.print("CAL_L[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(CAL_L[i]);
  }

  /*

    Serial.print("TCS_L_RED: ");
    Serial.print(RAW_TCS_L[R]);
    Serial.print(" TCS_L_GREEN: ");
    Serial.print(RAW_TCS_L[G]);
    Serial.print(" TCS_L_BLUE: ");
    Serial.print(RAW_TCS_L[B]);
    Serial.print(" TCS_L_LUX: ");
    Serial.println(RAW_TCS_L[L]);

    */
    /*
  Serial.print("TCS_R_RED: ");
  Serial.print(RAW_TCS_R[R]);
  Serial.print(" TCS_R_GREEN: ");
  Serial.print(RAW_TCS_R[G]);
  Serial.print(" TCS_R_BLUE: ");
  Serial.print(RAW_TCS_R[B]);
  Serial.print(" TCS_R_LUX: ");
  Serial.println(RAW_TCS_R[L]);

*/

    if (TCSL_Green()) {
      Serial.println("Links Green");
    }

    if (TCSR_Green()) {
      Serial.println("Rechts Green");
    }
    delay(200);

  } else {

    Serial.print("CAL_L_A:");
    Serial.print(CAL_L[L_A]);
    Serial.print(",");
    Serial.print("CAL_L_I:");
    Serial.print(CAL_L[L_I]);
    Serial.print(",");
    Serial.print("CAL_M:");
    Serial.print(CAL_L[M]);
    Serial.print(",");
    Serial.print("CAL_R_I:");
    Serial.print(CAL_L[R_I]);
    Serial.print(",");
    Serial.print("CAL_R_A:");
    Serial.println(CAL_L[R_A]);
  }
}