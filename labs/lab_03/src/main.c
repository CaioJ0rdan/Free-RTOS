#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


TaskHandle_t T1;
TaskHandle_t T2;
TaskHandle_t T3;
TaskHandle_t T4;


const uint LED_PIN =  15;
const uint LED_PIN2 =   13;



volatile uint32_t idleTicks = 0;


void vApplicationIdleHook(void)
{	
	unsigned long int tick = xTaskGetTickCount();
	while (xTaskGetTickCount() == tick);
	idleTicks++;

}


void taskCPUUsage()
{
	unsigned long int idle_tick_last, ticks;
	idle_tick_last = 0;// idleTicks = 0;
	for (;;) {
		// wait 3 seconds
		vTaskDelay(3000/portTICK_PERIOD_MS);

		/* Ticks per second calculation */
		if (idleTicks > idle_tick_last)
			ticks = idleTicks - idle_tick_last;
		else
			ticks = 0xFFFFFFFF - idle_tick_last + idleTicks;

        // Dividing by the number of tasks
		ticks /= 4;

		/* Printing idle ticks per second */
		printf("%ld idle ticks per second (out of %ld)\n", ticks, configTICK_RATE_HZ);

		/* calc and print CPU usage */
		ticks = (configTICK_RATE_HZ - ticks)/10;
		printf("CPU usage: %d%%\n", ticks);

		// updating 
		idle_tick_last = idleTicks;
	}
}




void task_blink()
{   
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(300/ portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(300/ portTICK_PERIOD_MS);
    }
    
}

void task_blink2()
{   
    
    gpio_init(LED_PIN2);
    gpio_set_dir(LED_PIN2, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN2, 1);
        vTaskDelay(300/ portTICK_PERIOD_MS);
        gpio_put(LED_PIN2, 0);
        vTaskDelay(300/ portTICK_PERIOD_MS);
    }
    
}



void task_button(){

    const int Gpio = 16;
    gpio_init(Gpio);
    gpio_set_dir(Gpio,GPIO_IN);
    while(1){
        if(gpio_get(Gpio)){
            gpio_put(13,0);
            gpio_put(15,0);
            vTaskDelete(T1);
            vTaskDelete(T2);
            vTaskDelete(NULL);
        }
    }

}




int main()
{
    stdio_init_all();

    //uint tempo = 300; // time in seconds to execute task 3
    xTaskCreate(task_blink, "LED_Task 1", 256,NULL, 1, &T1);
  
    xTaskCreate(task_blink2, "LED_Task 2", 256, NULL, 1, &T2);

    xTaskCreate(task_button, "Button_Task 3", 256, NULL, 1 , &T3);

    xTaskCreate(taskCPUUsage,"Task CPU Usage", 256,NULL, 2, &T4 );

    

    vTaskStartScheduler();

    while(1){};
}