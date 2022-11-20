///////////////////Includes//////////////////

#include <Arduino.h>

#include <Wire.h>
#include <ESP32_Servo.h>

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

Servo ServoOne;
Servo ServoTwo;

TaskHandle_t Loop1;

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

  DisplayInit();

  BootPage();

  if ((BATVoltage() < 3.5) && (BATVoltage() > 2)) {
    BatteryWarning();
  }

  MPUCaliPage();

  MPU6060Init();

  digitalWrite(STAT_LED_GREEN, LOW);



  //EEPROM.begin(EEPROM_SIZE);

  //EepromLoad();

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
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for(;;){
    if (ENMainProgram == 0){
      Menu();
      delay(20);
    }else{
      UpdateMPU6050();
      ReadEncoder();
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
  int diff = (RAW_L[R_I] - RAW_L[L_I]) * FollowLineDiffFactor;
  if (abs(diff) < 3) diff = 0;
  int vl = v + diff;
  int vr = v - diff;
  Forward(OUT_B, vl);
  Forward(OUT_A, vr);
}

void DriveFixedAngle(int time){
  ResetZAngle();
  unsigned long temptime = millis();
  float diff = ZAngle;
  while ((temptime + time) > millis()) {
    if (abs(diff) < 2) diff = 0;
    int vl = Normal + diff;
    int vr = Normal - diff;
    Forward(OUT_B, vl);
    Forward(OUT_A, vr);
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





/*
void EepromSave() {
  if (EEPROM.read(LanguageAddress) != Language) {
    EEPROM.write(LanguageAddress, Language);
  }

  if (EEPROM.read(DoseUnitAddress) != DoseUnit) {
    EEPROM.write(DoseUnitAddress, DoseUnit);
  }

  if (EEPROM.read(TimeFormatAddress) != TimeFormat) {
    EEPROM.write(TimeFormatAddress, TimeFormat);
  }

  if (EEPROM.read(GeigerTubeModelAddress) != GeigerTubeModel) {
    EEPROM.write(GeigerTubeModelAddress, GeigerTubeModel);
  }

  if (EEPROM.read(MainPageNumberAddress) != MainPageNumber) {
    EEPROM.write(MainPageNumberAddress, MainPageNumber);
  }

  if (EEPROM.read(DoseAlertRateAddress) != DoseAlertRate) {
    EEPROM.write(DoseAlertRateAddress, DoseAlertRate);
  }

  if (EEPROM.read(TotalDoseAlertRateAddress) != TotalDoseAlertRate) {
    EEPROM.write(TotalDoseAlertRateAddress, TotalDoseAlertRate);
  }

  if (EEPROM.read(DisplayBrightnessAddress) != DisplayBrightness) {
    EEPROM.write(DisplayBrightnessAddress, DisplayBrightness);
  }

  if (EEPROM.read(HVSetPointAdress) != ConvertHVSetPoint(0, 0)) {
    EEPROM.write(HVSetPointAdress, ConvertHVSetPoint(0, 0));
  }

  if (EEPROM.read(CustomZeroEffectAdress) != CustomZeroEffect) {
    EEPROM.write(CustomZeroEffectAdress, CustomZeroEffect);
  }

  EEPROM.commit(); //Saves the changes
}

void EepromLoad() {
  if (EEPROM.read(LanguageAddress) > 1) {
    Language = 1;
  } else {
    Language = EEPROM.read(LanguageAddress);
  }

  if (EEPROM.read(DoseUnitAddress) > 2) {
    DoseUnit = 0;
  } else {
    DoseUnit = EEPROM.read(DoseUnitAddress);
  }

  if (EEPROM.read(TimeFormatAddress) > 1) {
    TimeFormat = 0;
  } else {
    TimeFormat = EEPROM.read(TimeFormatAddress);
  }

  if (EEPROM.read(GeigerTubeModelAddress) > 5) {
    GeigerTubeModel = 0;
  } else {
    GeigerTubeModel = EEPROM.read(GeigerTubeModelAddress);
  }

  if (EEPROM.read(MainPageNumberAddress) > 7) {
    MainPageNumber = 0;
  } else {
    MainPageNumber = EEPROM.read(MainPageNumberAddress);
  }

  if ((EEPROM.read(DoseAlertRateAddress) > 90) || (EEPROM.read(DoseAlertRateAddress) < 10)) {
    DoseAlertRate = 10;
  } else {
    DoseAlertRate = EEPROM.read(DoseAlertRateAddress);
  }

  if ((EEPROM.read(TotalDoseAlertRateAddress) > 90) || (EEPROM.read(TotalDoseAlertRateAddress) < 30)) {
    TotalDoseAlertRate = 45;
  } else {
    TotalDoseAlertRate = EEPROM.read(TotalDoseAlertRateAddress);
  }

  if ((EEPROM.read(DisplayBrightnessAddress) < 20) || (EEPROM.read(DisplayBrightnessAddress) > 255)) {
    DisplayBrightness = 255;
  } else {
    DisplayBrightness = EEPROM.read(DisplayBrightnessAddress);
  }

  if (EEPROM.read(HVSetPointAdress) > 14) {
    HVSetPoint = 300;
  } else {
    HVSetPoint = ConvertHVSetPoint(1, EEPROM.read(HVSetPointAdress));
  }

  CustomZeroEffect = EEPROM.read(CustomZeroEffectAdress);

}

void EepromFactoryReset() {
  if (EEPROM.read(LanguageAddress) != 1) {
    EEPROM.write(LanguageAddress, 1);
  }

  if (EEPROM.read(DoseUnitAddress) != 0) {
    EEPROM.write(DoseUnitAddress, 0);
  }

  if (EEPROM.read(TimeFormatAddress) != 0) {
    EEPROM.write(TimeFormatAddress, 0);
  }

  if (EEPROM.read(GeigerTubeModelAddress) != 0) {
    EEPROM.write(GeigerTubeModelAddress, 0);
  }

  if (EEPROM.read(MainPageNumberAddress) != 0) {
    EEPROM.write(MainPageNumberAddress, 0);
  }

  if (EEPROM.read(DoseAlertRateAddress) != 10) {
    EEPROM.write(DoseAlertRateAddress, 10);
  }

  if (EEPROM.read(TotalDoseAlertRateAddress) != 45) {
    EEPROM.write(TotalDoseAlertRateAddress, 45);
  }

  if (EEPROM.read(DisplayBrightnessAddress) != 255) {
    EEPROM.write(DisplayBrightnessAddress, 255);
  }

  if (EEPROM.read(HVSetPointAdress) != 0) {
    EEPROM.write(HVSetPointAdress, 0);
  }

  if (EEPROM.read(CustomZeroEffectAdress) != 20) {
    EEPROM.write(CustomZeroEffectAdress, 20);
  }

  EEPROM.commit(); //Saves the changes
}
*/