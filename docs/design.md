---
layout: page
title: Design
permalink: /design/
---
#New Hardware
There are two major pieces of new hardware in this project. The first is the APDS 9960 motion/gesture sensor. This sensor is being used to detect when to open and close the mask. The other new hardware is the EMAX ES08MA servo motor. The motors are used to control the movement of the opening and closing of the mask.

# MCU Design

## MCU Block Diagram

# FPGA Design
The FPGA has two major uses in the helmet. The first is reading the 4x4 matrix keypad. The keypad is used to detect whether or not the helmet is in its enabled mode. The helmet starts in a disabled mode, if the correct keypad code is put into the keypad, the helmet goes into the enabled mode. When the helmet is in the disabled mode the eye lights stay off and the mask will not be able to be opened. Once the correct enable code is put into the keypad the eyes light up and the mask is then able to be opened and closed using the gesture sensor. The second major use of the FPGA is the LED eyes. Each eye is made up of 8x8 LED matrices. When the correct enable code is put into the keypad, a short animation is played of the lights coming on. Another easter egg enable code is also installed. If that code is put into the keypad a different, cooler, animation is played on the eye LEDs before the helmet is put into the enable mode.
Once the keypad is in the enable mode it will stay enabled until the correct disable code is input. Even if random keys are pressed on the keypad, it will stay enabled unless the disable code is input. Once that disable code is input a powering-down animation will play on the eyes and the helmet will be put back into disable mode with the eyes off.


## FPGA Block Diagram
