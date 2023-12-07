/*
James Woo, jmwa2020@mymail.pomona.edu
Kaito Robson, kara2020@mymail.pomona.edu
12/7/2023
APDS-9960 I2C communication, not functional as of 12/7/23
*/

#include "stm32l4xx.h"
#include <stdio.h>

// registers as defined in APDS-9960 datasheet
#define APDS9960_I2C_ADDR       0x39
#define APDS9960_ENABLE         0x80
#define APDS9960_GSTATUS        0xAF
#define APDS9960_GFLVL          0xAE


#define APDS9960_GFIFO_U        0xFC
#define APDS9960_GFIFO_D        0xFD
#define APDS9960_GFIFO_L        0xFE
#define APDS9960_GFIFO_R        0xFF


#define APDS9960_CONTROL        0x8F
#define APDS9960_GPENTH         0xA0
#define APDS9960_GEXTH          0xA1
#define APDS9960_GCONF2         0xA3
#define APDS9960_GPULSE         0xA6
#define APDS9960_PPULSE         0x8E

#define GESTURES_START          0x01
#define GESTURES_STOP           0x02

#define DEFAULT_GPENTH          40     
#define DEFAULT_GEXTH           30      
#define DEFAULT_PGAIN           8 			
#define DEFAULT_GGAIN           0x40		
#define DEFAULT_PULSE_LENGTH    0x89    
#define APDS9960_PON            0x01
#define APDS9960_AEN            0x02
#define APDS9960_PEN            0x04
#define APDS9960_WEN            0x08
#define APSD9960_AIEN           0x10
#define APDS9960_PIEN           0x20
#define APDS9960_GEN            0x40
#define APDS9960_GVALID         0x01



#define APDS9960_I2C_ADDR 0x39

#define APDS9960_ENABLE 0x80
#define APDS9960_GPENTH 0xA0
#define APDS9960_GEXTH 0xA1
#define APDS9960_GCONF1 0xA2
#define APDS9960_GCONF2 0xA3
#define APDS9960_GCONF4 0xAB
#define APDS9960_GPULSE 0xA6
#define APDS9960_GOFFSET_U 0xA4
#define APDS9960_GOFFSET_D 0xA5
#define APDS9960_GOFFSET_L 0xA7
#define APDS9960_GOFFSET_R 0xA9
#define APDS9960_CONFIG1 0x8D
#define APDS9960_CONFIG2 0x90

// Bit fields
#define APDS9960_PON 0x01
#define APDS9960_GEN 0x40
#define APDS9960_GVALID 0x01

// Gesture control settings
#define DEFAULT_GPENTH 40
#define DEFAULT_GEXTH 30
#define DEFAULT_GFIFOTH 0x02 
#define DEFAULT_GEXMSK 0x00
#define DEFAULT_GEXPERS 0x02
#define DEFAULT_GGAIN 0x02 
#define DEFAULT_GLDRIVE 0x02 
#define DEFAULT_GWTIME 0x06
#define DEFAULT_GPLEN 0x02 
#define DEFAULT_GPULSE 0x89 
#define DEFAULT_GDIMENSIONS 0x01 
#define DEFAULT_GOFFSET 0x00 
#define DEFAULT_LEDBOOST 0x01 


int GestUpDown = 0;
int GestLeftRight = 0;


