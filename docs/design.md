---
layout: page
title: Design
permalink: /design/
---
# New Hardware
There are two major pieces of new hardware in this project. The first is the APDS 9960 motion/gesture sensor. This sensor is being used to detect when to open and close the mask. The other new hardware is the EMAX ES08MA servo motor. The motors are used to control the movement of the opening and closing of the mask.

# MCU Design

## MCU Block Diagram

# FPGA Design
The FPGA has two major uses in the helmet. The first is reading the 4x4 matrix keypad. The keypad is used to detect whether or not the helmet is in its enabled mode. The helmet starts in a disabled mode, if the correct keypad code is put into the keypad, the helmet goes into the enabled mode. When the helmet is in the disabled mode the eye lights stay off and the mask will not be able to be opened. Once the correct enable code is put into the keypad the eyes light up and the mask is then able to be opened and closed using the gesture sensor. The second major use of the FPGA is the LED eyes. Each eye is made up of 8x8 LED matrices. When the correct enable code is put into the keypad, a short animation is played of the lights coming on. Another easter egg enable code is also installed. If that code is put into the keypad a different, cooler, animation is played on the eye LEDs before the helmet is put into the enable mode.
Once the keypad is in the enable mode it will stay enabled until the correct disable code is input. Even if random keys are pressed on the keypad, it will stay enabled unless the disable code is input. Once that disable code is input a powering-down animation will play on the eyes and the helmet will be put back into disable mode with the eyes off.

The keypad code detection works by reading the inputs from the keypad matrix, storing the five most recent inputs in registers, and then comparing the values in those registers to the known enable/disable codes. The FPGA reads the keypad by scanning through the columns while reading the rows. It works using a finite state machine, quickly flipping between four states, each one corresponding to a column. When the state is on the corresponding column that column is driven high. When a button is pressed, the corresponding row and column are connected. The FSM cycles through the columns very quickly so once any of the rows detects an input, the FPGA can determine which corresponding button on the keypad was pressed by seeing which row the input is on and which column the FSM is in. It then stays in a hold state until the button is released and it goes back to scanning through rows. Five registers store the five most recent button presses, as each button is pressed the value of each register is shifted by one register, and the most recent value is put into the first register.

<div style="text-align: left">
  <img src="./assets/img/keypadFSM.png" alt="keypadFSM" />
</div>



The code detection and enable state are also done using finite state machines. The system starts in the disabled state. Once the right five digits in the right order are stored in the registers the state changes from disabled to enabled. It will stay in that state until the correct disable code is input where it will then enter the disabled state.

<div style="text-align: left">
  <img src=".docs/assets/img/enablecodeFSM.png" alt="enablestateFSM" />
</div>






## FPGA Block Diagram
