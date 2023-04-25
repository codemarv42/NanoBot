#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION        0.90
#define MODELLNR       "DEV02"
#define DEBUGMODE      0 //0 - OFF, 1 - SET
#define CONTESTMODE    0 //0 - OFF, 1 - SET // If activated the robot will start directly the main program at boot
#define MANUALRGBVAL_G 1 //0 - OFF, 1 - SET // The calibrated green values are set manually
#define MANUALRGBVAL_R 1 //0 - OFF, 1 - SET // The calibrated red values are set manually
#define DEBUGRESCUE    0 //0 - OFF, 1 - SET // Starts the rescue program directly after start

//Engine speeds// At full battery charge!

#define Normal  80
#define Fast    150
#define Slow    80
#define Down    70
#define Up      120

//Display//

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 2 //Integers only!

//Buzzer//

#define EN_BUZZER 0 //1 - ON, 0 - OFF

//Grayscale//

#define Grey   300
#define Black  150
#define White  850
#define Metal  1500
#define Green  450

//Other//

#define ObstacleDir 1 //0 - left, 1 - right

#endif
