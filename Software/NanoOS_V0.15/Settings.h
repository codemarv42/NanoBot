#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.15
#define MODELLNR DEV01

//Engine speeds//

#define Normal  160
#define Fast    200
#define Slow    110
#define Down    90
#define UP      255

//Display//

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 0.9

//Buzzer//

#define EN_BUZZER 1 //1 - ON, 0 - OFF

#endif
