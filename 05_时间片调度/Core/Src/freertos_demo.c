#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

extern uint8_t key3_is_pressed; // 声明外部变量key3_is_pressed

// 启动任务配置
#define START_TASK_STACK_DEPTH 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle = NULL;
void start_task(void *arg);

// 任务1配置
#define TASK1_STACK_DEPTH 128
#define TASK1_PRIORITY 2
TaskHandle_t task1_handle = NULL;
void task1(void *arg);

// 任务2配置
#define TASK2_STACK_DEPTH 128
#define TASK2_PRIORITY 2
TaskHandle_t task2_handle = NULL;
void task2(void *arg);

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
  // 1. 创建启动任务
  xTaskCreate(
      (TaskFunction_t)start_task,                     // 任务函数
      (char *)"start_task",                           // 任务名称
      (configSTACK_DEPTH_TYPE)START_TASK_STACK_DEPTH, // 任务栈大小
      (void *)NULL,                                   // 任务参数
      (UBaseType_t)START_TASK_PRIORITY,               // 任务优先级
      (TaskHandle_t *)&start_task_handle);            // 任务句柄
  // 2. 启动调度器: 会自动创建空闲任务
  vTaskStartScheduler();
}

void start_task(void *arg)
{
  /* 进入临界区: 保护临界区代码不会被打断 */
  taskENTER_CRITICAL();

  // 1. 创建任务1
  xTaskCreate(
      (TaskFunction_t)task1,                     // 任务函数
      (char *)"task1",                           // 任务名称
      (configSTACK_DEPTH_TYPE)TASK1_STACK_DEPTH, // 任务栈大小
      (void *)NULL,                              // 任务参数
      (UBaseType_t)TASK1_PRIORITY,               // 任务优先级
      (TaskHandle_t *)&task1_handle);            // 任务句柄
  // 2. 创建任务2
  xTaskCreate(
      (TaskFunction_t)task2,                     // 任务函数
      (char *)"task2",                           // 任务名称
      (configSTACK_DEPTH_TYPE)TASK2_STACK_DEPTH, // 任务栈大小
      (void *)NULL,                              // 任务参数
      (UBaseType_t)TASK2_PRIORITY,               // 任务优先级
      (TaskHandle_t *)&task2_handle);            // 任务句柄

  // 删除启动任务，只需要执行一次
  vTaskDelete(start_task_handle);

  /* 退出临界区 */
  taskEXIT_CRITICAL();
}

uint16_t count1 = 0;
void task1(void *arg)
{
  // 任务1的实现，实现LED1每500ms闪烁一次
  while (1)
  {
    count1++;
    printf("task1 is running, count: %d\r\n", count1);
    // vTaskDelay(500 / portTICK_PERIOD_MS); // 延时500ms，为了观察时间片调度，不使用freertos的延时函数，而是使用HAL库的延时函数
    // 使用hal_delay前提：hal时钟修改成其他定时器，且中断优先级较高
    HAL_Delay(10); // 使用HAL库的延时函数，单位为ms
  }
}
uint16_t count2 = 0;
void task2(void *arg)
{
  // 任务2的实现，实现LED2每500ms闪烁一次
  while (1)
  {
    count2++;
    printf("task2 is running, count: %d\r\n", count2);
    // vTaskDelay(500 / portTICK_PERIOD_MS); // 延时500ms
    HAL_Delay(10); // 使用HAL库的延时函数，单位为ms
  }
}
