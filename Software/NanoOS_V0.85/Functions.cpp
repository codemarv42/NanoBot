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

#define EEPROM_SIZE 110 //20x 8Bit - light values, 40x 8Bit - green values, 40x 8Bit red values //MAX 500x 8Bit

/////////////////////////////////////////////

//////////////////Variables//////////////////

bool ShiftRegisterBits[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool EEPROMValid = true;
bool ForceDispUpdate = false;

unsigned int Core0Time; //in ms
unsigned int Core0TimeTmp;

/////////////////////////////////////////////

////////////////Hardware init////////////////

void HardwareInit() { //Initalizes all inputs and outputs, as well as all I2C ports
  Serial.begin(115200);

  //Output
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

  pinMode(EN_RST, OUTPUT);
  pinMode(P_ON, OUTPUT);

  //Input
  pinMode(ADC_MULTI, INPUT);
  pinMode(VBAT, INPUT);

  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_SW, INPUT);
  
  pinMode(CHARGE_STAT, INPUT);

  digitalWrite(P_ON, HIGH);

  digitalWrite(EN_RST, LOW);

  ShiftRegisterReset();

  LightTest();

  ServoInit();

  Wire.begin();

  Wire.setClock(3000000); //3 MHz I2C

  DisplayInit();

  BootPage();

  VL53l1xInit(2, 2);
  VL53l1xInit(3, 0);
  VL53l1xInit(4, 2);

  if ((BATVoltage() < 3.7) && (BATVoltage() > 2)) {
    BatteryWarning();
  }

  MPUCaliPage();

  MPU6050Init();

  TCS34725Init();

  EEPROM.begin(EEPROM_SIZE);

  EepromLoad();

  if (!EEPROMValid) {
    EEPROMWarning();
    ShiftRegisterWrite(SR_LED_L_RED, HIGH);
    ShiftRegisterWrite(SR_LED_R_RED, HIGH);
  } else {
    ShiftRegisterWrite(SR_LED_L_GREEN, HIGH);
    ShiftRegisterWrite(SR_LED_R_GREEN, HIGH);
  }

  if (MANUALRGBVAL_G == 1){
    SetTCS_Green(0);
  }

  if (MANUALRGBVAL_R == 1){
    SetTCS_Red(0);
  }

  //if (CONTESTMODE == 0){
  //  OTASetup();
  //}

  Serial.print("Loop running on core:");
  Serial.println(xPortGetCoreID());
  if (xPortGetCoreID() == 0){
    Serial.println("!!!!FAIL use the other Core: 1");
  }

  xTaskCreatePinnedToCore(  //Task is assigned to a processor core
    Loop1Code,              /* Task function. */
    "Loop1",                /* name of task. */
    10000,                  /* Stack size of task */
    NULL,                   /* parameter of the task */
    0,                      /* priority of the task */
    &Loop1,                 /* Task handle to keep track of created task */
    0);                     /* pin task to core 0 */
}

/////////////////////////////////////////////

///////////////////LOOPs/////////////////////

void Loop1Code(void* pvParameters) {  //Task of a processor core 0
  Serial.print("Loop1Code running on core:");
  Serial.println(xPortGetCoreID());
  unsigned long timer1 = 0;
  bool DisplayEN = true;
  for (;;) {
    if (ENMainProgram == 0) {
      if (!TCS_Calibrate_EN) {  //When the RGB sensor is calibrated, turn off the display update, otherwise the processor core will crash.
        Menu();
        delay(20);
      } else {
        delay(500);
        Serial.println("CORE 0 is idle...");
      }

      if (MenuPage == 9) {
        UpdateMPU6050();
      } else if (MenuPage == 10) {
        UpdateVL53l1x(1);  //all TOF Sensors
      }
    } else {  //Main Loop

      CalculateCoreTime(0);

      ReadEncoder();

      if (ENCButtonState == 1) {
        digitalWrite(P_ON, LOW);
      }

      if ((TilesCase == 3) || (TilesCase == 6) || (TilesCase == 4) || (TilesCase == 5)) {
        if (DisplayEN == true) {  //Display update anomaly
          DisplayTilesCase();
          DisplayEN = false;
        }

        UpdateMPU6050();
      } else if ((TilesCase == 12) || (TilesCase == 8)) {  //END
        if (DisplayEN == true) {                          //Display update anomaly
          DisplayTilesCase();
          DisplayEN = false;
        }

        UpdateMPU6050();
        UpdateVL53l1x(1);  //all TOF Sensors
      } else {

        DisplayEN = true;
        UpdateMPU6050();
        UpdateVL53l1x(0);
        UpdateTCS34725();

        if (((millis() - timer1) > 1500) || (ForceDispUpdate == true)) {
          DisplayTilesCase();
          timer1 = millis();
          ForceDispUpdate = false;
        }
      }
      CalculateCoreTime(1);
    }
  }
}

