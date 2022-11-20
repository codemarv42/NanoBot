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

#define EEPROM_SIZE 12

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

  if ((BATVoltage() < 3.5) && (BATVoltage() > 2)) {
    BatteryWarning();
  }

  MPUCaliPage();

  MPU6060Init();

  TCS34725Init();

  digitalWrite(STAT_LED_GREEN, LOW);

  EEPROM.begin(EEPROM_SIZE);

  EepromLoad();

  xTaskCreatePinnedToCore(
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

void Loop1Code( void * pvParameters ){
  Serial.print("Loop1Code running on core ");
  Serial.println(xPortGetCoreID());
  unsigned long timer1 = 0;

  for(;;){
    if (ENMainProgram == 0){
      Menu();
      delay(20);
    }else{
      UpdateMPU6050();
      ReadEncoder();
      UpdateVL53l0x();
      //UpdateTCS34725();

      if ((millis() - timer1) > 200) {
        DisplayTilesCase();
        timer1 = millis();
      }

    }
  }
}

/////////////////////////////////////////////

void ShiftRegisterWrite(int pin, bool state) {

  ShiftRegisterBits[pin] = state;

  digitalWrite(STCP, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, ShiftRegisterBits[i]);
    digitalWrite(SHCP, HIGH);
  }

  digitalWrite(STCP, HIGH);
}

void ShiftRegisterReset() {
  digitalWrite(STCP, LOW);
  for (int i = 7; i >= 0; i--) {
    digitalWrite(SHCP, LOW);
    digitalWrite(DS, 0);
    digitalWrite(SHCP, HIGH);
  }

  digitalWrite(STCP, HIGH);
}

void Forward(byte mot, int v) {
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

void Backward(byte mot, int v) {
  Forward(mot, -v);
}

void Stop() {
  ShiftRegisterWrite(SR_STBY, LOW);
  digitalWrite(PWMA, LOW);
  digitalWrite(PWMB, LOW);
}

void Left(int v) {
  Forward(OUT_B, v);
  Backward(OUT_A, v);
}

void Right(int v) {
  Forward(OUT_A, v);
  Backward(OUT_B, v);
}

void RotateAngle(int ang) {  //positiv - left, negativ - right

  ResetZAngle();
  if (ang < 0) {
    while (ZAngle > ang) {
      Serial.print(F("zAng: "));
      Serial.println(ZAngle);
      Right(Slow);
    }
  } else {
    while (ZAngle < ang) {
      Serial.print(F("zAng: "));
      Serial.println(ZAngle);
      Left(Slow);
    }
  }
  Stop();
}

void ServoInit() {
  ServoOne.attach(SERVO1);
  ServoTwo.attach(SERVO2);
}

void Servo1(int pos) {  //0-180 deg
  ServoOne.write(pos);
}

void Servo2(int pos) {  //0-180 deg
  ServoTwo.write(pos);
}

void FollowLine(int v) {
  int diff = (CAL_L[R_I] - CAL_L[L_I]) * FollowLineDiffFactor;
  if (abs(diff) < 10){
    diff = 0;
  }  
  int vl = v + (diff/3);
  int vr = v - (diff/3);  
  Forward(OUT_B, vl);
  Forward(OUT_A, vr);
}

void DriveFixedAngle(int time){
  ResetZAngle();  
  unsigned long temptime = millis();
  while ((temptime + time) > millis()) {
    if ((abs(ZAngle) < 1)){
      Forward(OUT_B, Slow);
      Forward(OUT_A, Slow);
    }else{
      while((abs(ZAngle) > 1)){
        if (ZAngle < 0){
          Left(Slow);
        }else{
          Right(Slow);
        }
      }
    }
  }
}

void DriveTime(int time){
  unsigned long temptime = millis();
  while ((temptime + time) > millis()) {
    Forward(OUT_AB, Slow);
  }
}

void TCASelect(uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(I2CMULTI_ADRESS);
  Wire.write(1 << i);
  Wire.endTransmission();
}

void PlayTone(int val) {  //0 - Notification, 1 - Boot, 2 - Warning

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
  }
}

void EepromSave() {

  for (int i = 0; i < 5; i++){
    EEPROM.write(i, map(RAW_MIN[i],0,4096,0,255));
  }

  for (int f = 5; f < 10; f++){
    EEPROM.write(f, map(RAW_MAX[f-5],0,4096,0,255));
  }

  EEPROM.commit(); //Saves the changes
}

void EepromLoad() { //Adress: 0-9 MIN MAX, 

  for (int i = 0; i < 5; i++){
    unsigned int a = EEPROM.read(i);  
    RAW_MIN[i] = map(a,0,255,0,4096);
  }

  for (int f = 5; f < 10; f++){
    unsigned int a = EEPROM.read(f);  
    RAW_MAX[f-5] = map(a,0,255,0,4096);
  }
}

void PrintDataSerial(){
  for (int i = 0; i < 5; i++){
    Serial.print("CAL_L[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(CAL_L[i]);
  }
}
