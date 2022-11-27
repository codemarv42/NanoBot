///////////////////Includes//////////////////

#include <Arduino.h>

#include <Wire.h>
#include <MPU6050_light.h>
#include <CD74HC4067.h>
#include <VL53L0X.h>
#include <Adafruit_TCS34725.h>

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

MPU6050 mpu(Wire);

CD74HC4067 ADCMULTI(S0, S1, S2, S3);

VL53L0X TOF_L;
VL53L0X TOF_R;
VL53L0X TOF_M;

Adafruit_TCS34725 TCS_L = Adafruit_TCS34725(-100, TCS34725_GAIN_1X);
Adafruit_TCS34725 TCS_R = Adafruit_TCS34725(-100, TCS34725_GAIN_1X);

/////////////////////////////////////////////

/////////////////Variables///////////////////

float XAngle;
float YAngle;
float ZAngle;
float CorrectionValueZAngle = 0.00;

//Encoder//

int ENCCounter = 0;
int currentStateENC_A;
int lastStateENC_A;
bool ENCButtonState = 0; //0 - unpressed, 1 - pressed

//Light measure//

unsigned int RAW_L[5] = {L_A, L_I, M, R_I, R_A}; //Raw light data 0 - 4096
unsigned int RAW_MIN[5]; //Raw data 0 - 4096
unsigned int RAW_MAX[5]; //Raw data 0 - 4096
unsigned int CAL_L[5]; //Raw data 0 - 100

//TOF Sensor//

unsigned int DistL = 0;
unsigned int DistM = 0;
unsigned int DistR = 0;
bool TOF_L_Available = 0;
bool TOF_R_Available = 0;
bool TOF_M_Available = 0;

//RGB Sensor//

bool TCS_L_Available = 0;
bool TCS_R_Available = 0;
unsigned int RAW_TCS_L[4] = {R, G, B, C};
unsigned int RAW_TCS_R[4] = {R, G, B, C};

/////////////////////////////////////////////

void MPU6060Init(){
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { }

  Serial.println(F("While calculating the offsets value, do not move the MPU6050 sensor!"));
  delay(500);
  mpu.calcOffsets();
  Serial.println("Done!\n");
}

void UpdateMPU6050(){
  mpu.update();

  XAngle = mpu.getAngleX();
  YAngle = mpu.getAngleY();
  ZAngle = mpu.getAngleZ();
  ZAngle -= CorrectionValueZAngle;
}

void ResetZAngle(){
  UpdateMPU6050();
  CorrectionValueZAngle = mpu.getAngleZ();
}

int ADCRead(int pin){ 
  ADCMULTI.channel(pin);
  return analogRead(ADC_MULTI);
}

void MeasureRawLight() {
  //ShiftRegisterWrite(SR_LED_PT, HIGH);

  for (int i = 0; i < 5; i++) {
    RAW_L[i] = ADCRead(i);
  }

  //ShiftRegisterWrite(SR_LED_PT, LOW);
}

void Calibrate(int anz) {
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  for (int i = 0; i < 5; i++) {
    RAW_MAX[i] = 0;
    RAW_MIN[i] = 4096;
  }
  Forward(OUT_AB, 50);

  for (int n = 0; n < (anz/2); n++) {
    MeasureRawLight();

    for (int i = 0; i < 5; i++) {
      if (RAW_MAX[i] < RAW_L[i]) {
        RAW_MAX[i] = RAW_L[i];
      }
      if (RAW_MIN[i] > RAW_L[i]) {
        RAW_MIN[i] = RAW_L[i];
      }
    }
    delay(1);
  }

  Backward(OUT_AB, 50);
  
  for (int n = 0; n < (anz/2); n++) {
    MeasureRawLight();

    for (int i = 0; i < 5; i++) {
      if (RAW_MAX[i] < RAW_L[i]) {
        RAW_MAX[i] = RAW_L[i];
      }
      if (RAW_MIN[i] > RAW_L[i]) {
        RAW_MIN[i] = RAW_L[i];
      }
    }
    delay(1);
  }

  Stop();

  EepromSave();

  ShiftRegisterWrite(SR_LED_PT, LOW);

}

