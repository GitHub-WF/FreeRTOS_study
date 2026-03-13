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

// 任务3配置
#define TASK3_STACK_DEPTH 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle = NULL;
void task3(void *arg);

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
  // 2. 创建任务3
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
  printf("task1 is runing...\r\n");
  // 任务1：低优先级任务，同高优先级一样的操作，不同的是低优先级任务占用信号量的时间久一点
  while (1)
  {
    printf("低优先级Task1正在运行\r\n");
    HAL_Delay(3000);
    printf("低优先级Task1释放信号量\r\n");
    xSemaphoreGive(xSemaphore);
    vTaskDelay(1000);
  }
}

void task2(void *arg)
{
  while (1)
  {
    printf("中优先级的Task2正在执行\r\n");
    HAL_Delay(1500);
    printf("Task2执行完成一次.....\r\n");
    vTaskDelay(1000);
  }

}

void task3(void *arg)
{
  printf("task3 is runing...\r\n");
  // 任务3：高优先级任务，会获取二值信号量，获取成功以后打印提示信息，处理完后释放信号量
  while (1)
  {
    printf("高优先级Task3获取信号量\r\n");
    xSemaphoreTake(xSemaphore,portMAX_DELAY);
    printf("高优先级Task3正在运行\r\n");
    HAL_Delay(1000);
    printf("高优先级Task3释放信号量\r\n");
    xSemaphoreGive(xSemaphore);
    vTaskDelay(1000);
  }
}
