#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.45
#define MODELLNR "DEV01"
#define DEBUGMODE 0

//Engine speeds// At full battery charge!

#define Normal  100
#define Fast    200
#define Slow    80
#define Down    50
#define Up      200

//Display//

#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 1 //Integers only!

//Buzzer//

#define EN_BUZZER 0 //1 - ON, 0 - OFF

//Grayscale//

#define Grey   500
#define Black  150
#define White  850
#define Metal  1500
#define Green  450

//Other//

#define ObstacleDir 0 //0 - left, 1 - right

#endif
