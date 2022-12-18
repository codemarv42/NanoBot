///////////////////Includes//////////////////

#include <Arduino.h>

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

/////////////////////////////////////////////

//////////////////Variables//////////////////



/////////////////////////////////////////////

void MotorTest() {
  Forward(OUT_AB, 255);
  delay(1500);
  Forward(OUT_A, 255);
  delay(1000);
  Backward(OUT_B, 255);
  delay(1000);
  Backward(OUT_AB, 255);
  delay(1500);
  Stop();

  Left(Slow);
  delay(1500);
  Right(Slow);
  delay(1500);
  Stop();
}

void LightTest() {
  digitalWrite(STAT_LED_RED, LOW);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, HIGH);
  delay(120);
  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, LOW);
  digitalWrite(STAT_LED_BLUE, HIGH);
  delay(120);
  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, LOW);
  delay(120);
  digitalWrite(STAT_LED_RED, HIGH);
  digitalWrite(STAT_LED_GREEN, HIGH);
  digitalWrite(STAT_LED_BLUE, HIGH);

  ShiftRegisterWrite(SR_LED_RGB, HIGH);
  delay(120);
  ShiftRegisterWrite(SR_LED_RGB, LOW);
  ShiftRegisterWrite(SR_LED_PT, HIGH);
  delay(120);
  ShiftRegisterWrite(SR_LED_PT, LOW);
}