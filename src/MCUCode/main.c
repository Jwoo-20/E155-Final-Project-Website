/*James Woo and Kaito Robson
jmwa2020@mymail.pomona.edu    kara2020@mymail.pomona.edu
12/7/2023
File to control the motor and LED eyes of the Iron Man mask.
Opens and closes the mask and tells the FPGA what state it is in.
*/


#include <stdint.h>
#include <stdbool.h>
#include "STM32L432xx.h"
#include "STM32L432KC.h"

volatile bool open = false;
volatile bool button = false;
volatile bool enabled = false;


int main(void) {


  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;  //enable GPIO A
  RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;  //enable TIM2
  RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;  //enable TIM15

  TIM2->CCMR1 |= _VAL2FLD(TIM_CCMR1_OC1M, 0b0110);  //set OC1M to PWM mode 1
  TIM2->CCMR1 |= TIM_CCMR1_OC1PE; //set OCPE for channel 1
  TIM2->CR1 |= TIM_CR1_ARPE;  //set ARPE bit

  TIM2->CR2 |= _VAL2FLD(TIM_CR2_MMS, 0b100);  //set OC1REF as trigger output
  TIM2->CCER |= TIM_CCER_CC1E;  //enable output for OC1

  TIM2->EGR |= TIM_EGR_UG;  //Set update flag for TIM2 PWM

  TIM2->ARR = (1000);  //set frequency for note
  TIM2->CCR1 = (0);   //set duty cycle to 50% for current note

  TIM2->EGR |= TIM_EGR_UG; //Set update flag for TIM2 PWM

  initTIM(TIM2);
  initTIM(TIM15);

  TIM2->PSC = (39); //set prescale value

  pinMode(PA5, GPIO_ALT); //TIM2 PWM Output alternate function
  pinMode(PA4, GPIO_INPUT); //Button toggling open and close of helmet
  pinMode(PA10, GPIO_INPUT);  //Reads whether the system is in the enable state from the FPGA
  pinMode(PA9, GPIO_OUTPUT);  //TElls the FPGA if the mask is open or closed

  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD4, 0b01); // Set PA4 as pull-up
  GPIOA->PUPDR |= _VAL2FLD(GPIO_PUPDR_PUPD10, 0b01); // Set PA10 as pull-up
  GPIOA->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 1);  //set PA5 to AF1 to output PWM from TIM2 Ch 1
 

while(1){

button = !digitalRead(PA4);
enabled = digitalRead(PA10);

if (button & open & enabled) {  //close the mask

TIM2->CCR1 = (550);   //set duty cycle to 55% to move motor
delay_millis(TIM15, 1000);  //delay to give it time to move
open = false;
digitalWrite(PA9, 0); //tell FPGA that mask is closed
TIM2->CCR1 = 0; //turn off motor


    }


else if (button & !open & enabled) {  //open the mask

TIM2->CCR1 = (60);  //set duty cycle to 6%
delay_millis(TIM15, 500);
open = true;
digitalWrite(PA9, 1); //tell FPGA mask is open
    }

  }

}