/////////////////////////////////////////////

void ShiftRegisterWrite(int pin, bool state) {  //Sets the individual pins of the shift register to HIGH or LOW. Pin table is located in Pins.h

  if (ShiftRegisterBits[pin] != state) {

    ShiftRegisterBits[pin] = state;

    digitalWrite(STCP, LOW);
    for (int i = 15; i >= 0; i--) {
      digitalWrite(SHCP, LOW);
      digitalWrite(DS, ShiftRegisterBits[i]);
      digitalWrite(SHCP, HIGH);
    }

    digitalWrite(STCP, HIGH);
  }
}

void ShiftRegisterReset() {  //Sets all pins of the shift register to LOW
  digitalWrite(STCP, LOW);
  for (int i = 15; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, 0);
    digitalWrite(SHCP, HIGH);
  }

  digitalWrite(STCP, HIGH);
}

void Forward(byte mot, int v) {  //Turns a certain motor forward at a certain speed
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

void Backward(byte mot, int v) {  //Turns a certain motor backward at a certain speed
  Forward(mot, -v);
}

void Stop() {  //Stops all motors
  ShiftRegisterWrite(SR_STBY, LOW);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
}

void Left(int v) {  //The robot is rotated to the left at a certain speed
  Forward(OUT_B, v);
  Backward(OUT_A, v);
}

void Right(int v) {  //The robot is rotated to the right at a certain speed
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

void ServoInit() {  //Ininitalizes all servos
  ServoOne.attach(SERVO1);
  ServoTwo.attach(SERVO2);
}

void Servo1(int pos) {  //0-180 deg //The position of the servo is transferred
  ServoOne.write(pos);
}

void Servo2(int pos) {  //0-180 deg //The position of the servo is transferred
  ServoTwo.write(pos);
}

void FollowLine(int v) {  //The robot follows the black line
  int diff = (CAL_L[R_A] - CAL_L[L_A]) * FollowLineDiffFactor;


  if (abs(diff) < 10) {
    Forward(OUT_B, v);
    Forward(OUT_A, v);
  } else {
    int vl = v + (diff / 2);
    int vr = v - (diff / 2);

    if ((vl < 30) && (vl > 0)){
      Forward(OUT_B, 40);      
    }else if ((vl > -30) && (vl < 0)){
      Backward(OUT_B, -40);      
    }else{
      Forward(OUT_B, vl);
    }

    if ((vr < 30) && (vr > 0)){
      Forward(OUT_B, 40);      
    }else if ((vr > -30) && (vr < 0)){
      Backward(OUT_A, -40);      
    }else{
      Forward(OUT_A, vr);
    }

  }

}

void DriveFixedAngle(int time) {  //Drive exactly straight with a given time
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

void DriveTime(int time) {  //Move forward after a certain time
  unsigned long temptime = millis();
  while ((temptime + time) > millis()) {
    Forward(OUT_AB, Slow);
  }
}

void Align() {  //Aligns to the black line
  int diff;
  while ((diff > 0) && (CAL_L[L_A2] > Black) && (CAL_L[R_A2] > Black)) {
    MeasureLight();
    diff = (CAL_L[R_A2] - CAL_L[L_A2]) * 2;
    if (abs(diff) < 150) {
      diff = 0;
    } else {
      int vl = (Slow - 50) + (diff / 3);
      int vr = (Slow - 50) - (diff / 3);
      Forward(OUT_B, vl);
      Forward(OUT_A, vr);
    }
  }
}

void TCASelect(uint8_t i) {  //Sets the I2C output / input pin of the I2C distributor
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

void EepromSave() {  //Saves the values in the EEPROM

  //Light min max//

  for (int i = 0; i < 10; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_MIN[i / 2]);
  }

  for (int i = 10; i < 20; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_MAX[(i - 10) / 2]);
  }

  //RGB min max green//

  Serial.println("");

  for (int i = 20; i < 30; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_L_G_MAX[(i - 20) / 2]);

    Serial.print("RAW_TCS_L_G_MAX[");
    Serial.print((i - 20) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_G_MAX[(i - 20) / 2]);
  }

  Serial.println("");

  for (int i = 30; i < 40; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_L_G_MIN[(i - 30) / 2]);

    Serial.print("RAW_TCS_L_G_MIN[");
    Serial.print((i - 30) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_G_MIN[(i - 30) / 2]);
  }

  Serial.println("");

  for (int i = 40; i < 50; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_R_G_MAX[(i - 40) / 2]);

    Serial.print("RAW_TCS_R_G_MAX[");
    Serial.print((i - 40) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_G_MAX[(i - 40) / 2]);
  }

  Serial.println("");

  for (int i = 50; i < 60; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_R_G_MIN[(i - 50) / 2]);

    Serial.print("RAW_TCS_R_G_MIN[");
    Serial.print((i - 50) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_G_MIN[(i - 50) / 2]);
  }

  //RGB min max red//

  Serial.println("");

  for (int i = 60; i < 70; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_L_R_MAX[(i - 60) / 2]);

    Serial.print("RAW_TCS_L_R_MAX[");
    Serial.print((i - 60) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_R_MAX[(i - 60) / 2]);
  }

  Serial.println("");

  for (int i = 70; i < 80; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_L_R_MIN[(i - 70) / 2]);

    Serial.print("RAW_TCS_L_R_MIN[");
    Serial.print((i - 70) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_R_MIN[(i - 70) / 2]);
  }

  Serial.println("");

  for (int i = 80; i < 90; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_R_R_MAX[(i - 80) / 2]);

    Serial.print("RAW_TCS_R_R_MAX[");
    Serial.print((i - 80) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_R_MAX[(i - 80) / 2]);
  }

  Serial.println("");

  for (int i = 90; i < 100; i += 2) {
    writeUnsignedIntIntoEEPROM(i, RAW_TCS_R_R_MIN[(i - 90) / 2]);

    Serial.print("RAW_TCS_R_R_MIN[");
    Serial.print((i - 90) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_R_MIN[(i - 90) / 2]);
  }

  Serial.println("");

  EEPROM.commit();  //Saves the changes

  Serial.println("All saved.");
}

