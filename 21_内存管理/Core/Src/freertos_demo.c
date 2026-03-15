#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

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

  // 删除启动任务，只需要执行一次
  vTaskDelete(start_task_handle);

  /* 退出临界区 */
  taskEXIT_CRITICAL();
}

void task1(void *arg)
{
  uint8_t *buf = NULL;
  // 任务1的实现，实现LED1每500ms闪烁一次
  while (1)
  {
    if (key_is_pressed == 1)
    {
      /* 申请内存 */
      /* 实际申请空间要大，多出两部分：1.堆结构体大小 2.字节对齐浪费一部分 */
      buf = pvPortMalloc(20);
      if (buf != NULL)
      {
        printf("申请内存成功，剩余的空闲内存大小=%d\r\n", xPortGetFreeHeapSize());
      }
      else
      {
        printf("申请内存失败，剩余的空闲内存大小=%d\r\n", xPortGetFreeHeapSize());
      }
      key_is_pressed = 0;
    }
    else if (key_is_pressed == 2)
    {
      if (buf != NULL)
      {
        /* 释放内存 */
        vPortFree(buf);
        printf("释放内存，剩余的空闲内存大小=%d\r\n", xPortGetFreeHeapSize());
      }
      key_is_pressed = 0;
    }
    vTaskDelay(500);
  }
}
