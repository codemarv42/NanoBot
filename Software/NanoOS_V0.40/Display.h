#ifndef DISPLAY_H
#define DISPLAY_H

extern void DisplayInit();
extern void BootPage();
extern void Menu();
extern void MPUCaliPage();
extern void BatteryWarning();

extern int MenuPage;
extern bool ENMainProgram;

extern void MenuActions();

extern void DriveFixedAngle(int time);

extern bool LoopState;

extern void DisplayTilesCase();

#endif
