#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "timers.h"

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

// 定时器任务1
TimerHandle_t time1_handle;
void vTimerCallback1(TimerHandle_t xTimer);
// 定时器任务2
TimerHandle_t time2_handle;
void vTimerCallback2(TimerHandle_t xTimer);

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

  /* 创建一次性定时器 */
  time1_handle = xTimerCreate(
    (char *)"time1",
    pdMS_TO_TICKS(1000),
    pdFALSE,
    (void *)0,
    vTimerCallback1
  );
  if (time1_handle != NULL) printf("time1 create success\r\n");

  /* 创建循环定时器 */
  time2_handle = xTimerCreate(
    (char *)"time2",
    pdMS_TO_TICKS(1000),
    pdTRUE,
    (void *)1,
    vTimerCallback2
  );
  if (time2_handle != NULL) printf("time2 create success\r\n");

  // 删除启动任务，只需要执行一次
  vTaskDelete(start_task_handle);

  /* 退出临界区 */
  taskEXIT_CRITICAL();
}

uint32_t res;
void task1(void *arg)
{
  // 任务1的实现，用于按键扫描，并对软件定时器进行开启、停止操作
  printf("task1 is running\r\n");
  while (1)
  {
    if (key_is_pressed == 1)
    {
      res = xTimerStart(time1_handle, portMAX_DELAY);
      if (res == pdTRUE)
      {
        printf("time1 start success\r\n");
      }
      else
      {
        printf("time1 start fail\r\n");
      }
      res = xTimerStart(time2_handle, portMAX_DELAY);
      if (res == pdTRUE)
      {
        printf("time2 start success\r\n");
      }
      else
      {
        printf("time2 start fail\r\n");
      }
      key_is_pressed = 0;
    }
    else if (key_is_pressed == 2)
    {
      res = xTimerStop(time1_handle, portMAX_DELAY);
      if (res == pdTRUE)
      {
        printf("time1 stop success\r\n");
      }
      else
      {
        printf("time1 stop fail\r\n");
      }
      res = xTimerStop(time2_handle, portMAX_DELAY);
      if (res == pdTRUE)
      {
        printf("time2 stop success\r\n");
      }
      else
      {
        printf("time2 stop fail\r\n");
      }
      key_is_pressed = 0;
    }
    vTaskDelay(10);
  }
}

void vTimerCallback1(TimerHandle_t xTimer)
{
  static uint16_t count = 0;
  printf("time1 %d\r\n", ++count);
}

void vTimerCallback2(TimerHandle_t xTimer)
{
  static uint16_t count = 0;
  printf("time2 %d\r\n", ++count);
}
