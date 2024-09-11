#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


TaskHandle_t T1;
TaskHandle_t T2;
TaskHandle_t T3;

void task_01(void *pin)
{   
    
    const uint LED_PIN = (int *) pin;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(500/ portTICK_PERIOD_MS);
        gpio_put(LED_PIN, 0);
        vTaskDelay(500/ portTICK_PERIOD_MS);
    }
    
 
}


void Task_Delete(void* p )
{   
    
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while(1) {
        for(uint i=(uint *) p;i>0;i--){
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        for(uint i=0;i<3;i++) {
            gpio_put(LED_PIN, 0);
            vTaskDelay(150/ portTICK_PERIOD_MS);
            gpio_put(LED_PIN, 1); 
            vTaskDelay(150/ portTICK_PERIOD_MS);
        }
        vTaskDelete(T1);
        vTaskDelete(T2);
        vTaskDelete();
    }
    

    // Delete the task if it is outside the loop 
    
}



int main()
{
    stdio_init_all();
    uint tempo = 300; // time in seconds to execute task 3
    xTaskCreate(task_01, "LED_Task 1", 256, (void *) 15, tskIDLE_PRIORITY, &T1);
    xTaskCreate(task_01, "LED_Task 2", 256, (void *)13, tskIDLE_PRIORITY, &T2);
    xTaskCreate(Task_Delete, "LED_Task 3", 256, (void *) tempo, tskIDLE_PRIORITY , &T3);

    vTaskStartScheduler();

    while(1){};
}