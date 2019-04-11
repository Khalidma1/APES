/*
 * Alert_Task.c
 *
 *  Created on: Apr 11, 2019
 *      Author: Khalid AlAwadhi
 */


#include "Alert_Task.h"

//Standard includes
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//TivaWare includes
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"

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



uint8_t Alert_TaskInit()
{
    //Create task, if it fails return 1, else 0
    if( xTaskCreate(Alert_Task,						/* The function that implements the task */
			   (const portCHAR *)"Alert",			/* The text name assigned to the task - for debug only as it is not used by the kernel */
			   ((configMINIMAL_STACK_SIZE) * 1),	/* The size of the stack to allocate to the task */
			   NULL,								/* The parameter passed to the task */
			   PRIORITY_ALERT_TASK, 				/* The priority assigned to the task */
			   NULL)								/* The task handle is not required, so NULL is passed */
    			!= pdTRUE
      )
      {
    	return 1;
      }

	return 0;
}



void Alert_Task(void *pvParameters)
{
	/* Init PIN PM5 to be an INPUT for the Alert PIN coming from the sensor */
	Init_Alert_PIN_Polling();

	while(1)
	{
		if( Alert_PIN_Poll() )
		{
			/* Create Msg to send */
			struct MsgStruct TXMessage;
			TXMessage.Source = Alert;
			strcpy(TXMessage.Msg, "Alert! - Temp is above set Threshold!");

			/* Send Msg to Logger Task */
			xQueueSend( xQueue_Logger, &TXMessage, ( TickType_t ) 0 );
		}
	}
}
