///////////////////Includes//////////////////

#include <Arduino.h>

#include <Wire.h>
#include <MPU6050_light.h>
#include <CD74HC4067.h>
#include <VL53L1X.h>
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

VL53L1X TOF_L;
VL53L1X TOF_R;
VL53L1X TOF_M;

Adafruit_TCS34725 TCS_L = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X); //GAIN - light level
Adafruit_TCS34725 TCS_R = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_1X); //TCS34725_INTEGRATIONTIME_2_4MS

/////////////////////////////////////////////

/////////////////Variables///////////////////

//MPU6050//

float XAngle;
float YAngle;
float ZAngle;
float CorrectionValueZAngle = 0.00;
float XAccel;

//Encoder//

int ENCCounter = 0;
int currentStateENC_A;
int lastStateENC_A;
bool ENCButtonState = 0;  //0 - unpressed, 1 - pressed

//Light measure//

unsigned int RAW_L[8] = { L_A, L_I, M, R_I, R_A, L_A2, R_A2, REF };  //Raw light data 0 - 4096
unsigned int RAW_MIN[7];                            //Raw data 0 - 4096
unsigned int RAW_MAX[7];                            //Raw data 0 - 4096
unsigned int CAL_L[7];                              //Raw data 0 - 100
bool CalState = false;                              //true - Light sensor is calibrated before the start of main program

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
bool TCS_Calibrate_EN = false;
float RAW_TCS_L[3] = { R, G, B};  //RGB values
float RAW_TCS_R[3] = { R, G, B};

//Calibrated green values
unsigned int RAW_TCS_L_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534

//Calibrated red values
unsigned int RAW_TCS_L_R_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_R_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_R_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_R_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534


/////////////////////////////////////////////

void MPU6050Init() {  //Initalizes the Gyro Sensor (MPU6050)
  byte status = mpu.begin();
  if(status != 0){
    Serial.println(F("Fail to detect MPU6050!"));
  }else{
    Serial.println(F("Found MPU6050"));
    Serial.println(F("While calculating the offsets value, do not move the MPU6050 sensor!"));
    delay(500);
    mpu.calcOffsets();
    Serial.println("Done!\n");
  }
}

void UpdateMPU6050() {  //Reads out the values of the gyro sensor via I2C
  mpu.update();

  XAngle = mpu.getAngleX();
  YAngle = mpu.getAngleY();
  ZAngle = mpu.getAngleZ();
  XAccel = mpu.getAccAngleX();
}

void ResetZAngle() {  //The Z axis is reset
  CorrectionValueZAngle = ZAngle;
}

int ADCRead(int pin) {  //Reads out the inputs of the analog multiplexer
  ADCMULTI.channel(pin);
  delayMicroseconds(1);
  return analogRead(ADC_MULTI);
  
}

void MeasureRawLight() {  //Measure the raw light values from the ADC multiplexer

  int RAW_L_TMP[8];

  for (int i = 0; i < 8; i++) {
    RAW_L_TMP[i] = ADCRead(i);
  }

  for (int i = 0; i < 8; i++) {
    RAW_L[i] = (ADCRead(i) + RAW_L_TMP[i]) / 2;
  }
}

