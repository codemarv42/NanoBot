#ifndef MEASURE_H
#define MEASURE_H

extern void MPU6060Init();
extern void UpdateMPU6050();
extern void ResetZAngle();

extern float XAngle;
extern float YAngle;
extern float ZAngle;

extern int ADCRead(int pin);

#define L_A  0
#define L_I  1
#define M    2
#define R_I  3
#define R_A  4

extern float BATVoltage();

extern void ReadEncoder();
extern int ENCCounter;
extern bool ENCButtonState;

extern void MeasureRawLight();
extern void Calibrate(int anz);
extern void MeasureLight();

extern unsigned int CAL_L[5];
extern unsigned int RAW_L[5];

extern unsigned int RAW_MIN[5];
extern unsigned int RAW_MAX[5];

extern void VL53l0xInit(int Port);
extern void UpdateVL53l0x();

extern unsigned int DistL;
extern unsigned int DistM;
extern unsigned int DistR;

#define R   0
#define G   1
#define B   2
#define C   3

extern void TCS34725Init();
extern void UpdateTCS34725();

extern unsigned int RAW_TCS_L[4];
extern unsigned int RAW_TCS_R[4];

#endif
