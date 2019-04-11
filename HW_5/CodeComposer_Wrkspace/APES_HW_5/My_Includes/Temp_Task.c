/*
 * Temp_Task.c
 *
 *  Created on: Apr 9, 2019
 *      Author: Khalid AlAwadhi
 */
#include "Temp_Task.h"

//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//TivaWare includes

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//My includes
#include "Global_Defines.h"
#include "Master_Functions.h"
#include "My_TMP102_Driver.h"

/* Global Variables */
extern QueueHandle_t xQueue_Logger;


uint8_t Temp_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(Temp_Task,						/* The function that implements the task */
			   (const portCHAR *)"Temp",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
			   ((configMINIMAL_STACK_SIZE) * 1),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_TEMP_TASK, 					/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void Temp_Task(void *pvParameters)
{
	/* Init I2C0 */
	Init_I2C0();

	/* Set fault tolerance setting */
	TMP102_SetFault(0);					//Trigger alert immediately

	/* Set the polarity of the alert */
	TMP102_SetAlertPolarity(1);			//Active HIGH

	/* Set sensor in Interrupt Mode */
	TMP102_SetAlertMode(1);				//Interrupt Mode

	/* Set the Conversion Rate */
	TMP102_SetConversionRate(3);		//8Hz

	/* Set Extended mode */
	TMP102_SetExtendedMode(0);			//12-BIT (-55C to +128C)

	/* Set T_HIGH, the upper limit to trigger the alert on */
	TMP102_SetHighTempC(29.0);

	/* Set T_LOW, the lower limit to turn off the alert */
	TMP102_SetLowTempC(26.0);

	/* Wakeup sensor */
	TMP102_Wakeup();

	 /* Block for 1000ms (1Hz) */
	const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;


	while(1)
	{

		float TempInC = TMP102_ReadTempC();

		vTaskDelay(xDelay);

		/* Create Msg to send */
		struct MsgStruct TXMessage;
		TXMessage.Source = Temp;

		strcpy(TXMessage.Msg, "Current Temp in C: ");
		TXMessage.Value = TempInC;

		/* Send Msg to Logger Task */
		xQueueSend( xQueue_Logger, &TXMessage, ( TickType_t ) 0 );
	}
}
