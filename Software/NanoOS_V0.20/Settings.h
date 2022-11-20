#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.20
#define MODELLNR DEV01

//Engine speeds//

#define Normal  160
#define Fast    200
#define Slow    90
#define Down    110
#define UP      255

//Display//

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 10

//Buzzer//

#define EN_BUZZER 1 //1 - ON, 0 - OFF

//Grayscale//

#define Grey   35
#define Black  15
#define White  85
#define Metal  130

#endif
