///////////////////Includes//////////////////

#include <Arduino.h>

#include <Wire.h>
#include <MPU6050_light.h>
#include <CD74HC4067.h>

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

MPU6050 mpu(Wire);

CD74HC4067 ADCMULTI(S0, S1, S2, S3);

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

float BATVoltage(){
  ADCMULTI.channel(ADC_VBAT);
  int a = analogRead(ADC_MULTI);
  return ((a - 0) * (4.20 - 0.00) / (4095 - 0) + 0.00) - 0.13;
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
}

