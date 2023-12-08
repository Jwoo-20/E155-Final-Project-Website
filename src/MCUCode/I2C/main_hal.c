/* USER CODE BEGIN Header */
/*
James Woo, jmwa2020@mymail.pomona.edu
Kaito Robson, kara2020@mymail.pomona.edu
12/7/2023
I2C with APDS-9960 with HAL
*/

/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define APDS9960_ENABLE         0x80
#define APDS9960_GSTATUS        0xAF
#define APDS9960_GFLVL          0xAE

//Gesture FIFO Register (0xFC – 0xFF):
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

#define DEFAULT_GPENTH          40      // Threshold for entering gesture mode
#define DEFAULT_GEXTH           30      // Threshold for exiting gesture mode    
#define DEFAULT_PGAIN           8 			// Proximity Gain Control: 4X 
#define DEFAULT_GGAIN           0x40		// Gesture Gain Control: 4X

#define DEFAULT_PULSE_LENGTH    0x89    // 16us, 10 pulses

/* Bit fields */
#define APDS9960_PON            0x01
#define APDS9960_AEN            0x02
#define APDS9960_PEN            0x04
#define APDS9960_WEN            0x08
#define APSD9960_AIEN           0x10
#define APDS9960_PIEN           0x20
#define APDS9960_GEN            0x40
#define APDS9960_GVALID         0x01

#define APDS9960_ID_REG         0x92

#define APDS9960_EXPECTED_ID    0xAB 

// APDS9960 I2C address
//#define APDS9960_I2C_ADDR (0x39 << 1) // Assuming 7-bit address is shifted for HAL functions
#define APDS9960_I2C_ADDR 0x40

// APDS9960 Registers


#define APDS9960_GCONFIG2 0xA3



// Configuration values

#define DEFAULT_GLDRIVE 0x00 // LED Drive Strength (adjust as needed)


int GestUpDown = 0;
int GestLeftRight = 0;

/* Function prototypes */
void APDS9960_Init(I2C_HandleTypeDef *hi2c);
uint8_t APDS9960_ReadGesture(I2C_HandleTypeDef *hi2c);

void APDS9960_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t data;

    printf("sensor init 0\n");	
    data = APDS9960_PON;
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_ENABLE, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 1\n");
    data = DEFAULT_GPENTH;
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_GPENTH, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 2\n");
    data = DEFAULT_GEXTH;
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_GEXTH, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 3\n");
    data = (uint8_t)((DEFAULT_GGAIN << 5) | (DEFAULT_GLDRIVE << 3)); // Adjust bit positions as needed
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_GCONFIG2, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 4\n");
    data = DEFAULT_PULSE_LENGTH;
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_GPULSE, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 5\n");
    data = APDS9960_GEN | APDS9960_PEN | APDS9960_PON;
    HAL_I2C_Mem_Write(hi2c, APDS9960_I2C_ADDR, APDS9960_ENABLE, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

    printf("sensor init 6\n");
}
uint8_t APDS9960_ReadGesture(I2C_HandleTypeDef *hi2c) {
    uint8_t gstatus;
    uint8_t data[4];
    uint8_t gesture = 0; // Default: No gesture

    // Check if gesture data is available
    HAL_I2C_Mem_Read(hi2c, APDS9960_I2C_ADDR, APDS9960_GSTATUS, I2C_MEMADD_SIZE_8BIT, &gstatus, 1, HAL_MAX_DELAY);

    if (gstatus & APDS9960_GVALID) {
        // Read gesture data
        HAL_I2C_Mem_Read(hi2c, APDS9960_I2C_ADDR, APDS9960_GFIFO_U, I2C_MEMADD_SIZE_8BIT, data, 4, HAL_MAX_DELAY);
      
        // Simple gesture processing
        if (data[0] > data[1]) {
            gesture = 1; // Gesture UP
        } else if (data[0] < data[1]) {
            gesture = 2; // Gesture DOWN
        } else if (data[2] > data[3]) {
            gesture = 3; // Gesture LEFT
        } else if (data[2] < data[3]) {
            gesture = 4; // Gesture RIGHT
        }
    }

    return gesture;
}
uint8_t Test_I2C_Communication(I2C_HandleTypeDef *hi2c) {
    uint8_t device_id = 0;

    // Read the device ID register
    HAL_I2C_Mem_Read(hi2c, APDS9960_I2C_ADDR, APDS9960_ID_REG, I2C_MEMADD_SIZE_8BIT, &device_id, 1, HAL_MAX_DELAY);

    // Compare the read value with the expected value
    if (device_id == APDS9960_EXPECTED_ID) {
        printf("I2C Communication OK. Device ID: 0x%X\n", device_id);
        return 1; // Success
    } else {
        printf("I2C Communication Failed. Read ID: 0x%X\n", device_id);
        return 0; // Failure
    }
}

void Read_Multiple_Registers(I2C_HandleTypeDef *hi2c, uint8_t start_reg, uint8_t end_reg) {
    uint8_t reg_value = 0;

    for (uint8_t reg = start_reg; reg <= end_reg; ++reg) {
        HAL_Delay(250);
        // Read the current register
        if (HAL_I2C_Mem_Read(hi2c, APDS9960_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &reg_value, 1, HAL_MAX_DELAY) == HAL_OK) {
            // Print the value of the register
            printf("Register 0x%X: 0x%X\n", reg, reg_value);
        } else {
            // Handle communication error
            printf("Error reading register 0x%X, Read = 0x%X\n", reg, reg_value);
        }
    }
}



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
uint8_t buf[12];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  Test_I2C_Communication(&hi2c1);

  /* USER CODE BEGIN 2 */
      // Define the range of registers to read
    uint8_t start_register = 0x00; // Start address of the register range
    uint8_t end_register = 0xFF;   // End address of the register range

    // Read and print values from the specified range of registers
  // Read_Multiple_Registers(&hi2c1, start_register, end_register);

  //APDS9960_Init(&hi2c1);

}

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /*
while (1) {
        // Read and process gestures
        uint8_t gesture = APDS9960_ReadGesture(&hi2c1);
        printf("hello!\n");
        printf("gesture = %d\n", gesture);
        if (gesture != 0) {
        
            // Gesture detected, process accordingly
            char msg[30];
            sprintf(msg, "Gesture detected: %d\r\n", gesture);
            
            HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
        }

        HAL_Delay(100); // Delay to avoid too frequent polling
    }
}
*/






    /* USER CODE END WHILE */

/*
strcpy((char*)buf, "Hello!\r\n");
HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
HAL_Delay(500);

*/





    /* USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00907CB8;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
