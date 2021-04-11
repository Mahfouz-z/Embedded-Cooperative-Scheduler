#include "main.h"
#include "application_1.h"
#include <stdio.h>
#include "co_sched_API.h"

extern I2C_HandleTypeDef hi2c3;
extern UART_HandleTypeDef huart1;
float temp = 0;
int alarm_flag = 0;
float critical_temp = 30;


char uart1_rec_buffer[10];
int rec_buf_i = 0; 
int len_temp = 0;


void receive_uart(void)
{
		char failure_message[] = {'T','E','M', 'P',' ','U','P','D','A','T', 'E',' ','F','A','I','L','E','D','\r','\n'};
		uint8_t data_byte = NULL;
		HAL_UART_Receive (&huart1, &data_byte, sizeof(data_byte), 1000);
		
		uart1_rec_buffer[rec_buf_i] = data_byte;
		rec_buf_i++;
		if(rec_buf_i == 10)
		{
				rec_buf_i = 0; // out of memory bounds, rewrite data
				len_temp = 0;
				HAL_UART_Transmit(&huart1, (unsigned char *)failure_message, sizeof(failure_message), HAL_MAX_DELAY);
		}
		if(data_byte == '\n') // received a full temprature value 
		{
				len_temp = rec_buf_i - 2;
				rec_buf_i = 0;
				QueTask(update_critical_temprature, 1);
		}
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
}
void update_critical_temprature(void)
{
		char success_message[] = {'T','E','M', 'P',' ','U','P','D','A','T', 'E',' ','S','U','C','C','E','S','S','\r','\n'};
		char confirm_temp_message[] = {'C','R','I','T','I','C','A','L',' ','T','E','M','P',':'};
		char endl[] = {'\r', '\n'};
		char value_buffer[6];
		int i;
		
		i = 0;
		while(uart1_rec_buffer[i] != '\r' && i < 6)
		{
				value_buffer[i] =  uart1_rec_buffer[i];
				i++;
		}
		while (i < 6)
		{
				value_buffer[i]=0;
				i++;
		}
		sscanf(value_buffer,"%f",&critical_temp);
		if(temp >= critical_temp && alarm_flag == 0) 
		{
				alarm_flag = 1;
				QueTask(alarm_led_toggle_task, 2);
		}
		else if(temp < critical_temp && alarm_flag == 1) alarm_flag = 0;		
		HAL_UART_Transmit(&huart1, (unsigned char *)success_message, sizeof(success_message), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (unsigned char *)confirm_temp_message, sizeof(confirm_temp_message), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (unsigned char *)value_buffer, sizeof(value_buffer), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (unsigned char *)endl, sizeof(endl), HAL_MAX_DELAY);
}

void init_I2C_UART_task(void)
{
		// init UART receive
		__HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);
		//check that I2C device is ready to operate
		char success_message[] = {'I','2','C',' ','S','U','C','C','E','S','S','\r','\n'};
		char failure_message[] = {'I','2','C',' ','F','A','I','L','E','D','\r','\n'};
		if (HAL_I2C_IsDeviceReady(&hi2c3, 0xD0, 10, HAL_MAX_DELAY) == HAL_OK)
		{
				HAL_UART_Transmit(&huart1, (unsigned char *)success_message, sizeof(success_message), HAL_MAX_DELAY);
		}
		else
		{
				HAL_UART_Transmit(&huart1, (unsigned char *)failure_message, sizeof(failure_message), HAL_MAX_DELAY);
		}
}

void alarm_led_toggle_task(void)
{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
		if(alarm_flag == 1) 
		{
				ReRunMe(5);
		}
		else QueTask(alarm_led_off_task, 2);
}

void alarm_led_off_task(void)
{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
}

void update_temprature_task(void)
{
		float temp = read_temprature();
		char temp_message[] = {'C', 'U', 'R', 'R', ' ', 'T', 'E', 'M', 'P', ':'};
		char temp_holder[8];
		char endl[] = {'\r', '\n'};
		
		snprintf(temp_holder, sizeof(temp_holder), "%f", temp);
		
		HAL_UART_Transmit(&huart1, (unsigned char *)temp_message, sizeof(temp_message), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (unsigned char *)temp_holder, sizeof(temp_holder), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart1, (unsigned char *)endl, sizeof(endl), HAL_MAX_DELAY);
		
		if(temp >= critical_temp && alarm_flag == 0) 
		{
				alarm_flag = 1;
				QueTask(alarm_led_toggle_task, 2);
		}
		else if(temp < critical_temp && alarm_flag == 1) alarm_flag = 0;
			
		// ReRunMe(60); // rerun after 3 seconds
		ReRunMe(600); // rerun after 30 seconds 
}

float read_temprature(void)
{
		uint8_t temp_int_buffer[2], temp_float_buffer[2], ctrl_reg[2], status_reg[2];
		
		// force temprature calculation to read live temprature
		status_reg[0] = 0x0F; 	// Status Register Address	
		ctrl_reg[0] = 0x0E; 		// Control Register Address
		// read status register
		HAL_I2C_Mem_Read(&hi2c3, 0xD0, status_reg[0], 1, &status_reg[1], 1,10);

		// If the controller is not executing TCXO function, force it.
		if((status_reg[1]&0x4) == 0)
		{
				// read current status register to rewrite it
				HAL_I2C_Mem_Read(&hi2c3, 0xD0, ctrl_reg[0], 1, &ctrl_reg[1], 1,10);
				ctrl_reg[1] |= 0x20; // set the conv bit to force temprature conversion of temprature to digital value
				HAL_I2C_Mem_Write(&hi2c3, 0xD0, ctrl_reg[0], 1, &ctrl_reg[1], 1, 10);
		}

		//read temprature
		temp_int_buffer[0] = 0x11;    // Address of int part of temprature
		temp_float_buffer[0] = 0x12;	// Address of float part of temprature
		HAL_I2C_Mem_Read(&hi2c3, 0xD0, temp_int_buffer[0], 1, &temp_int_buffer[1], 1, 10);
		HAL_I2C_Mem_Read(&hi2c3, 0xD0, temp_float_buffer[0], 1, &temp_float_buffer[1], 1, 10);

		temp = temp_int_buffer[1]  + (temp_float_buffer[1]>>6) / 4.0; 
		HAL_Delay(1);
		return temp;
	
}	

