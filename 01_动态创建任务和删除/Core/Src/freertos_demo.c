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
#define TASK2_PRIORITY 3
TaskHandle_t task2_handle = NULL;
void task2(void *arg);

// 任务3配置
#define TASK3_STACK_DEPTH 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle = NULL;
void task3(void *arg);

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
  // 3. 创建任务3
  xTaskCreate(
      (TaskFunction_t)task3,                     // 任务函数
      (char *)"task3",                           // 任务名称
      (configSTACK_DEPTH_TYPE)TASK3_STACK_DEPTH, // 任务栈大小
      (void *)NULL,                              // 任务参数
      (UBaseType_t)TASK3_PRIORITY,               // 任务优先级
      (TaskHandle_t *)&task3_handle);            // 任务句柄

  // 删除启动任务，只需要执行一次
  vTaskDelete(start_task_handle);

  /* 退出临界区 */
  taskEXIT_CRITICAL();
}

void task1(void *arg)
{
  // 任务1的实现，实现LED1每500ms闪烁一次
  while (1)
  {
    printf("task1 is running\r\n");
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
    vTaskDelay(500 / portTICK_PERIOD_MS); // 延时500ms
  }
}

void task2(void *arg)
{
  // 任务2的实现，实现LED2每500ms闪烁一次
  while (1)
  {
    printf("task2 is running\r\n");
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    vTaskDelay(500 / portTICK_PERIOD_MS); // 延时500ms
  }
}

void task3(void *arg)
{
  // 任务3的实现，判断按键KEY3是否按下，按下则删掉task1
  while (1)
  {
    printf("task3 is running\r\n");
    if (key3_is_pressed && task1_handle != NULL)
    {
      printf("Key3 is pressed, deleting task1\r\n");
      vTaskDelete(task1_handle); // 删除任务1
      key3_is_pressed = 0;       // 重置标记
      task1_handle = NULL;       // 重置任务句柄
    }
    else if (key3_is_pressed == 0)
    {
      printf("Key3 is not pressed\r\n");
      printf("task1_handle: %p\r\n", task1_handle);
    }
    else
    {
      printf("Key3 is pressed, but task1_handle is NULL\r\n");
    }

    vTaskDelay(500 / portTICK_PERIOD_MS); // 延时500ms
    // HAL_Delay(500); // 使用 hal_delay 会导致 task3 无法进入阻塞状态，持续占用CPU，导致其他任务无法执行
  }
}
