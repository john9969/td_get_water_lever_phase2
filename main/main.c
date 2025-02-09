#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <board.h>

#include "app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

// #include "lte_app.h"
#define MONITER_MEMORY_HEAP 0

#if MONITER_MEMORY_HEAP

#define MONITORING_BUFF 2048
char task_monitoring_buff[MONITORING_BUFF];

#endif

//Button mbs_button = {.gpio = &button_gpio};
void p_monitor(){
	while(1){
	#if MONITER_MEMORY_HEAP
    	memset(task_monitoring_buff,0,MONITORING_BUFF);
    	vTaskList(task_monitoring_buff);
    	printf("**********************************\n");
    	printf("Task	State	Prio	Stack	Num\n");
    	printf("**********************************\n");
    	printf(task_monitoring_buff);
    	printf("\n**********************************\n");
    	uint32_t  free_heap_size = esp_get_free_heap_size();
    	printf("free heap size = %lu\n",free_heap_size);
#endif
        vTaskDelay(pdMS_TO_TICKS(1000));
	}
}
void app_main(void)
{
	board_init();
	app_init();
	p_monitor();
}

