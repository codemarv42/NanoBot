#ifndef MEASURE_H
#define MEASURE_H

extern void MPU6060Init();
extern void UpdateMPU6050();
extern void ResetZAngle();

extern float XAngle;
extern float YAngle;
extern float ZAngle;

extern int ADCRead(int pin);

#define PT_L_A  0
#define PT_L_I  1
#define PT_M    2
#define PT_R_I  3
#define PT_R_A  4

extern float BATVoltage();

extern void ReadEncoder();
extern int ENCCounter;

#endif
