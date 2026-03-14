#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"
#include "semphr.h"

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

// 队列句柄
QueueSetHandle_t queue_set;
QueueHandle_t queue1;
QueueHandle_t semaphore1;

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
  // 0. 创建队列集、队列、信号量
  /* 队列集 */
  queue_set = xQueueCreateSet(2);
  if (queue_set != NULL) printf("create queue_set success\r\n");
  /* 消息队列 */
  queue1 = xQueueCreate(2, sizeof(uint8_t));
  if (queue1 != NULL) printf("queue1 create success\r\n");
  /* 信号量 */
  semaphore1 = xSemaphoreCreateBinary();
  if (semaphore1 != NULL) printf("semaphore1 create success\r\n");
  /* 将消息队列和信号量加入队列集，注意：这里的消息队列和信号量必须为空 */
  BaseType_t tag;
  tag = xQueueAddToSet(queue1, queue_set);
  if (tag == pdPASS) printf("add 1 success\r\n");
  xQueueAddToSet(semaphore1, queue_set);
  if (tag == pdPASS) printf("add 2 success\r\n");

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
  // 任务1：用于扫描按键，当KEY1按下，往队列写入数据，当KEY2按下，释放二值信号量
  BaseType_t res;
  while (1)
  {
    if (key_is_pressed == 1)
    {
      res = xQueueSend(queue1, &key_is_pressed, portMAX_DELAY);
      if (res == pdPASS)
      {
        printf("send %d success\r\n", key_is_pressed);
      }
      else
      {
        printf("send %d fail\r\n", key_is_pressed);
      }
      key_is_pressed = 0;
    }
    if (key_is_pressed == 2)
    {
      res = xSemaphoreGive(semaphore1);
      if (res == pdPASS)
      {
        printf("give %d success\r\n", key_is_pressed);
      }
      else
      {
        printf("give %d fail\r\n", key_is_pressed);
      }
      key_is_pressed = 0;
    }
  }
}

void task2(void *arg)
{
  printf("task2 running...\r\n");
  // 任务2：读取队列集中的消息，并打印
  QueueSetMemberHandle_t res;
  char *pvBuffer;
  while (1)
  {
    res = xQueueSelectFromSet(queue_set, portMAX_DELAY);
    if (res == queue1)
    {
      xQueueReceive(queue1, &pvBuffer, portMAX_DELAY);
      printf("receive data: %d\r\n", pvBuffer);
    }
    else if (res == semaphore1)
    {
      xSemaphoreTake(semaphore1, portMAX_DELAY);
      printf("take semaphore\r\n");
    }
    else
    {
      printf("queueSet empty\r\n");
    }

  }
}