void Calibrate(int anz) {  //The values of the phototransistors are calibrated
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  ShiftRegisterWrite(SR_LED_RGB, HIGH);

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);

  ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
  ShiftRegisterWrite(SR_LED_R_GREEN, LOW);

  ShiftRegisterWrite(SR_LED_L_BLUE, HIGH);
  ShiftRegisterWrite(SR_LED_R_BLUE, HIGH);

  for (int i = 0; i < 7; i++) {
    RAW_MAX[i] = 0;
    RAW_MIN[i] = 4096;
  }
  Forward(OUT_AB, 50);  //Drive over the black line

  for (int n = 0; n < (anz / 2); n++) {
    MeasureRawLight();

    for (int i = 0; i < 7; i++) {
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

  for (int n = 0; n < (anz / 2); n++) {
    MeasureRawLight();

    for (int i = 0; i < 7; i++) {
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

  CalState = true;

  ShiftRegisterWrite(SR_LED_PT, LOW);
  ShiftRegisterWrite(SR_LED_RGB, LOW);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);
}

void MeasureLight() {  //Read the raw values of the phototransistors
  MeasureRawLight();
  for (int i = 0; i < 7; i++) {
    CAL_L[i] = map(RAW_L[i], RAW_MIN[i], RAW_MAX[i], 0, 1000);
    if (CAL_L[i] > 1000){
      CAL_L[i] = 1000;
    }
  }
}

float BATVoltage() {  //The battery voltage is read out
  int a = analogRead(VBAT);
  return (a - 0) * (4.20 - 0.00) / (4096 - 0) + 0.00;
}

void ReadEncoder() {  //The encoder is read out

  currentStateENC_A = digitalRead(ENC_A);

  if (currentStateENC_A != lastStateENC_A && currentStateENC_A == 1) {

    if (digitalRead(ENC_B) != currentStateENC_A) {

      if (ENCCounter == 0) {
        ENCCounter = 0;
      } else {
        ENCCounter--;
      }

    } else {
      ENCCounter++;
    }
  }

  lastStateENC_A = currentStateENC_A;

  bool a = digitalRead(ENC_SW);

  if ((a == 1) && (ENCButtonState != 1)) {
    ENCButtonState = 1;
  }
}

void VL53l1xInit(int Port, unsigned int mode) {  //The TOF sensors are initalised (TOF - Time of flight) //Distance sensors mode: 0 - Short, 1 - Medium, 2 - Long
  TCASelect(Port);

  if (Port == 2) {  //TOF_L
    TOF_L.setTimeout(300);
    if(millis() < 6000){
      if (!TOF_L.init()) {
      Serial.println("Failed to detect and initialize TOF_L");
    } else {
      Serial.println("Found TOF_L");
      TOF_L.setDistanceMode(VL53L1X::Long);
      TOF_L.setMeasurementTimingBudget(50000);
      TOF_L.startContinuous(50);
      TOF_L_Available = 1;
    }
    }else{
      if (mode == 0){
        TOF_L.setDistanceMode(VL53L1X::Short);
        TOF_L.setMeasurementTimingBudget(20000);
        TOF_L.startContinuous(20);
      }else if (mode == 1){
        TOF_L.setDistanceMode(VL53L1X::Medium);
        TOF_L.setMeasurementTimingBudget(35000);
        TOF_L.startContinuous(35);
      }else{
        TOF_L.setDistanceMode(VL53L1X::Long);
        TOF_L.setMeasurementTimingBudget(50000);
        TOF_L.startContinuous(50);
      }
    }
  } else if (Port == 3) {  //TOF_M

    TOF_M.setTimeout(300);
    if(millis() < 6000){
      if (!TOF_M.init()) {
      Serial.println("Failed to detect and initialize TOF_M");
    } else {
      Serial.println("Found TOF_M");
      TOF_M.setDistanceMode(VL53L1X::Long);
      TOF_M.setMeasurementTimingBudget(50000);
      TOF_M.startContinuous(50);
      TOF_M_Available = 1;
    }
    }else{
      if (mode == 0){
        TOF_M.setDistanceMode(VL53L1X::Short);
        TOF_M.setMeasurementTimingBudget(15000);
        TOF_M.startContinuous(15);
      }else if (mode == 1){
        TOF_M.setDistanceMode(VL53L1X::Medium);
        TOF_M.setMeasurementTimingBudget(35000);
        TOF_M.startContinuous(35);
      }else{
        TOF_M.setDistanceMode(VL53L1X::Long);
        TOF_M.setMeasurementTimingBudget(50000);
        TOF_M.startContinuous(50);
      }
    }

  } else if (Port == 4) {  //TOF_R

    TOF_R.setTimeout(300);
    if(millis() < 6000){
      if (!TOF_R.init()) {
      Serial.println("Failed to detect and initialize TOF_R");
    } else {
      Serial.println("Found TOF_R");
      TOF_R.setDistanceMode(VL53L1X::Long);
      TOF_R.setMeasurementTimingBudget(50000);
      TOF_R.startContinuous(50);
      TOF_R_Available = 1;
    }
    }else{
      if (mode == 0){
        TOF_R.setDistanceMode(VL53L1X::Short);
        TOF_R.setMeasurementTimingBudget(20000);
        TOF_R.startContinuous(20);
      }else if (mode == 1){
        TOF_R.setDistanceMode(VL53L1X::Medium);
        TOF_R.setMeasurementTimingBudget(35000);
        TOF_R.startContinuous(35);
      }else{
        TOF_R.setDistanceMode(VL53L1X::Long);
        TOF_R.setMeasurementTimingBudget(50000);
        TOF_R.startContinuous(50);
      }
    }

  }
}

void UpdateVL53l1x(bool EN) {  //EN: 0 - Only TOF_M, 1 - ALL //The TOF sensors are read out via I2C
  if (EN) {

    if (TOF_L_Available == 1) {
      TCASelect(2);
      DistL = TOF_L.read();
    }

    if (TOF_M_Available == 1) {
      TCASelect(3);
      DistM = TOF_M.read();
    }

    if (TOF_R_Available == 1) {
      TCASelect(4);
      DistR = TOF_R.read();
    }

  } else {
    if (TOF_M_Available == 1) {
      TCASelect(3);
      DistM = TOF_M.read();
    }
  }
}

void TCS34725Init() {  //The RGB sensors are initalized
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

void UpdateTCS34725() {  //The RGB sensors are read out via I2C

  if (TCS_L_Available == 1) {
    float r, g, b, avg;
    TCASelect(0);
    TCS_L.getRGB(&r, &g, &b);

    avg = (r + g + b) / 3;

    RAW_TCS_L[R] = r / avg;
    RAW_TCS_L[G] = g / avg;
    RAW_TCS_L[B] = b / avg;
  }

  if (TCS_R_Available == 1) {
    float r, g, b, avg;
    TCASelect(1);
    TCS_R.getRGB(&r, &g, &b);

    avg = (r + g + b) / 3;

    RAW_TCS_R[R] = r / avg;
    RAW_TCS_R[G] = g / avg;
    RAW_TCS_R[B] = b / avg;
  }
}

void CalibrateTCS34725Green(int anz, int abweichung) {  //The RGB sensors are calibrated with green
  ShiftRegisterWrite(SR_LED_PT, LOW);
  ShiftRegisterWrite(SR_LED_RGB, HIGH);

  for (int i = 0; i < 4; i++) {
    RAW_TCS_L_G_MAX[i] = 0;
    RAW_TCS_R_G_MAX[i] = 0;
    RAW_TCS_L_G_MIN[i] = 1024;
    RAW_TCS_R_G_MIN[i] = 1024;
  }

  RAW_TCS_L_G_MAX[L] = 0;
  RAW_TCS_R_G_MAX[L] = 0;
  RAW_TCS_L_G_MIN[L] = 534;
  RAW_TCS_R_G_MIN[L] = 534;

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);

  ShiftRegisterWrite(SR_LED_L_GREEN, HIGH);
  ShiftRegisterWrite(SR_LED_R_GREEN, HIGH);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);

  delay(2000);
  PlayTone(0);

  ShiftRegisterWrite(SR_LED_PT, HIGH);

  delay(100);

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);

  ShiftRegisterWrite(SR_LED_L_GREEN, HIGH);
  ShiftRegisterWrite(SR_LED_R_GREEN, LOW);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);

  //Calibrate left RGB

  for (int n = 0; n < anz / 2; n++) {
    UpdateTCS34725();

    for (int i = 0; i < 5; i++) {
      if (RAW_TCS_L_G_MAX[i] < RAW_TCS_L[i]) {
        RAW_TCS_L_G_MAX[i] = RAW_TCS_L[i] + abweichung;
      }
      if (RAW_TCS_L_G_MIN[i] > RAW_TCS_L[i]) {
        RAW_TCS_L_G_MIN[i] = RAW_TCS_L[i] - abweichung;
      }
    }

    delay(3);
  }

  //Calibrate right RGB

  PlayTone(0);
  ShiftRegisterWrite(SR_LED_PT, LOW);
  delay(2000);
  PlayTone(0);

  ShiftRegisterWrite(SR_LED_PT, HIGH);

  delay(100);

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);

  ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
  ShiftRegisterWrite(SR_LED_R_GREEN, HIGH);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);

  for (int n = 0; n < anz / 2; n++) {
    UpdateTCS34725();

    for (int i = 0; i < 5; i++) {
      if (RAW_TCS_R_G_MAX[i] < RAW_TCS_R[i]) {
        RAW_TCS_R_G_MAX[i] = RAW_TCS_R[i] + abweichung;
      }
      if (RAW_TCS_R_G_MIN[i] > RAW_TCS_R[i]) {
        RAW_TCS_R_G_MIN[i] = RAW_TCS_R[i] - abweichung;
      }
    }

    delay(3);
  }


  EepromSave();

  ShiftRegisterWrite(SR_LED_PT, LOW);
  ShiftRegisterWrite(SR_LED_RGB, LOW);

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);

  ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
  ShiftRegisterWrite(SR_LED_R_GREEN, LOW);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);
}

