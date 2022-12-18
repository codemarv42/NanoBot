#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.40
#define MODELLNR 1
#define DEBUGMODE 0

//Engine speeds//

#define Normal  100
#define Fast    200
#define Slow    85
#define Down    50
#define Up      200

//Display//

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 1

//Buzzer//

#define EN_BUZZER 0 //1 - ON, 0 - OFF

//Grayscale//

#define Grey   500
#define Black  150
#define White  850
#define Metal  1500
#define Green  450

#endif
