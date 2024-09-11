#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"


void led_task()
{   
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN, 1);
        vTaskDelay(1000);
        gpio_put(LED_PIN, 0);
        vTaskDelay(1000);
    }
}

void led_task2()
{   
    const uint LED_PIN2 = 16;
    gpio_init(LED_PIN2);
    gpio_set_dir(LED_PIN2, GPIO_OUT);
    while (true) {
        gpio_put(LED_PIN2, 1);
        vTaskDelay(2000);
        gpio_put(LED_PIN2, 0);
        vTaskDelay(2000);
    }
}

int main()
{
    stdio_init_all();

    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, NULL);
    xTaskCreate(led_task2, "LED_Task2", 256, NULL, 2, NULL);

    vTaskStartScheduler();

    while(1){};
}