void CalibrateTCS34725Red(int anz, int abweichung) {  //The RGB sensors are calibrated with red
  ShiftRegisterWrite(SR_LED_RGB, HIGH);

  for (int i = 0; i < 4; i++) {
    RAW_TCS_L_R_MAX[i] = 0;
    RAW_TCS_R_R_MAX[i] = 0;
    RAW_TCS_L_R_MIN[i] = 1024;
    RAW_TCS_R_R_MIN[i] = 1024;
  }

  RAW_TCS_L_R_MAX[L] = 0;
  RAW_TCS_R_R_MAX[L] = 0;
  RAW_TCS_L_R_MIN[L] = 534;
  RAW_TCS_R_R_MIN[L] = 534;

  ShiftRegisterWrite(SR_LED_L_RED, HIGH);
  ShiftRegisterWrite(SR_LED_R_RED, HIGH);

  ShiftRegisterWrite(SR_LED_L_GREEN, LOW);
  ShiftRegisterWrite(SR_LED_R_GREEN, LOW);

  ShiftRegisterWrite(SR_LED_L_BLUE, LOW);
  ShiftRegisterWrite(SR_LED_R_BLUE, LOW);

  delay(2000);
  PlayTone(0);

  ShiftRegisterWrite(SR_LED_PT, HIGH);

  for (int n = 0; n < anz; n++) {
    UpdateTCS34725();

    for (int i = 0; i < 5; i++) {
      if (RAW_TCS_L_R_MAX[i] < RAW_TCS_L[i]) {
        RAW_TCS_L_R_MAX[i] = RAW_TCS_L[i] + abweichung;
      }
      if (RAW_TCS_L_R_MIN[i] > RAW_TCS_L[i]) {
        RAW_TCS_L_R_MIN[i] = RAW_TCS_L[i] - abweichung;
      }
    }

    for (int i = 0; i < 5; i++) {
      if (RAW_TCS_R_R_MAX[i] < RAW_TCS_R[i]) {
        RAW_TCS_R_R_MAX[i] = RAW_TCS_R[i] + abweichung;
      }
      if (RAW_TCS_R_R_MIN[i] > RAW_TCS_R[i]) {
        RAW_TCS_R_R_MIN[i] = RAW_TCS_R[i] - abweichung;
      }
    }

    delay(3);
  }

  EepromSave();

  ShiftRegisterWrite(SR_LED_PT, LOW);
  ShiftRegisterWrite(SR_LED_RGB, LOW);

  ShiftRegisterWrite(SR_LED_L_RED, LOW);
  ShiftRegisterWrite(SR_LED_R_RED, LOW);
}

