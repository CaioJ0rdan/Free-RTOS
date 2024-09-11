#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <queue.h>


#define ADC_NUM 0
#define ADC_PIN (26 +ADC_NUM )
#define ADC_VREF 3.3

#define LED_PIN 15

#define QUEUE_SIZE 5



TaskHandle_t T1;
TaskHandle_t T2;




void taskLed(void *pvParameters)
{   
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    QueueHandle_t Queue0 = (QueueHandle_t) pvParameters;
    bool Upper20k;



    while (true) {

        if(xQueueReceive(Queue0, &Upper20k, portMAX_DELAY) == pdPASS){
            

            if(Upper20k){
                gpio_put(LED_PIN, 1);
            }else{
                gpio_put(LED_PIN, 0);
            }

        }
        vTaskDelay(300/ portTICK_PERIOD_MS);
    }
    
}


void taskReadPot(void *pvParameters){
    
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);

    QueueHandle_t Queue0 = (QueueHandle_t) pvParameters;
    bool Upper20k;
    
    while (true){
        uint16_t result = adc_read();
                
        float voltage = result * 3.3 / (1 << 12);

        printf("ADC Value: %d, Voltage: %.2f V\n", result, voltage);
        

        // 1.6 is the proportion of 2k to 2^12

        // 3.3 - x
        // 4






        if(voltagem >= 1.6){
            Upper20k = true;
        }else{
            Upper20k = false;
        }

        xQueueSend(Queue0, &Upper20k, portMAX_DELAY) == pdPASS;
            
        

    }




}


int main()
{

    stdio_init_all();

    QueueHandle_t Queue0;
    Queue0 = xQueueCreate(QUEUE_SIZE,sizeof(bool));


    xTaskCreate(taskLed, "LED_Task",
     configMINIMAL_STACK_SIZE, (void*) Queue0, 1, &T1);

    xTaskCreate(taskReadPot, "Read_Potentiometer_Task",
     configMINIMAL_STACK_SIZE, (void*) Queue0 , 1 , &T2);

    vTaskStartScheduler();

    while(1){};
}