uint8_t i2c1_read(uint8_t addr);
void i2c1_write(uint8_t addr, uint8_t data);
void I2C1_init(void)
{
    // Enable clocks for I2C1 and GPIOB

    //set apb1 prescaler to 8 (so pclk is 8 Mhz)
    RCC->CFGR |= _VAL2FLD(RCC_CFGR_PPRE1, 0b110);

    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;


    // Configure GPIO pins for I2C1
    GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7); // Clear mode bits
    GPIOB->MODER |= (GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1); // Set to Alternate Function
    GPIOB->OSPEEDR |= (3 << 12) | (3 << 14); // high speed for pins b6, b7
    GPIOB->PUPDR |= (1 << 12) | (1 << 14); // set up pull internal pull ups
    GPIOB->OTYPER |= (GPIO_OTYPER_OT6); // Set to Open-Drain
    GPIOB->OTYPER |= (GPIO_OTYPER_OT7); // Set to Open-Drain
      // set to AF04 for I2C alternate functions
    GPIOB->AFR[0] |= (0b0100 << GPIO_AFRL_AFSEL6_Pos);  // SCL to af4 (PB6)
    GPIOB->AFR[0] |= (0b0100 << GPIO_AFRL_AFSEL7_Pos); // SDA to af4  (PB7)
    

    I2C1->CR1 &= ~(I2C_CR1_PE); // disable first
    I2C1->CR1 &= ~(I2C_CR1_NOSTRETCH);
    I2C1->CR1 |= I2C_CR1_PE; // Enable I2C1


    I2C1->TIMINGR &= ~(I2C_TIMINGR_PRESC_Msk); 
    I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLDEL_Msk); 
    I2C1->TIMINGR &= ~(I2C_TIMINGR_SDADEL_Msk);
    I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLH_Msk);
    I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLL_Msk);


    // I2C1 initialization
    I2C1->TIMINGR |= (3 << I2C_TIMINGR_PRESC_Pos); 
    I2C1->TIMINGR |= (0x4 << I2C_TIMINGR_SCLDEL_Pos);
    I2C1->TIMINGR |= (0x2 << I2C_TIMINGR_SDADEL_Pos);
    I2C1->TIMINGR |= (0xF << I2C_TIMINGR_SCLH_Pos);
    I2C1->TIMINGR |= (0x13 << I2C_TIMINGR_SCLL_Pos);
    I2C1->CR2 &= ~I2C_CR2_ADD10; // 7-bit addressing mode

 
    if (!(RCC->APB1ENR1 & RCC_APB1ENR1_I2C1EN)) {
         printf("i2c clock not enabled\n"); // Handle error: I2C clock not enabled
     }

    
    if (!(I2C1->CR1 & I2C_CR1_PE)) {
       printf("i2c1 not enabled!\n"); // Handle error: I2C1 not enabled
    }

    // Check if the I2C bus is busy
    if (I2C1->ISR & I2C_ISR_BUSY) {
        printf("i2c bus is busy!\n"); // Handle error: I2C bus is busy
    }
}

//-----------------------------------------------------------------------

void APDS9960_init(void) {
        printf("sensor init 0\n");	
	i2c1_write(APDS9960_CONTROL, DEFAULT_PGAIN);
        printf("sensor init 1\n");		
	i2c1_write(APDS9960_GPENTH, DEFAULT_GPENTH);
        printf("sensor init 2\n");	
	i2c1_write(APDS9960_GEXTH, DEFAULT_GEXTH);
        printf("sensor init 3\n");	
	i2c1_write(APDS9960_GCONF2, DEFAULT_GGAIN);
        printf("sensor init 4\n");	
	i2c1_write(APDS9960_GPULSE, DEFAULT_PULSE_LENGTH);
        printf("sensor init 5\n");	
	i2c1_write(APDS9960_PPULSE, DEFAULT_PULSE_LENGTH);
        printf("sensor init 6\n");	
	
}

//-----------------------------------------------------------------------

uint8_t i2c1_read(uint8_t addr)
{
    uint8_t data;
    printf("read 0\n");
    // Start the I2C communication
    I2C1->CR2 = (APDS9960_I2C_ADDR << 1) | (1 << 16); // Set slave address and number of bytes to read
    printf("read 1\n");
    I2C1->CR2 |= I2C_CR2_START; // Generate start condition
    printf("read 3\n");

    // Wait for the start condition to be sent
    while ((I2C1->ISR & I2C_ISR_TXIS) == 0);
    printf("read 4\n");

    // Send the address of the register to read
    I2C1->TXDR = addr;
    printf("read 5\n");

    // Wait for the transfer to complete
    while ((I2C1->ISR & I2C_ISR_TC) == 0);
    printf("read 6\n");

    // Generate restart and send slave address for read operation
    I2C1->CR2 = (APDS9960_I2C_ADDR << 1) | I2C_CR2_RD_WRN | (1 << 16);
    printf("read 7\n");
    I2C1->CR2 |= I2C_CR2_START;
    printf("read 8\n");

    // Wait for the data to be received
    while ((I2C1->ISR & I2C_ISR_RXNE) == 0);
    printf("read 9\n");

    // Read the data
    data = I2C1->RXDR;
    printf("read 10\n");

    // Generate stop condition
    I2C1->CR2 |= I2C_CR2_STOP;
    printf("read 0\n");

    return data;
}


void i2c1_write(uint8_t addr, uint8_t data) {
    while(I2C1->CR2 & I2C_CR2_START_Msk);
    // Start the I2C communication
    
    
    //old - I2C1->CR2 = (APDS9960_I2C_ADDR << 1) | (1 << 16); // Set slave address and number of bytes to write
    
     // set nbytes to 1
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_NBYTES, 1);

   // put address in SADD[7:1]
  I2C1->CR2 &= ~(I2C_CR2_SADD);
  I2C1->CR2 |= _VAL2FLD(I2C_CR2_SADD, (APDS9960_I2C_ADDR << 1));