bool TCSL_Green() {    

   if ((RAW_TCS_L[R] < 0.95) && (RAW_TCS_L[G] < 1.25) && (RAW_TCS_L[B] < 1.20) && (RAW_TCS_L[R] > 0.70) && (RAW_TCS_L[G] > 1.05) && (RAW_TCS_L[B] > 0.95)){
     return true;
   }else{
     return false;
   }
}

bool TCSR_Green() {      

   if ((RAW_TCS_R[R] < 0.95) && (RAW_TCS_R[G] < 1.25) && (RAW_TCS_R[B] < 1.20) && (RAW_TCS_R[R] > 0.70) && (RAW_TCS_R[G] > 1.05) && (RAW_TCS_R[B] > 0.95)){
     return true;
   }else{
     return false;
   }
}

bool TCSL_Red() {     
  
  if ((RAW_TCS_L[R] < 1.55) && (RAW_TCS_L[G] < 0.95) && (RAW_TCS_L[B] < 1.15) && (RAW_TCS_L[R] > 0.95) && (RAW_TCS_L[G] > 0.60) && (RAW_TCS_L[B] > 0.65)) {
    return true;
  }else if ((RAW_TCS_L[R] < 1.40) && (RAW_TCS_L[G] < 0.90) && (RAW_TCS_L[B] < 0.95) && (RAW_TCS_L[R] > 1.20) && (RAW_TCS_L[G] > 0.70) && (RAW_TCS_L[B] > 0.75)){
    return true;
  }else{
    return false;
  }
}

