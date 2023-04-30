#ifndef PINS_H
#define PINS_H

#define EN_RST           17
#define P_ON             2

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
#define ADC_PT_L_2       5
#define ADC_PT_R_2       6
#define ADC_PT_REF       7
#define ADC_AE1          8 //External Analog Input
#define ADC_AE2          9 //External Analog Input


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

//Shiftregister IC1

#define SR_AIN1          0
#define SR_AIN2          1
#define SR_BIN1          2
#define SR_BIN2          3
#define SR_STBY          4
#define SR_LED_RGB       5
#define SR_LED_PT        6
#define SR_LED_L_RED     7

//Shiftregister IC2

#define SR_LED_L_GREEN   8
#define SR_LED_L_BLUE    9
#define SR_LED_R_RED     10
#define SR_LED_R_GREEN   11
#define SR_LED_R_BLUE    12
#define SR_DE1           13 //External Digital Pins
#define SR_DE2           14 //External Digital Pins
#define SR_DE3           15 //External Digital Pins

/////////////////Rotary Encoder/////////////////////

#define ENC_A            35
#define ENC_B            34
#define ENC_SW           15

/////////////////////Buzzer/////////////////////////

#define BUZZER           26

////////////////////External////////////////////////

#define EXT1             16

/////////////////////Battery////////////////////////

#define VBAT             36
#define CHARGE_STAT      14 //NC in V2 because of no Voltage devider before Input Pin Voltage Overshoot about 1V!

//////////////////////I2C///////////////////////////

#define SDA              21
#define SCL              22
#define MPU6050_ADRESS   0x51
#define I2CMULTI_ADRESS  0x70
#define OLED_ADRESS      0x3C
#define TOF_ADRESS       0x29
#define RGBS_ADRESS      0x29


#endif
