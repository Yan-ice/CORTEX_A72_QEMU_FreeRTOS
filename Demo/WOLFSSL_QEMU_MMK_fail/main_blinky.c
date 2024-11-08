/*
 * FreeRTOS V202212.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include "aarch64.h"


/* Priorities used by the tasks. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY    ( tskIDLE_PRIORITY + 2 )
#define mainQUEUE_SEND_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

/* The rate at which data is sent to the queue.  The 200ms value is converted
 * to ticks using the pdMS_TO_TICKS() macro. */
#define mainQUEUE_SEND_FREQUENCY_MS        pdMS_TO_TICKS( 1000 )

/* The maximum number items the queue can hold.  The priority of the receiving
 * task is above the priority of the sending task, so the receiving task will
 * preempt the sending task and remove the queue items each time the sending task
 * writes to the queue.  Therefore the queue will never have more than one item in
 * it at any time, and even with a queue length of 1, the sending task will never
 * find the queue full. */
#define mainQUEUE_LENGTH                   ( 2 )

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/*-----------------------------------------------------------*/

static void prvQueueSendTask( void * pvParameters )
{
    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;
    int f = 1;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();
    
    // WOLFSSL_CTX *ctx;
    // WOLFSSL *ssl;
    // char buf[256];
    
    // // 初始化 wolfSSL
    // wolfSSL_Init();
    
    // // 创建 SSL_CTX 对象
    // ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    // if (ctx == NULL) {
    //     perror("wolfSSL_CTX_new failed");
    //     exit(EXIT_FAILURE);
    // }
    
    for( ; ; )
    {
        char buf[ 40 ];

        //sprintf( buf, "%d: %s: %s", cpuid(),
        //         pcTaskGetName( xTaskGetCurrentTaskHandle() ),
        //         ( f ) ? pcMessage1 : pcMessage2 );
        //vSendString( buf );
        printf("send: %lx\n", ulValueToSend);

        f = !f;
        /* Send to the queue - causing the queue receive task to unblock and
         * toggle the LED.  0 is used as the block time so the sending operation
         * will not block - it shouldn't need to block as the queue should always
         * be empty at this point in the code. */
        xQueueSend( xQueue, &ulValueToSend, 0U );

        /* Place this task in the blocked state until it is time to run again. */
        vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

    }
}


/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void * pvParameters )
{
    unsigned long ulReceivedValue;
    const unsigned long ulExpectedValue = 100UL;
    int f = 1;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    for( ; ; )
    {
        char buf[ 40 ];

        /* Wait until something arrives in the queue - this task will block
         * indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
         * FreeRTOSConfig.h. */
        xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );
        /*  To get here something must have been received from the queue, but
         * is it the expected value?  If it is, toggle the LED. */
        if( ulReceivedValue == ulExpectedValue )
        {
            // sprintf( buf, "%d: %s: %s", cpuid(),
            //          pcTaskGetName( xTaskGetCurrentTaskHandle() ),
            //          ( f ) ? pcMessage1 : pcMessage2 );
            // vSendString( buf );
            printf("receive: %lx\n", ulReceivedValue);
            f = !f;

            ulReceivedValue = 0U;
        }
        else
        {
            printf("unexpected: %lx\n", ulReceivedValue);
            ulReceivedValue = 0U;
        }
    }
}

/*-----------------------------------------------------------*/

static void prvGetStats(void *pvParameters) {
    TickType_t xNextWakeTime;
    xNextWakeTime = xTaskGetTickCount();
    printf("Stats waiting since %lx.\n", xNextWakeTime);
    vTaskDelayUntil( &xNextWakeTime, pdMS_TO_TICKS(6000) );
    printf("Stats wakeup.\n");
    char pcStatsBuffer[512];
    vTaskGetRunTimeStats(pcStatsBuffer);
    
    vSendString(pcStatsBuffer);
}
/*-----------------------------------------------------------*/

int main_blinky( void )
{
    vSendString( "Hello FreeRTOS! \n" );

    /* Create the queue. */
    xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint32_t ) );
    vSendString( "Queue create finish\n" );

    // TickType_t xNextWakeTime;
    // xNextWakeTime = xTaskGetTickCount();
    // vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );
    // vSendString( "Task delay finish\n" );
    if( xQueue != NULL )
    {
        /* Start the two tasks as described in the comments at the top of this
         * file. */
        xTaskCreate( prvQueueReceiveTask, "Rx", configMINIMAL_STACK_SIZE * 2U, NULL,
                     mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
        xTaskCreate( prvQueueSendTask, "Tx", configMINIMAL_STACK_SIZE * 2U, NULL,
                     mainQUEUE_SEND_TASK_PRIORITY, NULL );
        xTaskCreate( prvGetStats, "Stats", configMINIMAL_STACK_SIZE * 5U, NULL,
                     tskIDLE_PRIORITY+3, NULL );
    }

    vSendString( "Start Scheduler\n" );
    vTaskStartScheduler();
    

    return 0;
}

