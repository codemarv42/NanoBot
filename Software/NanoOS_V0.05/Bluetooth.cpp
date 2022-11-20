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

/*
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "TFT_eSPI.h"
#include "PCF8563.h"

#include "Bitmap.h"
#include "Display.h"
#include "Functions.h"
#include "Measure.h"
#include "Pins.h"
#include "Settings.h"
#include "Tests.h"
#include "Bluetooth.h"
#include "BluetoothSerial.h"
#include "ESP32_SD.h"
#include "ESP32_WiFi.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

*/
/////////////////////////////////////////////

/////////////////Variables///////////////////

/* 
int BTSpeed = 1000; //500 - 2000ms min alue is depending of the calculation loop
String BTNameModel = "GMZ-05";
String BTName = "";
bool BTLog = false;
float BTVersion = 4.20;
*/
/////////////////////////////////////////////

/*
int GenerateRandomNumber(int digits) {
  randomSeed(hallRead());
  int RandomValue = 0;

  if (digits == 0) {
    RandomValue = random(0, 9);
  } else if (digits == 1) {
    RandomValue = random(0, 99);
  } else if (digits == 2) {
    RandomValue = random(0, 999);
  } else if (digits == 3) {
    RandomValue = random(0, 9999);
  }
  return RandomValue;
}

void BTOn() {
  BTName = String(BTNameModel + "_" + GenerateRandomNumber(3));
  SerialBT.begin(BTName);
  Serial.println("Bluetooth active");
}

bool BTConnected() { //Ergibt Fehler!
  if (BTStat != 0) {
    return SerialBT.connected(1);
  } else {
    return false;
  }
}

void BTOff() {
  SerialBT.disconnect();
  SerialBT.end();
}

void BTPrintData() {
  SerialBT.print(GEIGEROS_VERSION); //Version
  SerialBT.print("|");

  SerialBT.print(GMZ_MODEL_NR); //Model number
  SerialBT.print("|");

  SerialBT.print(IsCharging()); //Battery Status
  SerialBT.print("|");

  SerialBT.print(BATPercent); //Battery Percentage
  SerialBT.print("|");

  SerialBT.print(CPM); //Counts per minute
  SerialBT.print("|");

  SerialBT.print(CurrentCount); //Total counts
  SerialBT.print("|");

  SerialBT.print(DoseLevel); //Dose level
  SerialBT.print("|");

  SerialBT.print(DoseUnit); //Dose unit
  SerialBT.print("|");

  if (DoseUnit == 0) {
    SerialBT.print(DoseUnitSivert); //Sivert
    SerialBT.print("|");
  } else if (DoseUnit == 1) {
    SerialBT.print(DoseUnitRem); //Rem
    SerialBT.print("|");
  } else if (DoseUnit == 2) {
    SerialBT.print(DoseUnitSivert); //Gray
    SerialBT.print("|");
  }

  SerialBT.print(DoseRate, 4); //Dose rate
  SerialBT.print("|");

  SerialBT.print(MaxDoseUnit); //Max dose unit
  SerialBT.print("|");

  SerialBT.print(MaxDoseRate, 4); //Max dose rate
  SerialBT.print("|");

  SerialBT.println(TotalDose, 4); //Total dose

  //Log Data//

  if (BTLog == true) {
    if (Language == 0) { //German
      SerialBT.println("―――――――――");

      SerialBT.print("BAT: ");
      SerialBT.print(BATPercent);
      SerialBT.println("%");

      SerialBT.print("CPM: ");
      SerialBT.println(CPM);

      SerialBT.print("Zaehlungen: ");
      SerialBT.println(CurrentCount);

      SerialBT.print("Dosis: ");

      if (DoseUnit == 0) {
        if (DoseUnitSivert == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("uSv/h");
        } else if (DoseUnitSivert == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mSv/h");
        } else if (DoseUnitSivert == 2) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Sv/h");
        }
      } else if (DoseUnit == 1) {
        if (DoseUnitRem == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mRem/h");
        } else if (DoseUnitRem == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Rem/h");
        }
      } else if (DoseUnit == 2) {
        if (DoseUnitSivert == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("uGy/h");
        } else if (DoseUnitSivert == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mGy/h");
        } else if (DoseUnitSivert == 2) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Gy/h");
        }
      }

      SerialBT.print("Dosis Level: ");
      if (DoseLevel == 0) { //Radiation
        SerialBT.println("Normal");
      } else if (DoseLevel == 1) {
        SerialBT.println("Erhoeht");
      } else if (DoseLevel == 2) {
        SerialBT.println("Hoch!");
      } else if (DoseLevel == 3) {
        SerialBT.println("Toedlich!");
      }

      SerialBT.print("Ges. Dosis: ");

      if (DoseUnit == 0) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("uSv");
      } else if (DoseUnit == 1) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("mRem");
      } else if (DoseUnit == 2) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("uGy");
      }

      SerialBT.print("Max. Dosis: ");

      if (MaxDoseUnit == 0) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("uSv/h");
      } else if (MaxDoseUnit == 1) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mSv/h");
      } else if (MaxDoseUnit == 2) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Sv/h");
      } else if (MaxDoseUnit == 3) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mRem/h");
      } else if (MaxDoseUnit == 4) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Rem/h");
      } else if (MaxDoseUnit == 5) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("uGy/h");
      } else if (MaxDoseUnit == 6) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mGy/h");
      } else if (MaxDoseUnit == 7) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Gy/h");
      }

      SerialBT.println("―――――――――");

    } else if (Language == 1) { //English

      SerialBT.println("―――――――――");

      SerialBT.print("BAT: ");
      SerialBT.print(BATPercent);
      SerialBT.println("%");

      SerialBT.print("CPM: ");
      SerialBT.println(CPM);

      SerialBT.print("Counts: ");
      SerialBT.println(CurrentCount);

      SerialBT.print("Dose: ");

      if (DoseUnit == 0) {
        if (DoseUnitSivert == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("uSv/h");
        } else if (DoseUnitSivert == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mSv/h");
        } else if (DoseUnitSivert == 2) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Sv/h");
        }
      } else if (DoseUnit == 1) {
        if (DoseUnitRem == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mRem/h");
        } else if (DoseUnitRem == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Rem/h");
        }
      } else if (DoseUnit == 2) {
        if (DoseUnitSivert == 0) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("uGy/h");
        } else if (DoseUnitSivert == 1) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("mGy/h");
        } else if (DoseUnitSivert == 2) {
          SerialBT.print(DoseRate, 4);
          SerialBT.println("Gy/h");
        }
      }

      SerialBT.print("Dose level: ");
      if (DoseLevel == 0) { //Radiation
        SerialBT.println("Normal");
      } else if (DoseLevel == 1) {
        SerialBT.println("Elevated");
      } else if (DoseLevel == 2) {
        SerialBT.println("High!");
      } else if (DoseLevel == 3) {
        SerialBT.println("Deadly!");
      }

      SerialBT.print("Full Dose: ");

      if (DoseUnit == 0) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("uSv");
      } else if (DoseUnit == 1) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("mRem");
      } else if (DoseUnit == 2) {
        SerialBT.print(TotalDose, 5);
        SerialBT.println("uGy");
      }

      SerialBT.print("Max. Dose: ");

      if (MaxDoseUnit == 0) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("uSv/h");
      } else if (MaxDoseUnit == 1) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mSv/h");
      } else if (MaxDoseUnit == 2) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Sv/h");
      } else if (MaxDoseUnit == 3) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mRem/h");
      } else if (MaxDoseUnit == 4) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Rem/h");
      } else if (MaxDoseUnit == 5) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("uGy/h");
      } else if (MaxDoseUnit == 6) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("mGy/h");
      } else if (MaxDoseUnit == 7) {
        SerialBT.print(MaxDoseRate);
        SerialBT.println("Gy/h");
      }

      SerialBT.println("―――――――――");

    }
  }
}

*/