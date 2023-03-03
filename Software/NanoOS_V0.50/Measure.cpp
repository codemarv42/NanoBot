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

Adafruit_TCS34725 TCS_L = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);
Adafruit_TCS34725 TCS_R = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X);

/////////////////////////////////////////////

/////////////////Variables///////////////////

//MPU6050//

float XAngle;
float YAngle;
float ZAngle;
float CorrectionValueZAngle = 0.00;

//Encoder//

int ENCCounter = 0;
int currentStateENC_A;
int lastStateENC_A;
bool ENCButtonState = 0;  //0 - unpressed, 1 - pressed

//Light measure//

unsigned int RAW_L[5] = { L_A, L_I, M, R_I, R_A };  //Raw light data 0 - 4096
unsigned int RAW_MIN[5];                            //Raw data 0 - 4096
unsigned int RAW_MAX[5];                            //Raw data 0 - 4096
unsigned int CAL_L[5];                              //Raw data 0 - 100

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
unsigned int RAW_TCS_L[5] = { R, G, B, C, L }; //Raw RGB values
unsigned int RAW_TCS_R[5] = { R, G, B, C, L };
unsigned int RAW_TCS_L_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MAX[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MIN[5] = { R, G, B, C, L };  //0 - 1024 COLOR, LUX 534


/////////////////////////////////////////////

void MPU6060Init() { //Initalizes the Gyro Sensor (MPU6050)
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) {}

  Serial.println(F("While calculating the offsets value, do not move the MPU6050 sensor!"));
  delay(500);
  mpu.calcOffsets();
  Serial.println("Done!\n");
}

void UpdateMPU6050() { //Reads out the values of the gyro sensor via I2C
  mpu.update();

  XAngle = mpu.getAngleX();
  YAngle = mpu.getAngleY();
  ZAngle = mpu.getAngleZ();
}

void ResetZAngle() { //The Z axis is reset
  CorrectionValueZAngle = ZAngle;
}

int ADCRead(int pin) { //Reads out the inputs of the analog multiplexer
  ADCMULTI.channel(pin);
  delayMicroseconds(1);
  return analogRead(ADC_MULTI);
}

void MeasureRawLight() {
  
  int RAW_L_TMP[5];

  for (int i = 0; i < 5; i++) {
    RAW_L_TMP[i] = ADCRead(i);
  }

  for (int i = 0; i < 5; i++) {
    RAW_L[i] = (ADCRead(i) + RAW_L_TMP[i]) / 2;
  }
}

void Calibrate(int anz) { //The values of the phototransistors are calibrated
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  ShiftRegisterWrite(SR_LED_RGB, HIGH);

  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, LOW);

  for (int i = 0; i < 5; i++) {
    RAW_MAX[i] = 0;
    RAW_MIN[i] = 4096;
  }
  Forward(OUT_AB, 50); //Drive over the black line

  for (int n = 0; n < (anz / 2); n++) {
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

  for (int n = 0; n < (anz / 2); n++) {
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
  ShiftRegisterWrite(SR_LED_RGB, LOW);

  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, HIGH);
}

void MeasureLight() { //Read the raw values of the phototransistors
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
    CAL_L[i] = (CAL_TMP[i] + CAL_L[i]) / 2;
  }
}

float BATVoltage() { //The battery voltage is read out
  ADCMULTI.channel(ADC_VBAT);
  int a = analogRead(ADC_MULTI);
  return (a - 0) * (4.20 - 0.00) / (4096 - 0) + 0.00;
}