void EepromLoad() {  //Read the data from the EEPROM and set the variables accordingly

  //Light min max//

  for (int i = 0; i < 10; i += 2) {
    RAW_MIN[i / 2] = readUnsignedIntFromEEPROM(i);
    if (RAW_MIN[i / 2] == 65535) {
      EEPROMValid = false;
    }
  }

  for (int i = 10; i < 20; i += 2) {
    RAW_MAX[(i - 10) / 2] = readUnsignedIntFromEEPROM(i);
    if (RAW_MAX[(i - 10) / 2] == 65535) {
      EEPROMValid = false;
    }
  }

  //RGB min max green//

  Serial.println("");

  for (int i = 20; i < 30; i += 2) {
    RAW_TCS_L_G_MAX[(i - 20) / 2] = readUnsignedIntFromEEPROM(i);

    if (RAW_TCS_L_G_MAX[(i - 20) / 2] == 65535) {
      EEPROMValid = false;
    }

    Serial.print("RAW_TCS_L_G_MAX[");
    Serial.print((i - 20) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_G_MAX[(i - 20) / 2]);
  }

  Serial.println("");

  for (int i = 30; i < 40; i += 2) {
    RAW_TCS_L_G_MIN[(i - 30) / 2] = readUnsignedIntFromEEPROM(i);

    if (RAW_TCS_L_G_MIN[(i - 30) / 2] == 65535) {
      EEPROMValid = false;
    }

    Serial.print("RAW_TCS_L_G_MIN[");
    Serial.print((i - 30) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_G_MIN[(i - 30) / 2]);
  }

  Serial.println("");

  for (int i = 40; i < 50; i += 2) {
    RAW_TCS_R_G_MAX[(i - 40) / 2] = readUnsignedIntFromEEPROM(i);

    if (RAW_TCS_R_G_MAX[(i - 40) / 2] == 65535) {
      EEPROMValid = false;
    }

    Serial.print("RAW_TCS_R_G_MAX[");
    Serial.print((i - 40) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_G_MAX[(i - 40) / 2]);
  }

  Serial.println("");

  for (int i = 50; i < 60; i += 2) {
    RAW_TCS_R_G_MIN[(i - 50) / 2] = readUnsignedIntFromEEPROM(i);

    if (RAW_TCS_R_G_MIN[(i - 50) / 2] == 65535) {
      EEPROMValid = false;
    }

    Serial.print("RAW_TCS_R_G_MIN[");
    Serial.print((i - 50) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_G_MIN[(i - 50) / 2]);
  }

  //RGB min max red//

  Serial.println("");

  for (int i = 60; i < 70; i += 2) {
    RAW_TCS_L_R_MAX[(i - 60) / 2] = readUnsignedIntFromEEPROM(i);

    Serial.print("RAW_TCS_L_R_MAX[");
    Serial.print((i - 60) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_R_MAX[(i - 60) / 2]);
  }

  Serial.println("");

  for (int i = 70; i < 80; i += 2) {
    RAW_TCS_L_R_MIN[(i - 70) / 2] = readUnsignedIntFromEEPROM(i);

    Serial.print("RAW_TCS_L_R_MIN[");
    Serial.print((i - 70) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_L_R_MIN[(i - 70) / 2]);
  }

  Serial.println("");

  for (int i = 80; i < 90; i += 2) {
    RAW_TCS_R_R_MAX[(i - 80) / 2] = readUnsignedIntFromEEPROM(i);

    Serial.print("RAW_TCS_R_R_MAX[");
    Serial.print((i - 80) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_R_MAX[(i - 80) / 2]);
  }

  Serial.println("");

  for (int i = 90; i < 100; i += 2) {
    RAW_TCS_R_R_MIN[(i - 90) / 2] = readUnsignedIntFromEEPROM(i);

    Serial.print("RAW_TCS_R_R_MIN[");
    Serial.print((i - 90) / 2);
    Serial.print("]: ");
    Serial.println(RAW_TCS_R_R_MIN[(i - 90) / 2]);
  }

  Serial.println("");

  Serial.println("All load.");

  if (EEPROMValid == false) {
    Serial.println("!EEPROM is not valid! Please recalibrate the robot!");
  }
}

