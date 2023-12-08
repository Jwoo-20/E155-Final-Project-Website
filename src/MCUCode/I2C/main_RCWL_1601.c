/*
James Woo, jmwa2020@mymail.pomona.edu
Kaito Robson, kara2020@mymail.pomona.edu
12/7/2023
I2C with RCWL-1601
*/

#include "stm32l4xx.h"
#include <stdio.h>


// Initialize I2C1
void I2C1_Init(void) {
    // Disable the I2C1 peripheral
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Enable clock for GPIOB
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;

    // Set PB6 and PB7 to alternate function mode (I2C1 SCL and SDA)
    GPIOB->MODER &= ~(GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
    GPIOB->MODER |= (GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1);

    // Set output type to open-drain for PB6 and PB7
    GPIOB->OTYPER |= (GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);

    // Set low speed for PB6 and PB7
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7);

    // No pull-up, no pull-down for PB6 and PB7
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);

    // Set PB6 and PB7 to I2C1 alternate function (AF4)
    GPIOB->AFR[0] |= (0b0100 << (4 * 6)) | (0b0100 << (4 * 7)); // AF4 for pin 6 and 7

    // Enable clock for I2C1
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // Configure I2C1 timing for 100kHz
    // These values should be adjusted based on your specific clock settings
    I2C1->TIMINGR = (uint32_t)0x00907CB8; // Example timing for 100kHz

    // Enable the I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

// Generate I2C start condition
void I2C_Start(I2C_TypeDef *I2C, uint8_t address, uint8_t direction) {
    // Send START condition
    I2C->CR2 = 0;
    I2C->CR2 |= (address << 1); // Set address
    I2C->CR2 |= direction; // Set read or write direction
    I2C->CR2 |= I2C_CR2_START; // Generate start

    // Wait for start condition to be generated
    while (!(I2C->CR2 & I2C_CR2_START));
}

// Send a byte of data
void I2C_Write(I2C_TypeDef *I2C, uint8_t data) {
    // Send data
    I2C->TXDR = data;
    printf("data sent\n");
    // Wait until data is transferred
    while (!(I2C->ISR & I2C_ISR_TXE));
    printf("write done\n");
}

// Read a byte with ACK
uint8_t I2C_Read_ACK(I2C_TypeDef *I2C) {
    // Enable ACK
    I2C->CR2 &= ~I2C_CR2_NACK;

    // Wait until byte is received
    while (!(I2C->ISR & I2C_ISR_RXNE));

    // Return received byte
    return (uint8_t)I2C->RXDR;
}

// Read a byte with NACK
uint8_t I2C_Read_NACK(I2C_TypeDef *I2C) {
    // Disable ACK
    I2C->CR2 |= I2C_CR2_NACK;

    // Wait until byte is received
    while (!(I2C->ISR & I2C_ISR_RXNE));

    // Return received byte
    return (uint8_t)I2C->RXDR;
}

// Generate I2C stop condition
void I2C_Stop(I2C_TypeDef *I2C) {
    // Send STOP condition
    I2C->CR2 |= I2C_CR2_STOP;

    // Wait for stop condition to be generated
    while (I2C->CR2 & I2C_CR2_STOP);
}



int main(void) {
    // Initialize I2C1
    I2C1_Init();

    // Main loop
    while (1) {
        // Start communication with the sensor
        I2C_Start(I2C1, 0x57, 0); // 0x57 is the address, shifted left for write

        // Send '1' to start ranging
        I2C_Write(I2C1, 1);

        // Wait for a few milliseconds (adjust delay as needed)
        for (volatile int i = 0; i < 10000; i++); // Simple delay loop

        // Restart I2C for reading
        I2C_Start(I2C1, 0x57 << 1, 1); // 0x57 is the address, shifted left for read

        // Read 3 bytes for the distance
        uint8_t data1 = I2C_Read_ACK(I2C1);
        uint8_t data2 = I2C_Read_ACK(I2C1);
        uint8_t data3 = I2C_Read_NACK(I2C1);

        // Stop I2C communication
        I2C_Stop(I2C1);

        // Calculate distance
        int distance = (data1 << 16) | (data2 << 8) | data3;
        distance /= 1000; // Convert to mm

        // Output the distance 
        printf("Distance: %d mm\n", distance);

        // Add a delay or wait for a trigger before next measurement
        for (volatile int i = 0; i < 50000; i++); // Simple delay loop
    }
}