void MeasureLight(){
  unsigned int CAL_TMP[5];
  MeasureRawLight();
  for (int i = 0; i < 5; i++) {
    CAL_TMP[i] = map(RAW_L[i], RAW_MIN[i], RAW_MAX[i], 0, 1000);
  }
  
  MeasureRawLight();
  for (int i = 0; i < 5; i++) {
    CAL_L[i] = map(RAW_L[i], RAW_MIN[i], RAW_MAX[i], 0, 1000);
  }

  for (int i = 0; i < 5; i++) {
    CAL_L[i] = (CAL_TMP[i]+CAL_L[i])/2;
  }
}

float BATVoltage(){
  ADCMULTI.channel(ADC_VBAT);
  int a = analogRead(ADC_MULTI);
  return (a - 0) * (4.20 - 0.00) / (4096 - 0) + 0.00;
}

void ReadEncoder(){
  
  currentStateENC_A = digitalRead(ENC_A);

  if (currentStateENC_A != lastStateENC_A  && currentStateENC_A == 1){

    if (digitalRead(ENC_B) != currentStateENC_A) {

      if (ENCCounter == 0){
        ENCCounter = 0;
      }else{
        ENCCounter --;
      }
      
    } else {
      ENCCounter ++;
    }
  }

  lastStateENC_A = currentStateENC_A;

  bool a = digitalRead(ENC_SW);
  
  if ((a == 1) && (ENCButtonState != 1)){
    ENCButtonState = 1;
  }
}

void VL53l0xInit(int Port) {
  TCASelect(Port);

  if (Port == 2) {  //TOF_L
    TOF_L.setTimeout(500);
    if (!TOF_L.init()) {
      Serial.println("Failed to detect and initialize TOF_L");
    } else {
      TOF_L.startContinuous(50);  //100ms measurement gap
      TOF_L_Available = 1;
    }

  } else if (Port == 3) {  //TOF_M

    TOF_M.setTimeout(500);
    if (!TOF_M.init()) {
      Serial.println("Failed to detect and initialize TOF_M");
    } else {
      TOF_M.startContinuous(50);  //100ms measurement gap
      TOF_M_Available = 1;
    }

  } else if (Port == 4) {  //TOF_R

    TOF_R.setTimeout(500);
    if (!TOF_R.init()) {
      Serial.println("Failed to detect and initialize TOF_R");
    } else {
      TOF_R.startContinuous(50);  //100ms measurement gap
      TOF_R_Available = 1;
    }
  }
}

void UpdateVL53l0x(){

  if (TOF_L_Available == 1){
    TCASelect(2);
    if (TOF_L.timeoutOccurred()){
      TOF_L_Available = 0;
    }
    DistL = TOF_L.readRangeContinuousMillimeters();
  }

  if (TOF_M_Available == 1){
    TCASelect(3);
    if (TOF_M.timeoutOccurred()){
      TOF_M_Available = 0;
    }
    DistM = TOF_M.readRangeContinuousMillimeters();
  }

  if (TOF_R_Available == 1){
    TCASelect(4);
    if (TOF_R.timeoutOccurred()){
      TOF_R_Available = 0;
    }
    DistR = TOF_R.readRangeContinuousMillimeters();
  }

}

void TCS34725Init(){
  TCASelect(0);
  if (TCS_L.begin()) {
    Serial.println("Found TCS_L");
    TCS_L_Available = 1;
  } else {
    Serial.println("Failed to detect and initialize TCS_L");
  }
  TCASelect(1);
  if (TCS_R.begin()) {
    Serial.println("Found TCS_R");
    TCS_R_Available = 1;
  } else {
    Serial.println("Failed to detect and initialize TCS_R");
  }
}

void UpdateTCS34725(){
  if (TCS_L_Available == 1){   
    TCASelect(0);
    uint16_t r, g, b, c;
    TCS_L.getRawData(&r, &g, &b, &c);

    RAW_TCS_L[R] = r;
    RAW_TCS_L[G] = g;
    RAW_TCS_L[B] = b;
    RAW_TCS_L[C] = c;
  }

  if (TCS_R_Available == 1){
    TCASelect(1);
    uint16_t r, g, b, c;
    TCS_R.getRawData(&r, &g, &b, &c);

    RAW_TCS_R[R] = r;
    RAW_TCS_R[G] = g;
    RAW_TCS_R[B] = b;
    RAW_TCS_R[C] = c;
  }
}