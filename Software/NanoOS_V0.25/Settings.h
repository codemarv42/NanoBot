#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.25
#define MODELLNR DEV01

//Engine speeds//

#define Normal  140
#define Fast    200
#define Slow    90
#define Down    110
#define UP      255

//Display//

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define FollowLineDiffFactor 1

//Buzzer//

#define EN_BUZZER 0 //1 - ON, 0 - OFF

//Grayscale//

#define Grey   400
#define Black  120
#define White  900
#define Metal  1500

#endif
