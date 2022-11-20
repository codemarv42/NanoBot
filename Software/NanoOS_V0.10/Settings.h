#ifndef SETTINGS_H
#define SETTINGS_H

#define VERSION 0.10
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

#endif
