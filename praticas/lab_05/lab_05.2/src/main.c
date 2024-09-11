#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <stdio.h>

// Pin defines
#define BUTTON_PIN 16
#define LED_PIN 15

uint button_pressed = 0;

// Tasks
void button_task(void *pvParameters);
void led_task(void *pvParameters);
void semaphore_test_task(void *pvParameters);

// Semaphore and queue
SemaphoreHandle_t button_semaphore;
QueueHandle_t led_queue;

typedef enum {
    LED_TOGGLE 
} led_command_t;

void button_task(void *pvParameters) {
    (void)pvParameters;
    TickType_t xDelay = pdMS_TO_TICKS(50);

    while (1) {
        // Wait for semaphore
        if ((xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE) && button_pressed) {
            // Envia um comando para a tarefa do LED alternar seu estado
            printf("Semaforo consumido pela tarefa do botão\n");
            led_command_t command = LED_TOGGLE;
            xQueueSend(led_queue, &command, portMAX_DELAY);

            vTaskDelay(xDelay);
            xSemaphoreGive(button_semaphore);
            printf("Semaforo liberado pela tarefa do botão\n");
            button_pressed = !button_pressed;
        }
    }
}

// Task to blink Led
void led_task(void *pvParameters) {
    (void)pvParameters;

    led_command_t command;
    while (1) {
  
        if (xQueueReceive(led_queue, &command, portMAX_DELAY) == pdTRUE) {
            if (command == LED_TOGGLE) {
                gpio_put(LED_PIN, !gpio_get(LED_PIN));
            }
        }
    }
}

// Task to use semaphore
void semaphore_test(void *pvParameteres) {
	while (1) {
		if (xSemaphoreTake(button_semaphore, portMAX_DELAY) == pdTRUE) {
			printf("Semaforo livre\n");
			vTaskDelay(500);
			xSemaphoreGive(button_semaphore);
		} else {
			printf("Semaforo bloqueado\n");
			vTaskDelay(500);
		}
	}
}

/ Button Interruption
void button_isr(uint gpio, uint32_t events) {
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(button_semaphore, &higherPriorityTaskWoken);
    portYIELD_FROM_ISR(higherPriorityTaskWoken);
    button_pressed = !button_pressed;
}

// Button Interuption Config
void setup_button_interrupt(void) {
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE, true, button_isr);
}

int main() {
    // Start hardware
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN); 
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Crie a Semaphore Counter
    button_semaphore = xSemaphoreCreateCounting(3, 0); // Capacidade do semáforo: 3
    if (button_semaphore == NULL) {
        printf("Failed to create button semaphore\n");
        return -1;
    }

    // Create a queue
    led_queue = xQueueCreate(10, sizeof(led_command_t));
    if (led_queue == NULL) {
        printf("Failed to create LED queue\n");
        return -1;
    }

    // Create a tasks
    xTaskCreate(button_task, "ButtonTask", 256, NULL, 1, NULL);
    xTaskCreate(led_task, "LedTask", 256, NULL, 1, NULL);
    xTaskCreate(semaphore_test_task, "SemaphoreTestTask", 256, NULL, 1, NULL);
    xTaskCreate(semaphore_test, "Test_Semaforo", 256, NULL, 1, NULL);

    // Config a button interruption
    setup_button_interrupt();

    // Start a scheduler
    vTaskStartScheduler();

   
    for (;;) {}
    return 0;
}
