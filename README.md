# **860 MHz - 960 MHz RX / TX Amplifier**

## Specs

RX / TX Gain 19.8 dB

0.45 dB Noise Figure

TX sense adjustable from -16 dBm to +22 dBm

Supply VIN via BIAS-T 6-9 V

## Indicators

Indicator | Function
--------- | --------
VIN | Coax input voltage is present
5V | 5 V LDO operational
3V3 | 3.3 V LDO operational
RX | Amplifier in receive mode
TX | Amplifier in transmit mode
MCU | Microcontroller is operational
BC | Starts blinking if transmit mode got activated at least once, on continous if transmit mode got activated at least 5 times

## Threshold

Adjust TX threshold via trimmer, LED bar shows approximate dBm value, more precise output is possible via USB connection serial port

## PCB Stack Up

Layer | Material | Thickness (mil) | Thickness (mm)
----- | -------- | --------------- | --------------
L1 | Outer Copper Weight 1 oz | 1.38 | 0.0350
Prepreg | 7628, RC 49%, 8.6 mil | 8.28 | 0.2104
L2 | Inner Copper Weight 0.5 oz | 0.6 | 0.0152
Core | 0.4mm H/HOZ without copper | 15.75 | 0.4
L3 | Inner Copper Weight 0.5 oz | 0.6 | 0.0152
Prepreg | 7628, RC 49%, 8.6 mil | 8.28 | 0.2104
L4 | Inner Copper Weight 0.5 oz | 0.6 | 0.0152
Core | 0.4mm H/HOZ without copper | 15.75 | 0.4
L5 | Inner Copper Weight 0.5 oz | 0.6 | 0.0152
Prepreg | 7628, RC 49%, 8.6 mil | 8.28 | 0.2104
L6 | Outer Copper Weight 1 oz | 1.38 | 0.0350

## Flashing Firmware

Connect via USB-C to PC while pressing boot button, device shows up as external flash drive, drop .uf2 file to drive. Done.

## Pictures and Renders

![PCB_TQP_2024-10-10_1](https://github.com/user-attachments/assets/c339026f-137f-4f76-b3a7-dd731eb48abd)
![PCB_TQP_2024-10-10_1](https://github.com/user-attachments/assets/2eb37f10-12c8-40d0-aa93-4dd3a8db7e9a)

![PCB_TQP_2024-10-10_2](https://github.com/user-attachments/assets/2bde4ddd-27e3-476c-9faf-c19b9ea2110a)
![PCB_TQP_2024-10-10_3](https://github.com/user-attachments/assets/bb755897-9bde-4b0e-9cd5-cb3eef907eb5)

![PCB_TQP_2024-10-10_4](https://github.com/user-attachments/assets/abdddf95-8d66-4785-8ba8-203b6b7afbed)
![PCB_TQP_2024-10-10_5](https://github.com/user-attachments/assets/aa46f630-b872-49d5-a332-80911795692e)

![PCB_TQP_2024-10-10_6](https://github.com/user-attachments/assets/4751a517-5133-4078-891a-e124be13de59)

![IMG_6986](https://github.com/user-attachments/assets/494973b2-5131-44f8-90a0-f34d8f4bf3d3)
![IMG_6978](https://github.com/user-attachments/assets/cdfc0fae-265b-4c84-83c9-b4451f8b29fc)
![IMG_69741](https://github.com/user-attachments/assets/bdfffde8-1579-41e2-9c6d-57feef7329da)
![top](https://github.com/user-attachments/assets/38a3f5bc-5639-40c8-a40e-b3464e4d8ae5)
![bot](https://github.com/user-attachments/assets/90cc6d5b-512d-443d-a465-74136ed82f9e)
![nanovna-h4_2024-10-08_07-15-48](https://github.com/user-attachments/assets/bd2b8982-fd15-4655-a4a9-18fa295bbd67)