void writeUnsignedIntIntoEEPROM(int address, unsigned int number) { //write unsigned int to the EEPROM an uses 2 Bytes
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

unsigned int readUnsignedIntFromEEPROM(int address) { //read unsigned int from the EEPROM
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void writeIntIntoEEPROM(int address, int number) { //write int to the EEPROM an uses 2 Bytes
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int readIntFromEEPROM(int address) { //read int from the EEPROM
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

void PrintDataSerial(bool mode) {  //Mode: 0 - Terminal, 1 - Plotter //Outputs the data Serial via the USB port

  if (mode == 0) {
/*
    for (int i = 0; i < 8; i++) {
      Serial.print("CAL_L[");
      Serial.print(i);
      Serial.print("]: ");
      Serial.println(CAL_L[i]);
    }
*/


    if (TCSL_Green()) {
      Serial.println("Links Green");
    }

    if (TCSR_Green()) {
      Serial.println("Rechts Green");
    }

    if (TCSL_Red()){
      Serial.println("Links Red");
    }

    if (TCSR_Red()){
      Serial.println("Rechts Red");
    }

    if (TCSL_Green() && TCSR_Green()) {
      Serial.println("Both");
    }


    Serial.print("RAW_TCS_L[R]:");
    Serial.println(RAW_TCS_L[R]);
    Serial.print("RAW_TCS_L[G]:");
    Serial.println(RAW_TCS_L[G]);
    Serial.print("RAW_TCS_L[B]:");
    Serial.println(RAW_TCS_L[B]);
    Serial.println("");

    Serial.print("RAW_TCS_R[R]:");
    Serial.println(RAW_TCS_R[R]);
    Serial.print("RAW_TCS_R[G]:");
    Serial.println(RAW_TCS_R[G]);
    Serial.print("RAW_TCS_R[B]:");
    Serial.println(RAW_TCS_R[B]);
    Serial.println("");
    Serial.println("");

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

void SetRGB(unsigned int select, int color){ //dir: 0 - left, 1 - right, 2 - both
  if(select == 0){
    if(color == 0){ //red
      ShiftRegisterWrite(SR_LED_L_RED, LOW);
    }else if(color == 1){ //green
      ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
    }else{ //blue
      ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
    }
  }else if (select == 1){
    if(color == 0){ //red
      ShiftRegisterWrite(SR_LED_R_RED, LOW);
    }else if(color == 1){ //green
      ShiftRegisterWrite(SR_LED_R_GREEN, LOW);
    }else{ //blue
      ShiftRegisterWrite(SR_LED_R_BLUE, LOW);
    }
  }else{
    if(color == 0){ //red
      ShiftRegisterWrite(SR_LED_L_RED, LOW);
      ShiftRegisterWrite(SR_LED_R_RED, LOW);
    }else if(color == 1){ //green
      ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
      ShiftRegisterWrite(SR_LED_R_GREEN, LOW);
    }else{ //blue
      ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
      ShiftRegisterWrite(SR_LED_R_BLUE, LOW);
    }
  }
}

void RGBOff(){
  ShiftRegisterWrite(SR_LED_L_RED, HIGH);
  ShiftRegisterWrite(SR_LED_R_RED, HIGH);

  ShiftRegisterWrite(SR_LED_L_GREEN, HIGH);
  ShiftRegisterWrite(SR_LED_R_GREEN, HIGH);

  ShiftRegisterWrite(SR_LED_L_BLUE, HIGH);
  ShiftRegisterWrite(SR_LED_R_BLUE, HIGH);
}

void CalculateCoreTime(bool a) {
  if (DEBUGMODE == 1) {
  if (!a){
    Core0TimeTmp = millis();
  }else{
    Core0Time = millis() - Core0TimeTmp;
  }
  }
}