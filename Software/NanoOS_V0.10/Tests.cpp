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

/////////////////////////////////////////////

//////////////////Variables//////////////////



/////////////////////////////////////////////

void MotorTest(){
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