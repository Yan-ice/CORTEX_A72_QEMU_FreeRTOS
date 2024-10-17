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

#include "uart.h"
#include "gic_v3.h"

/* Run a simple demo just prints 'Blink' */
#define DEMO_BLINKY               1

extern void FreeRTOS_Tick_Handler( void );

void vConfigureTickInterrupt( void )
{
    vSendString("[Tick INTR enable]\n");
    int clock_rate;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (clock_rate) );    // 设置计数器的加载值（例如1秒）
    int cnt = clock_rate / configTICK_RATE_HZ;

    __asm__ volatile ("msr cntv_tval_el0, %0" :: "r" (cnt));
    // 设置计数器的加载值（例如1秒）

    __asm__ volatile ("mrs x27, CNTV_CTL_EL0 \n\t MOV x26, #0x1 \n\t ORR x27, x27, x26 \n\t msr CNTV_CTL_EL0, x27");
    // 设置 ENABLE 位(1), 启用 Local Timer

	return;
}

//this func is actually reset timer rather than disable timer.
void vClearTickInterrupt( void )
{
    int clock_rate;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (clock_rate) );    // 设置计数器的加载值（例如1秒）
    int cnt = clock_rate / configTICK_RATE_HZ;

    __asm__ volatile ("msr cntv_tval_el0, %0" :: "r" (cnt));
    // 设置计数器的加载值（例如1秒）
}

void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName );
void vApplicationTickHook( void );

int main_blinky( void );

/*-----------------------------------------------------------*/

int FreeRTOS_Error_Handler (uint64_t* ctx, int a, int b) {
    portDISABLE_INTERRUPTS();
    vSendStringISR("INVALID interrupt handled.\n");
    printHexISR(a);
    printHexISR(b);
    printHexISR((uint64_t)ctx); //sp
    printHexISR(ctx[31]); //sp_el0
    printHexISR(ctx[32]); //sepc
    while(1);
    return 0;
}

int FreeRTOS_SYNC_Handler (uint64_t* ctx) {
    portDISABLE_INTERRUPTS();
    //portENTER_CRITICAL();
    vSendStringISR("SYNC interrupt handled.\n");
    //printHex((uint64_t)ctx); //sp
    //printHex(ctx[31]); //sp_el0
    //printHex(ctx[32]); //sepc
    while(1);
    //portEXIT_CRITICAL();
    return 0;
}

// struct QueueDefinition; /* Using old naming convention so as not to break kernel aware debuggers. */
// typedef struct QueueDefinition   * QueueHandle_t;

// typedef struct QUEUE_REGISTRY_ITEM
//     {
//         const char * pcQueueName;
//         QueueHandle_t xHandle;
//     } xQueueRegistryItem;

// /* The old xQueueRegistryItem name is maintained above then typedefed to the
//  * new xQueueRegistryItem name below to enable the use of older kernel aware
//  * debuggers. */
// typedef xQueueRegistryItem QueueRegistryItem_t;

// extern int* pxCurrentTCB;
// PRIVILEGED_DATA QueueRegistryItem_t xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

int main( void )
{
    int ret;
    gic_v3_initialize();

    uartinit();
    vSendString("uart init success.\n");

    __asm__ volatile ("mrs %0, s3_1_c15_c3_0" : "=r"(ret));
    vSendString("gic base CBAR:");
    printHex(ret);

    #if defined( DEMO_BLINKY )
        ret = main_blinky();
    #else
    #error "Please add or select demo."
    #endif

    return ret;
}


/*-----------------------------------------------------------*/

void vApplicationGetTimerTaskMemory( 
    StaticTask_t **ppxTimerTaskTCBBuffer,
    StackType_t **ppxTimerTaskStackBuffer,
    uint32_t *pulTimerTaskStackSize )
{
    // 提供定时器任务的 TCB 缓冲区
    static StaticTask_t xTimerTaskTCB;
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    // 提供定时器任务的堆栈缓冲区
    static StackType_t xTimerTaskStack[configTIMER_TASK_STACK_DEPTH];
    *ppxTimerTaskStackBuffer = xTimerTaskStack;

    // 定义定时器任务堆栈的大小
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*-----------------------------------------------------------*/

/* 定义空闲任务所需的静态内存 */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    configSTACK_DEPTH_TYPE *pulIdleTaskStackSize )
{
    /* 提供用于空闲任务的静态内存 */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* vApplicationMallocFailedHook() will only be called if
     * configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
     * function that will get called if a call to pvPortMalloc() fails.
     * pvPortMalloc() is called internally by the kernel whenever a task, queue,
     * timer or semaphore is created.  It is also called by various parts of the
     * demo application.  If heap_1.c or heap_2.c are used, then the size of the
     * heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
     * FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
     * to query the size of free heap space that remains (although it does not
     * provide information on how the remaining heap might be fragmented). */
    taskDISABLE_INTERRUPTS();
    vSendStringISR("[MallocFailed Hook]\n");
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
     * to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
     * task.  It is essential that code added to this hook function never attempts
     * to block in any way (for example, call xQueueReceive() with a block time
     * specified, or call vTaskDelay()).  If the application makes use of the
     * vTaskDelete() API function (as this demo application does) then it is also
     * important that vApplicationIdleHook() is permitted to return to its calling
     * function, because it is the responsibility of the idle task to clean up
     * memory allocated by the kernel to any task that has since been deleted. */
    vSendStringISR("[ApplicationIdle Hook]\n");
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask,
                                    char * pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
     * configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
     * function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();

    vSendStringISR("[Overflow Hook]\n");
    for( ; ; )
    {
    }
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
    //vSendStringISR("[Tick Hook]\n");
}

void vTaskSwitchHook(TaskHandle_t pxTCB) {
    //vSendStringISR("Task switched to: ");
    //vSendStringISR(pxTCB->pcTaskName);
    //vSendStringISR("\n");
}
/*-----------------------------------------------------------*/

void vAssertCalled( void )
{
    volatile uint32_t ulSetTo1ToExitFunction = 0;

    //taskDISABLE_INTERRUPTS();

    vPortEnterCritical();
    while( ulSetTo1ToExitFunction != 1 )
    {
        vSendStringISR("[Assert] Exit.\n");
        __asm volatile ( "NOP" );
        while(1);
    }
}


/*-----------------------------------------------------------*/

void vRegisterIRQHandler( uint32_t ulID,
                          uintptr_t pxHandlerFunction,
                          void * pvContext )
{
    vSendStringISR("[vRegisterIRQHandler]\n");
    return;
}
/*-----------------------------------------------------------*/

void vApplicationIRQHandler( uint32_t ulICCIAR )
{
    //printHexISR(ulICCIAR);
    //vSendStringISR("[vApplicationIRQHandler]\n");

    return;
}