// write transfer
 I2C1->CR2 &= ~(I2C_CR2_RD_WRN); 

  // wait for tx buffer to clear
  while(!(I2C1->ISR & I2C_ISR_TXE_Msk));
  printf("tx buffer clear\n");


    I2C1->CR2 |= I2C_CR2_START; // Generate start condition

    // Wait for the start condition to be sent or an error to occur
  //  while (!(I2C1->ISR & I2C_ISR_TXIS) && !(I2C1->ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_OVR | I2C_ISR_TIMEOUT)));

/* Past attempts at checking for error flags
    if (I2C1->ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_OVR | I2C_ISR_TIMEOUT)) {
        Handle errors here
        printf("error flags!\n");
        // Clear error flags
        I2C1->ICR |= I2C_ICR_BERRCF | I2C_ICR_ARLOCF | I2C_ICR_NACKCF | I2C_ICR_OVRCF | I2C_ICR_TIMOUTCF;
        } else {
        printf("no error flags\n");
        }
        
 */
    // Send the address of the register to write
    I2C1->TXDR = addr;
    printf("address sent\n");
    // Wait for the address byte to be transmitted or an error to occur
    //while (!(I2C1->ISR & I2C_ISR_TXIS) && !(I2C1->ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_OVR | I2C_ISR_TIMEOUT)));

    // wait for tx buffer to clear
    while(!(I2C1->ISR & I2C_ISR_TXE_Msk));

    // Send the data
    I2C1->TXDR = data;
    printf("data sent\n");

    // Wait for the transfer to complete or an error to occur
    // while (!(I2C1->ISR & I2C_ISR_TC) && !(I2C1->ISR & (I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_OVR | I2C_ISR_TIMEOUT)));

    // Check for error flags again, similar to above

    // Generate stop condition
    I2C1->CR2 |= I2C_CR2_STOP;
}

// Gesture start initilization sequence, per datasheet
void GesturesSet(uint8_t GestSel) {
	
	switch (GestSel)  
	{  
		case GESTURES_START:
			i2c1_write(APDS9960_ENABLE, APDS9960_GEN | APDS9960_PEN | APDS9960_PON);
			break;
		case GESTURES_STOP:
			i2c1_write(APDS9960_ENABLE, APDS9960_PEN | APDS9960_PON);
			break;  
		default:
			i2c1_write(APDS9960_ENABLE, APDS9960_GEN | APDS9960_PEN | APDS9960_PON);			
	}
	
}


int main() {
    // Initialize peripherals
    I2C1_init();   // Initialize I2C for sensor communication
    printf("i2c init\n");
    
    APDS9960_init(); // Initialize the APDS9960 sensor
    printf("APDS 9960 init\n");

    while (1) {
        uint8_t GFLVL_buf = 0;
        uint8_t GSTATUS_buf = 0;

        uint8_t GestureUp = 0;
        uint8_t GestureDown = 0;
        uint8_t GestureLeft = 0;
        uint8_t GestureRight = 0;

        GestUpDown = 0;
        GestLeftRight = 0;

        GesturesSet(GESTURES_START); // Start gesture detection

        GSTATUS_buf = i2c1_read(APDS9960_GSTATUS);

        if (GSTATUS_buf & APDS9960_GVALID) {
            GFLVL_buf = i2c1_read(APDS9960_GFLVL);

            if (GFLVL_buf) {
                GestureUp = i2c1_read(APDS9960_GFIFO_U);
                GestureDown = i2c1_read(APDS9960_GFIFO_D);
                GestureLeft = i2c1_read(APDS9960_GFIFO_L);
                GestureRight = i2c1_read(APDS9960_GFIFO_R);

                GestUpDown = GestureUp - GestureDown;
                GestLeftRight = GestureLeft - GestureRight;

                // Determine and send the gesture over UART
                if (GestUpDown > 0 && GestLeftRight > 0) {
                    printf("Gesture: UP\n");
                } else if (GestUpDown < 0 && GestLeftRight < 0) {
                    printf("Gesture: DOWN\n");
                } else if (GestUpDown > 0 && GestLeftRight < 0) {
                    printf("Gesture: LEFT\n");
                } else if (GestUpDown < 0 && GestLeftRight > 0) {
                    printf("Gesture: RIGHT\n");
                } else {
                    printf("Gesture: UNKNOWN\n");
                }

                GesturesSet(GESTURES_STOP); // Stop gesture detection
            }
        }

        
    }
}
