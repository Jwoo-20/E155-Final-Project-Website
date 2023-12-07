---
layout: page
title: Documentation
permalink: /doc/
---

# Schematics
<!-- Include images of the schematics for your system. They should follow best practices for schematic drawings with all parts and pins clearly labeled. You may draw your schematics either with a software tool or neatly by hand. -->



<object data="../assets/schematics/FinalProjectSchematics.pdf" width="1000" height="1000" type='application/pdf'></object>

# Source Code Overview
<!-- This section should include information to describe the organization of the code base and highlight how the code connects. -->

The source code for the project is located in the Github repository [here](https://github.com/Jwoo-20/E155-Final-Project-Website/tree/main/src). The src folder is divided into three subfolders. FPGACode contains the files uploaded to the UPduino 3.1. In it there are three different files, main.sv, ledoutputs.sv, and keypad.sv. The files work together to run all the necessary functionalities performed by the FPGA. The folder MCUCode contains all the files and headers uploaded to the NUCLEO MCU. These files contain addresses, registers, functions, and everything else necessary to get the MCU to perform its functions.

# Bill of Materials
<!-- The bill of materials should include all the parts used in your project along with the prices and links.  -->

| Item | Part Number | Quantity | Unit Price | Source |
| ---- | ----------- | ----- | ---- | ---- |
| Mini Metal Gear Analog Servo |  ES08MA | 1 | $12.99 |  [link](https://www.amazon.com/ES08MA-Metal-Analog-Servo-Model/dp/B09SPLXDN4?th=1) |
| 4x4 Matrix Keypad |   | 1 |  | Storeroom  |
| PLA for 3D printing |   |  |  | Maker Space  |
| 8x8 LED dot matrix display | KWM-2088CUB  | 6 |  | Storeroom  |
| 330 ohm resistor |   | 17 |  | Storeroom  |
| 1000 ohm resistor | | 1 | | Storeroom |
| NPN Transistor |  2N3906 | 9 |  | Storeroom  |
| Relay| EC2-5NJ | 1 | | Storeroom |




**Total cost: $12.99**
