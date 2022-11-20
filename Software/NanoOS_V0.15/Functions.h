#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define OUT_A  1
#define OUT_B  2
#define OUT_AB 3

extern void HardwareInit();
extern void ShiftRegisterWrite(int pin, bool state);
extern void ShiftRegisterReset();
extern void Forward(byte mot, int v);
extern void Backward(byte mot, int v);
extern void Stop();
extern void Left(int v);
extern void Right(int v);
extern void RotateAngle(int ang);

extern void ServoInit();
extern void Servo1(int pos);
extern void Servo2(int pos);

extern void FollowLine(int v);

extern void TCASelect(uint8_t i);

extern void PlayTone(int val);

extern void Loop1Code( void * pvParameters );

#endif
