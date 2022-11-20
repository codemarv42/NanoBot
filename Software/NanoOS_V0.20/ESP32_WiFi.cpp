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

//int WiFiMode = 0; //0-Off, 1-Setup mode, 2-Server/client mode

/////////////////////////////////////////////
/*
void WiFiSetup() {
    
    WiFi.mode(WIFI_STA);
    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    //wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result

    bool res;
     //res = wm.autoConnect(); // auto generated AP name from chipid
     // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("GMZ-04","password"); // password protected ap

    if(!res) {
        Serial.println("WiFi Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("WiFi connected");
    }
}

*/