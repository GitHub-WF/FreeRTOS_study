#include "App_freeRTOS_Task.h"

// STM32F103C8T6的RAM为20KB，预留12KB给系统使用，剩余8KB给任务使用

void task1(void *arg);
#define TASK1_STACK_SIZE 128 // 任务栈大小，单位为字节，128字节对于这个简单的任务来说足够了
#define TASK1_PRIORITY 1 // 任务优先级，数值越大优先级越高（不推荐使用优先级 0）
TaskHandle_t task1Handle;

void task2(void *arg);
#define TASK2_STACK_SIZE 128 // 任务栈大小，单位为字节，128字节对于这个简单的任务来说足够了
#define TASK2_PRIORITY 1 // 任务优先级，数值越大优先级越高（不推荐使用优先级 0）
TaskHandle_t task2Handle;

/**
 * @brief 初始化freeRTOS任务
 *
 */
void App_freeRTOS_Task_Init(void)
{
  // 1.创建任务
  xTaskCreate(task1, "Task1", TASK1_STACK_SIZE, NULL, TASK1_PRIORITY, &task1Handle);
  xTaskCreate(task2, "Task2", TASK2_STACK_SIZE, NULL, TASK2_PRIORITY, &task2Handle);
  // 2.启动调度器
  vTaskStartScheduler();
}

void task1(void *arg)
{
  // task1启动后执行的内容
  while (1)
  {
    log("This is Task1");
    vTaskDelay(pdMS_TO_TICKS(1000)); // 延时1秒
  }
}

void task2(void *arg)
{
  // task2启动后执行的内容
  while (1)
  {
    log("This is Task2");
    vTaskDelay(pdMS_TO_TICKS(500)); // 延时0.5秒
  }
}
