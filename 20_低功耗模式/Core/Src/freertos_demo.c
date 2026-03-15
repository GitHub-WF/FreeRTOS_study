#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "semphr.h"

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

/* 进入睡眠模式 */
void PRE_SLEEP_PROCESSING()
{
  __HAL_RCC_GPIOA_CLK_DISABLE();
}
/* 退出睡眠模式 */
void POST_SLEEP_PROCESSING()
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
}

// 信号量
xSemaphoreHandle xSemaphore;

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
  // 创建二进制信号量
  xSemaphore = xSemaphoreCreateBinary();
  if (xSemaphore != NULL)
  {
    printf("xSemaphore create success\r\n");
  }
  else
  {
    printf("xSemaphore create fail\r\n");
  }

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
  printf("task1 is runing...\r\n");
  // 任务1：用于按键扫描，当检测到按键KEY1被按下时，释放二值信号量
  while (1)
  {
    if (key3_is_pressed == 1)
    {
      printf("key1 is pressed\r\n");
      key3_is_pressed = 0;

      if (xSemaphoreGive(xSemaphore) == pdTRUE)
      {
        printf("give xSemaphore success\r\n");
      }
      else
      {
        printf("give xSemaphore fail\r\n");
      }
    }
    vTaskDelay(10);
  }
}

void task2(void *arg)
{
  printf("task2 is runing...\r\n");
  // 任务2：获取二值信号量，当成功获取后打印提示信息
  while (1)
  {
    printf("try get xSemaphore...\r\n");
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
    {
      printf("get xSemaphore success\r\n");
    }
    else
    {
      printf("get xSemaphore fail\r\n");
    }
  }
}
