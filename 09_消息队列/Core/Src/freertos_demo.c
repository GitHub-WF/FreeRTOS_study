#include "freertos_demo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "queue.h"

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

// 任务3配置
#define TASK3_STACK_DEPTH 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle = NULL;
void task3(void *arg);

// 队列句柄
QueueHandle_t queue1;
QueueHandle_t big_queue;

/**
 * @description: 启动FreeRTOS
 * @return {*}
 */
void freertos_start(void)
{
  // 0. 创建任务队列
  /* 创建存放key值的队列 */
  queue1 = xQueueCreate(2, sizeof(uint8_t));
  if (queue1 != NULL)
  {
    printf("queue1 create success\r\n");
  }
  else
  {
    printf("queue1 create fail\r\n");
  }
  /* 创建存放大数据的队列，存放大数据地址即可 */
  big_queue = xQueueCreate(1, sizeof(char *));
  if (big_queue != NULL)
  {
    printf("big_queue create success\r\n");
  }
  else
  {
    printf("big_queue create fail\r\n");
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
  // 3. 创建任务3
  xTaskCreate(
      (TaskFunction_t)task3,                     // 任务函数
      (char *)"task3",                           // 任务名称
      (configSTACK_DEPTH_TYPE)TASK3_STACK_DEPTH, // 任务栈大小
      (void *)NULL,                              // 任务参数
      (UBaseType_t)TASK3_PRIORITY,               // 任务优先级
      (TaskHandle_t *)&task3_handle);            // 任务句柄

  /* 退出临界区 */
  taskEXIT_CRITICAL();

  // 删除启动任务，只需要执行一次
  vTaskDelete(start_task_handle);
}

char *big_data = "adsfdfasdgsdfgsdfgasdfasdfasdfasdfasd";
void task1(void *arg)
{
  uint8_t res = 0;
  printf("task1 runing...\r\n");
  // 任务1：当按键key1或key2按下，将键值拷贝到队列queue1（入队）；当按键key3按下，将传输大数据，这里拷贝大数据的地址到队列big_queue中
  while (1)
  {
    if (key_is_pressed == 1 || key_is_pressed ==2)
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
    if (key_is_pressed == 3)
    {
      res = xQueueSend(big_queue, &big_data, portMAX_DELAY);
      if (res == pdPASS)
      {
        printf("send big_data success\r\n");
      }
      else
      {
        printf("send big_data fail\r\n");
      }
      key_is_pressed = 0;
    }
  }
}

void task2(void *arg)
{
  uint8_t res = 0;
  uint8_t received_key;
  printf("task2 runing...\r\n");
  // 任务2：读取队列queue1中的消息（出队），打印出接收到的键值
  while (1)
  {
    res = xQueueReceive(queue1, &received_key, portMAX_DELAY);
    if (res == pdPASS)
    {
      printf("read info: %d\r\n", received_key);
    }
    else
    {
      printf("read fail\r\n");
    }
  }
}

void task3(void *arg)
{
  uint8_t res = 0;
  char *received_ptr;
  printf("task3 runing...\r\n");
  // 任务3：从队列big_queue读取大数据地址，通过地址访问大数据
  while (1)
  {
    res = xQueueReceive(big_queue, &received_ptr, portMAX_DELAY);
    if (res)
    {
      printf("read big_info: %s\r\n", received_ptr);
    }
    else
    {
      printf("read fail\r\n");
    }
  }
}
