#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "pico/stdlib.h"
#include <string.h>

// Define os pinos dos botões e do LED
#define LED_PIN 15
#define BUTTON_1 11
#define BUTTON_2 12


#define FILL_BLOCK_SIZE 20000  // Block Size
#define MAX_BLOCKS 6        // Number of blocks to be allocated




// Array of blocks
void *pvAllocatedBlocks[MAX_BLOCKS];
int iAllocatedBlocks = 0; // Counter of blocks allocated





// Task that monitors the state of the heap
void TaskMonitoramento(void *pvParameters) {
    const uint32_t ulTotalHeapSize = configTOTAL_HEAP_SIZE; // Total size of Heap
    uint32_t ulFreeHeapSize;

    while(1) {
        // Get a Free size a heap
        ulFreeHeapSize = xPortGetFreeHeapSize();

        // Calculates the Heap Free Percentage
        uint32_t percent= 100*ulFreeHeapSize/ulTotalHeapSize;


        // Send a Free size of serial port
        printf("Porcentagem livre do heap: %u \n", percent);

        // Up a Led if the free size are equal or less a 10%
        if(ulFreeHeapSize < (ulTotalHeapSize / 10)) {
            gpio_put(LED_PIN, 1);  
        } else {
            gpio_put(LED_PIN, 0);  
        }

       
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}





// Task to alocate memory when press a button
void TaskAlocarMemoria(void *pvParameters) {
    while(1) {
       
        if (gpio_get(BUTTON_1) == 1) {
            if (iAllocatedBlocks < MAX_BLOCKS) {
                // Allocate a block
                pvAllocatedBlocks[iAllocatedBlocks] = pvPortMalloc(FILL_BLOCK_SIZE);

                // Test if a alocation are sucesfully
                if (pvAllocatedBlocks[iAllocatedBlocks] != NULL) {
                    // Simulate a memory use
                    memset(pvAllocatedBlocks[iAllocatedBlocks], 0, FILL_BLOCK_SIZE);
                    iAllocatedBlocks++;
                    printf("Memory Allocated. Blocks allocated: %d\n", iAllocatedBlocks);
                } else {
                    printf("Fail to allocated memory!\n");
                }
            } else {
                printf("Max of allocations realized.\n");
            }

            
            while(gpio_get(BUTTON_1) == 1) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}








// Task to free memory when press a button
void TaskLiberarMemoria(void *pvParameters) {
    while(1) {
        
        if (gpio_get(BUTTON_2) == 1) {
            if (iAllocatedBlocks > 0) {
                // Free the last block
                iAllocatedBlocks--;
                vPortFree(pvAllocatedBlocks[iAllocatedBlocks]);
                pvAllocatedBlocks[iAllocatedBlocks] = NULL;
                printf("Memória liberada. Blocos restantes: %d\n", iAllocatedBlocks);
            } else {
                printf("Nenhum bloco para liberar.\n");
            }

            // Wait to liberation a button
            while(gpio_get(BUTTON_2) == 1) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}





int main() {
    // Init Led's
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


    // Init the button to allocate memory
    gpio_init(BUTTON_1);
    gpio_set_dir(BUTTON_1, GPIO_IN);
    gpio_pull_down(BUTTON_1);


    // Init the red button to free memory
    gpio_init(BUTTON_2);
    gpio_set_dir(BUTTON_2, GPIO_IN);
    gpio_pull_down(BUTTON_2);

    // Init serial port
    stdio_init_all();

    
    xTaskCreate(TaskMonitoramento, "Monitoramento da Heap", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(TaskAlocarMemoria, "Alocação de Memória", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(TaskLiberarMemoria, "Liberação de Memória", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);

    // Start a scheduler
    vTaskStartScheduler();

    while(1);
    return 0;
}
