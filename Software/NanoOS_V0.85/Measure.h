#ifndef MEASURE_H
#define MEASURE_H

extern void MPU6050Init();
extern void UpdateMPU6050();

extern float XAngle;
extern float YAngle;
extern float ZAngle;

extern void ResetZAngle();

extern float CorrectionValueZAngle;

extern int ADCRead(int pin);

#define L_A 0
#define L_I 1
#define M 2
#define R_I 3
#define R_A 4
#define L_A2 5
#define R_A2 6
#define REF  7

extern float BATVoltage();

extern void ReadEncoder();
extern int ENCCounter;
extern bool ENCButtonState;

extern void MeasureRawLight();
extern void Calibrate(int anz);
extern void MeasureLight();

extern unsigned int CAL_L[7];
extern unsigned int RAW_L[8];

extern unsigned int RAW_MIN[7];
extern unsigned int RAW_MAX[7];

extern void VL53l1xInit(int Port, unsigned int mode);
extern void UpdateVL53l1x(bool EN);

extern unsigned int DistL;
extern unsigned int DistM;
extern unsigned int DistR;

#define R 0
#define G 1
#define B 2
#define C 3
#define L 4

extern void TCS34725Init();
extern void UpdateTCS34725();

extern unsigned int RAW_TCS_L[3];
extern unsigned int RAW_TCS_R[3];

extern void CalibrateTCS34725Green(int anz, int abweichung);
extern bool TCS_Calibrate_EN;
extern bool TCSL_Green();
extern bool TCSR_Green();

extern void CalibrateTCS34725Red(int anz, int abweichung);
extern bool TCSL_Red();
extern bool TCSR_Red();

//Calibrated green values
extern unsigned int RAW_TCS_L_G_MAX[5];
extern unsigned int RAW_TCS_L_G_MIN[5];
extern unsigned int RAW_TCS_R_G_MAX[5];
extern unsigned int RAW_TCS_R_G_MIN[5];

//Calibrated red values
extern unsigned int RAW_TCS_L_R_MAX[5];
extern unsigned int RAW_TCS_L_R_MIN[5];
extern unsigned int RAW_TCS_R_R_MAX[5];
extern unsigned int RAW_TCS_R_R_MIN[5];

extern bool CalState;

extern void SetTCS_Green(bool DATASET);
extern void SetTCS_Red(bool DATASET);

extern unsigned int RAW_L_REF;

#endif
