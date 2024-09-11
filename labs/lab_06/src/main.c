#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include "hardware/adc.h"
#include <queue.h>


#define ADC_NUM 0
#define ADC_PIN (26 +ADC_NUM )

SemaphoreHandle_t xMutex;


uint16_t potValue;



void vTaskLerPotenciometro(void *pvParameters) {

    for(;;) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY)) {
            
            //  Leitura do Potenciometro
            potValue = adc_read();
            printf("Reading ...\n");
            
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


void vTaskMostrarConteudo(void *pvParameters) {

    for(;;) {
        if(xSemaphoreTake(xMutex, portMAX_DELAY)) {
            
            printf("Potentiometer Value : %u \n",potValue);    
            
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



int main() {
    // Start hardware
    stdio_init_all();


    // Potentiometer Config
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(ADC_NUM);
    
    // Create a Mutex
    xMutex = xSemaphoreCreateMutex();

    // Create a Tasks
    xTaskCreate(vTaskLerPotenciometro, "Potentiometer_Read", 256, NULL, 2, NULL);
    xTaskCreate(vTaskMostrarConteudo, "Print_Data", 256, NULL, 1, NULL);

    // Start a scheduler
    vTaskStartScheduler();

    for (;;) {}
    return 0;
}
