# NanoBot

The NanoBot is basically a small robot, which was developed for educational purposes. It comes with a variety of sensors such as light sensors, color sensors, distance sensors and with the paired Jetson Nano from Nvidia with one or more cameras. In addition, you can connect even more modules and sensors through the extra pins, making the limits almost infinite. 

![NanoBotRev1](https://github.com/codemarv42/NanoBot/blob/main/Hardware/NanoBot%20REV%20V1/Real1.JPG)


# Hardware

The hardware of this project consists of a self-developed circuit board and 3D printed parts made of TPE 60A and PLA. 

## Short building instruction

### Note that only the first version of the NanoBot board works without problems, so it is not recommended to rebuild the latest version!

The assembly of the robot requires **extensive equipment and experience** in soldering SMD components. How you solder the components is up to you.

Recommended tools:

- Flat tip soldering iron
- Solder (0.8mm), flux
- Hotplate
- Hot air soldering station
- Solder paste (SnAg3Cu0.5)
- Tweezers, side cutter

The required components are listed in the Excel document in the hardware folder and are mostly from LCSC. The board itself can be produced by different manufacturers, for example JLCPCB. Simply upload the Gerber file and select the following parameters:

Parameter for PCB (NanoBot REV1):

- Layer: 2
- Thickness: 1,6mm / 2mm
- Length: 112.9mm
- Height: 100.8mm
- Minimum Copper Trace Width: 0.15mm

Parameter for PCB (NanoBot REV2):

- Layer: 2
- Thickness: 1,6mm / 2mm
- Length: 112.9mm
- Height: 100.8mm
- Minimum Copper Trace Width: 0.15mm

## 3D printed parts

For better light measurements, it is recommended to print out the three different light guards and attach them with superglue. Also, use TPE 60A for the tires, as this elastic material prevents the robot from slipping on the floor, unlike TPU.

Parts: 
- ***4x Flange coupling connector 3mm (not 3d printed!)***
- 4x Motor bracket PLA (40% infill)
- 1x light sensor protection Resin (Black)
- 2x Outer light sensor protection Resin (Black)
- 4x Tire TPE 60A
- 1x Rotary encoder knob TPU / TPE
- 4x Rim PLA

<img src="https://github.com/codemarv42/NanoBot/blob/main/Hardware/3D%20Models/Tires/Flange%20coupling%20connector.png" width=40% height=40%>

![Light protector](https://github.com/codemarv42/NanoBot/blob/main/Hardware/3D%20Models/Light%20sensors/Light%20protection.jpg)

## Improvements

> - [ ] Place light sensors in the center to increase maneuverability
> - [ ] Bigger Tires
> - [X] Better XT-30 position on the PCB
> - [X] More external ports
> - [X] Internal Li-Po charger, which draws power via USB-C PD
> - [X] Physical switch or MOSFET to disconnect the battery from the PCB
> - [X] Battery under discharge protection
> - [X] Additional light sensors on the outside of the PCB
> - [X] Constant motor voltage
> - [ ] Jetson Nano power supply with short circuit protection
> - [X] Material reflective sensor on the bottom
> - [X] Use Led with a wider spectrum
> - [X] Esd and insulation around gyro sensor
> - [ ] Reset button does not work
> - [X] Mounting holes next to the light sensor bar and the silver strip sensor

![Nano Bot REV2](https://github.com/codemarv42/NanoBot/blob/main/Hardware/Nano%20Bot%20REV%20V2/PXL_20230427_140323679~2.jpg)

# Software

In my case, the robot is used to follow a line, avoid obstacles on the track and collect balls. These are the requirements for RoboCup in the Rescue Line division. However, depending on the requirements of the robot, the code will vary in your case. Therefore, this code serves mainly as a basis for your application. However, the code contains everything in the overall package to control and read out all sensors and peripherals. You can also reprogram the robot with Micro Python, because the processor (ESP32) allows it.

Required libraries:

- Adafruit_GFX.h - https://github.com/adafruit/Adafruit-GFX-Library
- Adafruit_SSD1306.h - https://github.com/adafruit/Adafruit_SSD1306
- ESP32_Servo.h - https://github.com/jkb-git/ESP32Servo
- MPU6050_light.h - https://github.com/rfetick/MPU6050_light
- CD74HC4067.h - https://github.com/waspinator/CD74HC4067
- VL53L1X.h - https://github.com/pololu/vl53l1x-arduino
- Adafruit_TCS34725.h - https://github.com/adafruit/Adafruit_TCS34725
- Adafruit_BusIO - https://github.com/adafruit/Adafruit_BusIO

## USB driver

In some cases, the USB controller is not automatically detected, which means that you have to install an additional driver:

- https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

## Compilation settings

To compile your own program and then upload it to the NanoBot, some steps are necessary:

1. First you have to install the kernel of the ESP32 in Arduino: https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html
2. Next, the compilation settings must be specified. To do this, first select "ESP32 Dev Module" under Tools --> Board.
3. In the last step, the parameters of the ESP32 on the NanoBot must be set. To do this, select the following under Tools: 
- Board: "ESP32 Dev Module"
- Upload Speed: "921600"
- CPU Frequency: "240MHz (WiFi/BT)"
- Flash Frequency: "80MHz"
- Flash Mode: "QIO"
- Flash Size: "16MB (128Mb)" **4MB if you installed the W25Q32JVZPIQ Flash**
- Partition Scheme: "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)" **for smaller code: Default**
- Programmer: "AVRISP mkll"
- Erase all flash: "Disabled"

## Improvements

> - [ ] Develop a communication protocol for the ESP32 and Jetson Nano via the I2C pin

## Flash BIN files to the robot

- Download the flash tool: https://www.espressif.com/en/support/download/other-tools
- Unzip the downloaded ZIP file and copy the selected BIN file into the bin folder contained in the unzipped file
- Start the exe file and select esp32
- Press the upper first box and then select the path of the bin file
- write next to the "@": 0x10000
- Then connect the robot to the pc and select the correct COM port (If the COM port is not displayed, install the driver)
- Click on start und press the boot button on the robot, if the upload does not start (If the boot button is not present, bridge the two pads with the name: "BOOT" and "GND")
