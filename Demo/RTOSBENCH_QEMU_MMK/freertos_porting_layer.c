#include <string.h>
#include "porting_layer.h"

#include "gic_v3.h"
#include "uart.h"
#include "my_stdlib.h"

#ifdef TRACING
#include "trace.h"
#endif

volatile unsigned int* coreTimer = (volatile unsigned int*)0x40000000;
volatile unsigned int* coreTimerPrescaler = (volatile unsigned int*)0x40000008;
volatile unsigned int* coreTimerLow = (volatile unsigned int*)0x4000001C;
volatile unsigned int* coreTimerHigh = (volatile unsigned int*)0x40000020;

volatile unsigned int* systemTimer = (volatile unsigned int*)0x3F003004;

volatile unsigned int* coreMailboxInterruptCtrl = (volatile unsigned int*)0x40000050;
volatile unsigned int* coreMailboxInterrupt = (volatile unsigned int*)0x40000080;
volatile unsigned int* coreMailboxInterruptClr = (volatile unsigned int*)0x400000C0;

char printfbuf[100];

static int task_ids = 100;

void no_initialize_test(no_task_entry_t init_test_fct)
{
	unsigned int ra;

	//rpi_cpu_irq_disable();
	portDISABLE_INTERRUPTS();

#ifdef TRACING
	clear_trace_buffer();
	start_trace();
#endif

	init_test_fct(NULL);

	vTaskStartScheduler();
}

no_task_handle_t no_create_task(no_task_entry_t task_entry, char task_name[4], unsigned int prio)
{
	no_task_handle_t thandle;
	xTaskCreate(task_entry, (const char* const)task_name, 128, NULL,
			prio, &thandle);

	return thandle;
}

void no_task_yield()
{
	taskYIELD();
}

void no_task_suspend(no_task_handle_t task)
{
	vTaskSuspend(task);
}

void no_task_suspend_self()
{
	vTaskSuspend(NULL);
}

void no_task_resume(no_task_handle_t task)
{
	vTaskResume(task);
}

void no_task_delay(unsigned int milliseconds)
{
	vTaskDelay(milliseconds);
}

void no_init_timer()
{
	/* Clock timer on APB clock (Half arm speed, 40MHz (from experiment) ??) */
	*coreTimer = (1 << 7);

	/* This is equivalent to setting prescaler to 1 */
	*coreTimerPrescaler = 0x80000000;
	asm volatile ("wfe" : :);
}

void no_disable_timer()
{
}

void no_reset_timer()
{
}

no_time_t no_add_times(const no_time_t* base, unsigned int milliseconds)
{
	no_time_t retval = ((*base) + milliseconds * 900000);
	vSendString(printfbuf);
	return retval;
}

no_time_t no_time_get()
{
	unsigned int cc;
	asm volatile ("mrs %0, cntvct_el0" : "=r" (cc));
	return cc;
}

long no_time_diff(const no_time_t* t1, const no_time_t* t2)
{
	long diff = ((long)*t2 - (long)*t1);
	if (diff < 0)
	{
		diff = -diff;
	}
	return diff;
}

void no_sem_create(no_sem_t* sem, int current_value)
{
	*sem = xSemaphoreCreateCounting(50, current_value);
}

void no_sem_wait(no_sem_t* sem)
{
	if (xSemaphoreTake(*sem, portMAX_DELAY) != pdTRUE)
	{
		no_serial_write("sem_wait: error");
	}
}

void no_sem_signal(no_sem_t* sem)
{
	if (xSemaphoreGive(*sem) != pdTRUE)
	{
		no_serial_write("sem_signal: error");
	}
}

void no_mutex_create(no_mutex_t* mutex)
{
	*mutex = xSemaphoreCreateMutex();
}

void no_mutex_acquire(no_mutex_t* mutex)
{
	if (xSemaphoreTake(*mutex, portMAX_DELAY) != pdTRUE) {
		no_serial_write("mutex_acquire: fail");
	}
}

void no_mutex_release(no_mutex_t* mutex)
{
	if (xSemaphoreGive(*mutex) != pdTRUE) {
		no_serial_write("mutex_release: fail");
	}
}

void no_event_create(no_event_t* event)
{
	*event = xEventGroupCreate();
}

void no_event_set(no_event_t* event)
{
	xEventGroupSetBits(*event, 0x08);
}

void no_event_reset(no_event_t* event)
{
	xEventGroupClearBits(*event, 0x08);
}

void no_event_wait(no_event_t* event)
{
	xEventGroupWaitBits(*event, 0x08, pdTRUE, pdTRUE, portMAX_DELAY);
}

void no_mq_create(no_mq_t* mq, unsigned int length, unsigned int msgsize)
{
	*mq = xQueueCreate(length, msgsize);
}

void no_mq_send(no_mq_t* mq, unsigned int msg)
{
	xQueueSend(*mq, (void*)&msg, portMAX_DELAY);
}

void no_mq_receive(no_mq_t* mq)
{
	unsigned int rcv;
	xQueueReceive(*mq, &rcv, portMAX_DELAY);
}

void no_serial_write(const char* string)
{
	vSendString((char*)string);
}

