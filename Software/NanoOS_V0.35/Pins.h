#ifndef PINS_H
#define PINS_H

////////////////////Status LEDs/////////////////////

#define STAT_LED_RED     14
#define STAT_LED_GREEN   2
#define STAT_LED_BLUE    15

///////////ADC Multiplexer (CD74HC4067S)////////////

#define S0               4
#define S1               5
#define S2               12
#define S3               13
#define ADC_MULTI        39

#define ADC_PT_L_1       0
#define ADC_PT_L_0       1
#define ADC_PT_M         2
#define ADC_PT_R_0       3
#define ADC_PT_R_1       4
#define ADC_VBAT         5

/////////////////////Servos/////////////////////////

#define SERVO1           19
#define SERVO2           18

/////////////Motor Driver (TB6612FNG)///////////////

#define PWMA             23
#define PWMB             25

////////////Shiftregister (74HC595PW)///////////////

#define SHCP             27
#define STCP             32
#define DS               33

#define SR_AIN1          0
#define SR_AIN2          1
#define SR_BIN1          2
#define SR_BIN2          3
#define SR_STBY          4
#define SR_LED_RGB       5
#define SR_LED_PT        6

/////////////////Rotary Encoder/////////////////////

#define ENC_A            35
#define ENC_B            34
#define ENC_SW           36

/////////////////////Buzzer/////////////////////////

#define BUZZER           26

//////////////////////Extra/////////////////////////

#define EXT1             16
#define EXT2             17

//////////////////////I2C///////////////////////////

#define SDA              21
#define SCL              22
#define MPU6050_ADRESS   0x51
#define I2CMULTI_ADRESS  0x70
#define OLED_ADRESS      0x3C
#define TOF_ADRESS       0x29
#define RGBS_ADRESS      0x29


#endif