bool TCSR_Red() {    
  
  if ((RAW_TCS_R[R] < 1.55) && (RAW_TCS_R[G] < 0.90) && (RAW_TCS_R[B] < 1.00) && (RAW_TCS_R[R] > 1.35) && (RAW_TCS_R[G] > 0.60) && (RAW_TCS_R[B] > 0.65)) {
    return true;
  }else if ((RAW_TCS_R[R] < 1.40) && (RAW_TCS_R[G] < 0.90) && (RAW_TCS_R[B] < 0.95) && (RAW_TCS_R[R] > 1.20) && (RAW_TCS_R[G] > 0.70) && (RAW_TCS_R[B] > 0.75)){
    return true;
  }else{
    return false;
  }
}

/*

//Calibrated green values
unsigned int RAW_TCS_L_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534

//Calibrated red values
unsigned int RAW_TCS_L_R_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_R_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_R_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_R_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534

*/

void SetTCS_Green(bool DATASET) {  //DATASET: A/B, 0/1 //The calibrated green values are set manually
  if (DATASET == 0) {              //DATASET: A

    RAW_TCS_L_G_MAX[R] = 11;
    RAW_TCS_L_G_MAX[G] = 17;
    RAW_TCS_L_G_MAX[B] = 23;
    RAW_TCS_L_G_MAX[C] = 36;
    RAW_TCS_L_G_MAX[L] = 12;

    RAW_TCS_L_G_MIN[R] = 0;
    RAW_TCS_L_G_MIN[G] = 3;
    RAW_TCS_L_G_MIN[B] = 7;
    RAW_TCS_L_G_MIN[C] = 12;
    RAW_TCS_L_G_MIN[L] = 1;

    RAW_TCS_R_G_MAX[R] = 6;
    RAW_TCS_R_G_MAX[G] = 14;
    RAW_TCS_R_G_MAX[B] = 25; //Hohe Streuung 11-20
    RAW_TCS_R_G_MAX[C] = 40; //Hohe Streuung 21-38
    RAW_TCS_R_G_MAX[L] = 12;

    RAW_TCS_R_G_MIN[R] = 0;
    RAW_TCS_R_G_MIN[G] = 3;
    RAW_TCS_R_G_MIN[B] = 4;
    RAW_TCS_R_G_MIN[C] = 12;
    RAW_TCS_R_G_MIN[L] = 1;

  } else {  //DATASET: B

    RAW_TCS_L_G_MAX[R] = 0;
    RAW_TCS_L_G_MAX[G] = 0;
    RAW_TCS_L_G_MAX[B] = 0;
    RAW_TCS_L_G_MAX[C] = 0;
    RAW_TCS_L_G_MAX[L] = 0;

    RAW_TCS_L_G_MIN[R] = 0;
    RAW_TCS_L_G_MIN[G] = 0;
    RAW_TCS_L_G_MIN[B] = 0;
    RAW_TCS_L_G_MIN[C] = 0;
    RAW_TCS_L_G_MIN[L] = 0;

    RAW_TCS_R_G_MAX[R] = 0;
    RAW_TCS_R_G_MAX[G] = 0;
    RAW_TCS_R_G_MAX[B] = 0;
    RAW_TCS_R_G_MAX[C] = 0;
    RAW_TCS_R_G_MAX[L] = 0;

    RAW_TCS_R_G_MIN[R] = 0;
    RAW_TCS_R_G_MIN[G] = 0;
    RAW_TCS_R_G_MIN[B] = 0;
    RAW_TCS_R_G_MIN[C] = 0;
    RAW_TCS_R_G_MIN[L] = 0;
  }
}