void ReadEncoder() { //The encoder is read out

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

void VL53l0xInit(int Port) { //The TOF sensors are initalised (TOF - Time of flight)
  TCASelect(Port);

  if (Port == 2) {  //TOF_L
    TOF_L.setTimeout(500);
    if (!TOF_L.init()) {
      Serial.println("Failed to detect and initialize TOF_L");
    } else {
      Serial.println("Found TOF_L");
      TOF_L.startContinuous(50);  //100ms measurement gap
      TOF_L_Available = 1;
    }

  } else if (Port == 3) {  //TOF_M

    TOF_M.setTimeout(500);
    if (!TOF_M.init()) {
      Serial.println("Failed to detect and initialize TOF_M");
    } else {
      Serial.println("Found TOF_M");
      TOF_M.startContinuous(50);  //100ms measurement gap
      TOF_M_Available = 1;
    }

  } else if (Port == 4) {  //TOF_R

    TOF_R.setTimeout(500);
    if (!TOF_R.init()) {
      Serial.println("Failed to detect and initialize TOF_R");
    } else {
      Serial.println("Found TOF_R");
      TOF_R.startContinuous(50);  //100ms measurement gap
      TOF_R_Available = 1;
    }
  }
}

void UpdateVL53l0x(bool EN) {  //EN: 0 - Only TOF_M, 1 - ALL //The TOF sensors are read out via I2C
  if (EN) {

    if (TOF_L_Available == 1) {
      TCASelect(2);
      if (TOF_L.timeoutOccurred()) {
        TOF_L_Available = 0;
      }
      DistL = TOF_L.readRangeContinuousMillimeters();
    }

    if (TOF_M_Available == 1) {
      TCASelect(3);
      if (TOF_M.timeoutOccurred()) {
        TOF_M_Available = 0;
      }
      DistM = TOF_M.readRangeContinuousMillimeters();
    }

    if (TOF_R_Available == 1) {
      TCASelect(4);
      if (TOF_R.timeoutOccurred()) {
        TOF_R_Available = 0;
      }
      DistR = TOF_R.readRangeContinuousMillimeters();
    }

  } else {
    if (TOF_M_Available == 1) {
      TCASelect(3);
      if (TOF_M.timeoutOccurred()) {
        TOF_M_Available = 0;
      }
      DistM = TOF_M.readRangeContinuousMillimeters();
    }
  }
}

void TCS34725Init() { //The RGB sensors are initalized
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

void UpdateTCS34725() { //The RGB sensors are read out via I2C
  if (TCS_L_Available == 1) {
    TCASelect(0);
    uint16_t r, g, b, c;
    TCS_L.getRawData(&r, &g, &b, &c);
    unsigned int lux = TCS_L.calculateLux(r, g, b);

    RAW_TCS_L[R] = r;
    RAW_TCS_L[G] = g;
    RAW_TCS_L[B] = b;
    RAW_TCS_L[C] = c;
    RAW_TCS_L[L] = lux;
  }

  if (TCS_R_Available == 1) {
    TCASelect(1);
    uint16_t r, g, b, c;
    TCS_R.getRawData(&r, &g, &b, &c);
    unsigned int lux = TCS_R.calculateLux(r, g, b);

    RAW_TCS_R[R] = r;
    RAW_TCS_R[G] = g;
    RAW_TCS_R[B] = b;
    RAW_TCS_R[C] = c;
    RAW_TCS_R[L] = lux;
  }
}

/*
unsigned int RAW_TCS_L_G_MAX[5] = {R, G, B, C, L}; //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_L_G_MIN[5] = {R, G, B, C, L}; //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MAX[5] = {R, G, B, C, L}; //0 - 1024 COLOR, LUX 534
unsigned int RAW_TCS_R_G_MIN[5] = {R, G, B, C, L}; //0 - 1024 COLOR, LUX 534
*/

void CalibrateTCS34725(int anz, int abweichung) { //The RGB sensors are calibrated with green
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

  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, LOW);
  digitalWrite(STAT_LED_BLUE, HIGH);

  delay(2000);
  PlayTone(0);

  ShiftRegisterWrite(SR_LED_PT, HIGH);

  delay(100);

  digitalWrite(STAT_LED_RED, LOW);
  digitalWrite(STAT_LED_GREEN, LOW);
  digitalWrite(STAT_LED_BLUE, HIGH);

  //Calibrate left RGB

  for (int n = 0; n < anz/2; n++) {
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

  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, LOW);
  digitalWrite(STAT_LED_BLUE, LOW);

  for (int n = 0; n < anz/2; n++) {
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

  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, HIGH);
}

bool TCSL_Green() {                                                                    //True - green, False - No green //Looks if it is the calibrated color
  if ((RAW_TCS_L[0] <= RAW_TCS_L_G_MAX[0]) && (RAW_TCS_L[0] >= RAW_TCS_L_G_MIN[0])) {  //Yes, that is extreamly ugly
    if ((RAW_TCS_L[1] <= RAW_TCS_L_G_MAX[1]) && (RAW_TCS_L[1] >= RAW_TCS_L_G_MIN[1])) {
      if ((RAW_TCS_L[2] <= RAW_TCS_L_G_MAX[2]) && (RAW_TCS_L[2] >= RAW_TCS_L_G_MIN[2])) {
        if ((RAW_TCS_L[3] <= RAW_TCS_L_G_MAX[3]) && (RAW_TCS_L[3] >= RAW_TCS_L_G_MIN[3])) {
          if ((RAW_TCS_L[4] <= RAW_TCS_L_G_MAX[4]) && (RAW_TCS_L[4] >= RAW_TCS_L_G_MIN[4])) {
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}

bool TCSR_Green() {                                                                    //True - green, False - No green //Looks if it is the calibrated color
  if ((RAW_TCS_R[0] <= RAW_TCS_R_G_MAX[0]) && (RAW_TCS_R[0] >= RAW_TCS_R_G_MIN[0])) {  //Yes, that is extreamly ugly
    if ((RAW_TCS_R[1] <= RAW_TCS_R_G_MAX[1]) && (RAW_TCS_R[1] >= RAW_TCS_R_G_MIN[1])) {
      if ((RAW_TCS_R[2] <= RAW_TCS_R_G_MAX[2]) && (RAW_TCS_R[2] >= RAW_TCS_R_G_MIN[2])) {
        if ((RAW_TCS_R[3] <= RAW_TCS_R_G_MAX[3]) && (RAW_TCS_R[3] >= RAW_TCS_R_G_MIN[3])) {
          if ((RAW_TCS_R[4] <= RAW_TCS_R_G_MAX[4]) && (RAW_TCS_R[4] >= RAW_TCS_R_G_MIN[4])) {
            return true;
          } else {
            return false;
          }
        } else {
          return false;
        }
      } else {
        return false;
      }
    } else {
      return false;
    }
  } else {
    return false;
  }
}