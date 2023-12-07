---
layout: page
title: Project Overview
permalink: /
exclude: true
---

# Project Abstract
In this project, the team aims to build a decorative Iron Man helmet with a few functionalities that are seen in the movies. The system reads its inputs from a keypad and a gesture sensor using the field programmable gate array (FPGA) and microcontroller (MCU) respectively. Inside the helmet are mounted two motors which are able to open and close the face mask on the helmet. Where the eyes are on the mask are 8x8 LED matrices. The FPGA and MCU communicate with each other and use the inputs to determine when to open and close the helmet as well as what to display on the LED eyes. 


<div style="text-align: left">
  <img src="./assets/img/Logo.png" alt="logo" width="100" />
</div>


# Project Motivation
A functional Iron Man suit is something that many people have wished for. Sadly, due to technological limitations, we are not able to fly through the sky in a metal suit. However, we are able to build a decorative (non-wearable) replica of the famous Iron Man helmet with a few of the functionalities. This helmet is a cool device that is able to open and close the face mask and can display different patterns and animations in the light-up eyes. The functionality is locked behind a secret keypad code so that only people who are allowed can use the helmet.



# System Block Diagram
<div style="text-align: left">
  <img src="./assets/schematics/FinalProjectFullBlock" alt="fullBlockDiagram.png" />
</div>

The block diagram of the full system is shown above. The MCU communicates with the gesture sensor using the I2C communication protocol. The MCU controls the motors via a driver while using PWM to control the position. The MCU and FPGA communicate with each other using direct connections. There are only two pieces of information that need to be communicated (whether the keypad has gotten the correct enable code and whether the helmet is open or closed) so a simple connection is sufficient. The FPGA interfaces with the keypad, scanning through the columns while reading the rows to read 16 different key inputs. The FPGA also controls the 8x8 LED matrices using time multiplexing. This allows the MCU to light up any combination of the 64 individual LED dots using 16 connections.


# Authors


[James Woo](https://www.linkedin.com/in/james-m-woo/), Pomona '24. Majoring in physics with an interest in electrical and embedded engineering.


[Kaito Robson](https://www.linkedin.com/in/kaitorobson/), Pomona '24. Majoring in physics. 






