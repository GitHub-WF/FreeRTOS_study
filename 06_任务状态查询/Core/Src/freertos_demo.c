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
#define TASK2_STACK_DEPTH 256
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

void task1(void *arg)
{
  // 任务1的实现，实现LED1每500ms闪烁一次
  while (1)
  {
    printf("task1 runing...\r\n");
    HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin); // 切换LED1状态
    vTaskDelay(500);                              // 延时500ms
  }
}

void task2(void *arg)
{
  /* 获取任务优先级 */
  UBaseType_t task1_priority = uxTaskPriorityGet(task1_handle);
  UBaseType_t task2_priority = uxTaskPriorityGet(task2_handle);
  printf("Task1 Priority: %lu, Task2 Priority: %lu\r\n", task1_priority, task2_priority);

  /* 修改任务的优先级 */
  vTaskPrioritySet(task1_handle, TASK1_PRIORITY + 1); // 提升任务1的优先级
  vTaskPrioritySet(task2_handle, TASK2_PRIORITY - 1); // 降低任务2的优先级
  printf("Task1 Priority after change: %lu, Task2 Priority after change: %lu\r\n",
         uxTaskPriorityGet(task1_handle), uxTaskPriorityGet(task2_handle));

  /* 获取系统中任务的数量 */
  UBaseType_t task_count = uxTaskGetNumberOfTasks();
  printf("Number of tasks in the system: %lu\r\n", task_count);

  /* 获取所有任务状态信息 */
  TaskStatus_t *pxTaskStatusArray;
  pxTaskStatusArray = pvPortMalloc(task_count * sizeof(TaskStatus_t));
  uxTaskGetSystemState(pxTaskStatusArray, task_count, NULL); // 获取系统状态信息
  for (UBaseType_t i = 0; i < task_count; i++)
  {
    printf("Task Name: %s, Task State: %d, Run Time Counter: %lu\r\n",
           pxTaskStatusArray[i].pcTaskName,
           pxTaskStatusArray[i].eCurrentState,
           pxTaskStatusArray[i].ulRunTimeCounter);
  }

  /* 获取指定单个任务信息 */
  TaskStatus_t task1_status;
  vTaskGetInfo(task1_handle, &task1_status, pdTRUE, eInvalid); // 获取任务1的状态信息
  printf("Task1 Name: %s, Task1 State: %d, Task1 Run Time Counter: %lu\r\n",
          task1_status.pcTaskName,
          task1_status.eCurrentState,
          task1_status.ulRunTimeCounter);

  /* 获取当前任务的任务句柄 */
  TaskHandle_t current_task_handle = xTaskGetCurrentTaskHandle();
  printf("Current Task Handle: %p\r\n", (void *)current_task_handle);

  /* 根据任务名字获取任务句柄 */
  TaskHandle_t task1_by_name = xTaskGetHandle("task1");
  printf("Task1 Handle by Name: %p\r\n", (void *)task1_by_name);

  /* 获取任务的任务栈历史剩余最小值 */
  UBaseType_t task1_stack_high_water_mark = uxTaskGetStackHighWaterMark(task1_handle);
  printf("Task1 Stack High Water Mark: %lu\r\n", task1_stack_high_water_mark);

  /* 获取任务状态 */
  eTaskState task1_state = eTaskGetState(task1_handle);
  printf("Task1 State: %d\r\n", task1_state);

  /* 以“表格”形式获取所有任务的信息 */
  char pcWriteBuffer[512];
  vTaskList(pcWriteBuffer); // 获取所有任务的信息并以表格形式输出
  printf("Task List:\r\n%s\r\n", pcWriteBuffer);

  /* 获取任务运行时间 */
  /* configRUN_TIME_COUNTER_TYPE task1_run_time = ulTaskGetRunTimeCounter(task1_handle);
  printf("Task1 Run Time Counter: %lu\r\n", task1_run_time); */

  // 任务2的实现，任务状态查询
  while (1)
  {
    printf("task2 runing...\r\n");
    vTaskDelay(500); // 使用FreeRTOS的延时函数，单位为tick
  }
}