void no_report_results(long elapsed, long per_loop_overhead, long loop_overhead, int iteration)
{
	memset(printfbuf, '\0', 100);
	long time = ((elapsed - loop_overhead) / iteration) - per_loop_overhead;
	sprintf(printfbuf, "elapsed=%lu\nper_loop_overhead=%lu\nloop_overhead=%lu\niteration=%d\ntime=%lu",
			elapsed, per_loop_overhead, loop_overhead, iteration, time);
	no_serial_write(printfbuf);
}

void no_reset_all_pmu_counter()
{
	
	asm volatile ("MOV x0, #-1 \n\t"
	 			"msr pmovsclr_el0, x0");
				
	asm volatile ("mrs x1, pmcr_el0 \n\t"
	 			"bic x1, x1, #1 \n\t msr pmcr_el0, x1 \n\t"
				"orr x1, x1, #1 \n\t msr pmcr_el0, x1");
}

unsigned int no_inst_get_count()
{
	unsigned int cc = 0;
	unsigned int reg = 0x0;
	/* PMSELR */
	// asm volatile ("msr PMSELR, %0" : : "r" (reg));
	// /* READ COUNT */
	// asm volatile ("mrs %0, PMSELR" : "=r" (cc));
	vSendStringISR("what is this?\n");
	return cc;
}

void no_cycle_reset_counter()
{
	__asm__ volatile ("mrs x27, CNTV_CTL_EL0 \n\t"
		"BIC x27, x27, #1 \n\t msr CNTV_CTL_EL0, x27 \n\t"
		"ORR x27, x27, #1 \n\t msr CNTV_CTL_EL0, x27");
    // 设置 ENABLE 位(1), restart Local Timer

}

unsigned int no_cycle_get_count()
{
	unsigned int cc;
	asm volatile ("mrs %0, cntvct_el0" : "=r" (cc));
	return cc;
}

void no_single_result_report(char* prefix, int64_t time)
{
	memset(printfbuf, '\0', 100);
	int32_t test = (int32_t)time;
	sprintf(printfbuf, "%s%d", prefix, test);
	no_serial_write(printfbuf);
}

void no_result_report(int64_t max, int64_t min, int64_t average)
{
	memset(printfbuf, '\0', 100);
	int32_t a, b, c;
	a = (int32_t)max;
	b = (int32_t)min;
	c = (int32_t)average;
	sprintf(printfbuf, "max=%d\nmin=%d\naverage=%d", a, b, c);
	no_serial_write(printfbuf);
}

void no_interrupt_do_sgi()
{
	// Trigger core0 interrupt
	*coreMailboxInterrupt = 0x1;
}

void no_interrupt_enable_sgi()
{
	*coreMailboxInterruptCtrl = 0x1;
}

void no_interrupt_register_sgi_handler(no_int_handler_t fn)
{
	no_serial_write("interrupt sgi handled");
	//rpi_irq_register_handler(64, fn, NULL);
}


void vApplicationIRQHandler( uint32_t ulICCIAR )
{
    //printHexISR(ulICCIAR);
    //vSendStringISR("[vApplicationIRQHandler]\n");

    return;
}

int FreeRTOS_Error_Handler (uint64_t* ctx, int a, int b) {
    portDISABLE_INTERRUPTS();
    vSendStringISR("INVALID interrupt handled.\n");
    while(1);
    return 0;
}

int FreeRTOS_SYNC_Handler (uint64_t* ctx) {
    portDISABLE_INTERRUPTS();
    vSendStringISR("SYNC interrupt handled.\n");
    while(1);
    return 0;
}

void vSetupTimerInterrupt( void )
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
void vClearTimerInterrupt( void )
{
    int clock_rate;
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r" (clock_rate) );    // 设置计数器的加载值（例如1秒）
    int cnt = clock_rate / configTICK_RATE_HZ;

    __asm__ volatile ("msr cntv_tval_el0, %0" :: "r" (cnt));
    // 设置计数器的加载值（例如1秒）
}


#ifdef TRACING

void trace_switch_in(int tid)
{
	no_tracing_switch_in(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "in tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_switch_out(int tid)
{
	no_tracing_switch_out(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "out tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_switch_ready(int tid)
{
	no_tracing_switch_ready(tid);

	//memset(printfbuf, '\0', 100);
	//sprintf(printfbuf, "ready tracesize=%d\n", no_tracing_status.idx);
	//rpi_aux_mu_string(printfbuf);
}

void trace_task_create(no_task_handle_t task)
{
	vTaskSetTaskNumber(task, task_ids);
	task_ids++;
}

extern char no_trace_buffer[];
extern int32_t tracing_on;

void no_tracing_report()
{
	no_tracing_status.on = 0;
	int i;
	int* buf_ptr = (int*)no_trace_buffer;

	memset(printfbuf, '\0', 100);
	int int_size = no_tracing_status.idx >> 2;
	sprintf(printfbuf, "tracesize=%d\n", no_tracing_status.idx);
	no_serial_write(printfbuf);
	for (i = 0; i < no_tracing_status.idx; i++)
	{
		uart_put_sync(no_trace_buffer[i]);
	}
	no_serial_write("Trace transmitted");
	no_tracing_status.on = 1;
}

void clear_trace_buffer()
{
	int i;
	for (i = 0; i < NO_BUFFER_SIZE; i++)
	{
		no_trace_buffer[i] = 0;
	}
}

void start_trace()
{
	rpi_aux_mu_string("TRACING ON\n");
	no_cycle_reset_counter();

	no_tracing_status.on = 1;
	no_tracing_status.idx = 0;
}

#endif