void SetTCS_Red(bool DATASET) {  //DATASET: A/B, 0/1 //The calibrated red values are set manually
  if (DATASET == 0) {            //DATASET: A

    RAW_TCS_L_R_MAX[R] = 7;
    RAW_TCS_L_R_MAX[G] = 4;
    RAW_TCS_L_R_MAX[B] = 11;
    RAW_TCS_L_R_MAX[C] = 20;
    RAW_TCS_L_R_MAX[L] = 65537;

    RAW_TCS_L_R_MIN[R] = 0;
    RAW_TCS_L_R_MIN[G] = 0;
    RAW_TCS_L_R_MIN[B] = 2;
    RAW_TCS_L_R_MIN[C] = 5;
    RAW_TCS_L_R_MIN[L] = 65533;

    RAW_TCS_R_R_MAX[R] = 7;
    RAW_TCS_R_R_MAX[G] = 5;
    RAW_TCS_R_R_MAX[B] = 9;
    RAW_TCS_R_R_MAX[C] = 18;
    RAW_TCS_R_R_MAX[L] = 65536;

    RAW_TCS_R_R_MIN[R] = 3;
    RAW_TCS_R_R_MIN[G] = 1;
    RAW_TCS_R_R_MIN[B] = 5;
    RAW_TCS_R_R_MIN[C] = 14;
    RAW_TCS_R_R_MIN[L] = 534;

  } else {  //DATASET: B

    RAW_TCS_L_R_MAX[R] = 0;
    RAW_TCS_L_R_MAX[G] = 0;
    RAW_TCS_L_R_MAX[B] = 0;
    RAW_TCS_L_R_MAX[C] = 0;
    RAW_TCS_L_R_MAX[L] = 0;

    RAW_TCS_L_R_MIN[R] = 0;
    RAW_TCS_L_R_MIN[G] = 0;
    RAW_TCS_L_R_MIN[B] = 0;
    RAW_TCS_L_R_MIN[C] = 0;
    RAW_TCS_L_R_MIN[L] = 0;

    RAW_TCS_R_R_MAX[R] = 0;
    RAW_TCS_R_R_MAX[G] = 0;
    RAW_TCS_R_R_MAX[B] = 0;
    RAW_TCS_R_R_MAX[C] = 0;
    RAW_TCS_R_R_MAX[L] = 0;

    RAW_TCS_R_R_MIN[R] = 0;
    RAW_TCS_R_R_MIN[G] = 0;
    RAW_TCS_R_R_MIN[B] = 0;
    RAW_TCS_R_R_MIN[C] = 0;
    RAW_TCS_R_R_MIN[L] = 0;
  }
}