#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

extern uint8_t key_is_pressed; // 声明外部变量key_is_pressed

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

// 句柄
EventGroupHandle_t event_group;
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)
#define BIT_3 (1 << 3)

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
  // 0. 创建事件标志组
  event_group = xEventGroupCreate();
  if (event_group != NULL)
    printf("event_group create success\r\n");

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

void task1(void *arg)
{
  printf("task1 running...\r\n");
  // 任务1：读取按键按下键值，根据不同键值将事件标志组相应事件位置一，模拟事件发生
  while (1)
  {
    switch (key_is_pressed)
    {
    case 1:
      printf("set bit 0\r\n");
      xEventGroupSetBits(event_group, BIT_0);
      break;
    case 2:
      printf("set bit 1\r\n");
      xEventGroupSetBits(event_group, BIT_1);
      break;
    case 3:
      printf("set bit 2\r\n");
      xEventGroupSetBits(event_group, BIT_2);
      break;
    case 4:
      printf("set bit 3\r\n");
      xEventGroupSetBits(event_group, BIT_3);
      break;
    }

    key_is_pressed = 0;
  }
}

void task2(void *arg)
{
  printf("task2 running...\r\n");
  // 任务2：同时等待事件标志组中的多个事件位，当这些事件位都置 1 的话就执行相应的处理
  EventBits_t uxBits;
  while (1)
  {
    uxBits = xEventGroupWaitBits(event_group, (BIT_0 | BIT_1 | BIT_2 | BIT_3), pdTRUE, pdTRUE, portMAX_DELAY);
    printf("等待到的事件标志位值=%#x\r\n",uxBits);
    if (uxBits & (BIT_0 | BIT_1 | BIT_2 | BIT_3) == (BIT_0 | BIT_1 | BIT_2 | BIT_3))
    {
      printf("event group is run\r\n");
    }
  }
}
