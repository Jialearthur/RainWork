# STM32F407VET6 StdPeriph FreeRTOS 移植说明

本文档对应当前工程：

```text
D:\Rainwork\Project\STM32F4xx_StdPeriph_Templates\MDK-ARM\Project.uvprojx
```

目标平台：

```text
MCU: STM32F407VET6
Core: Cortex-M4F
Toolchain: Keil MDK ARMCC 5.06
Library: STM32F4xx StdPeriph
RTOS: FreeRTOS Kernel V11.3.0
```

## 1. FreeRTOS 文件目录

FreeRTOS 内核文件放在工程根目录：

```text
D:\Rainwork\Middlewares\FreeRTOS\Source
```

当前精简移植只加入了必要文件：

```text
Middlewares\FreeRTOS\Source\list.c
Middlewares\FreeRTOS\Source\queue.c
Middlewares\FreeRTOS\Source\tasks.c
Middlewares\FreeRTOS\Source\portable\MemMang\heap_4.c
Middlewares\FreeRTOS\Source\portable\RVDS\ARM_CM4F\port.c
Middlewares\FreeRTOS\Source\portable\RVDS\ARM_CM4F\portmacro.h
Middlewares\FreeRTOS\Source\include\*.h
```

说明：

- `tasks.c`：任务调度核心。
- `list.c`：FreeRTOS 内部链表管理。
- `queue.c`：队列、信号量、互斥量基础实现。
- `heap_4.c`：动态内存管理，支持释放后合并空闲块。
- `portable\RVDS\ARM_CM4F\port.c`：Keil ARMCC + Cortex-M4F 移植层。

## 2. 用户新增文件

当前工程新增了两个用户文件：

```text
Project\STM32F4xx_StdPeriph_Templates\FreeRTOSConfig.h
Project\STM32F4xx_StdPeriph_Templates\freertos_hooks.c
```

`FreeRTOSConfig.h` 是 FreeRTOS 的核心配置文件，必须能被 `FreeRTOS.h` 找到。

本工程通过 Keil include path 中的 `../` 找到它。

`freertos_hooks.c` 实现了：

```c
void vApplicationMallocFailedHook(void);
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName);
```

用于内存申请失败和任务栈溢出时停机保护。

## 3. Keil 工程配置

Keil 工程文件：

```text
Project\STM32F4xx_StdPeriph_Templates\MDK-ARM\Project.uvprojx
```

目标名：

```text
STM32F407VET6
```

### 3.1 Include Paths

当前 C/C++ include path：

```text
../
../../../Libraries/CMSIS/Include
../../../Libraries/CMSIS/Device/ST/STM32F4xx/Include
../../../Libraries/STM32F4xx_StdPeriph_Driver/inc
../../../Middlewares/FreeRTOS/Source/include
../../../Middlewares/FreeRTOS/Source/portable/RVDS/ARM_CM4F
```

其中 FreeRTOS 必须包含这两个目录：

```text
../../../Middlewares/FreeRTOS/Source/include
../../../Middlewares/FreeRTOS/Source/portable/RVDS/ARM_CM4F
```

### 3.2 Defines

当前宏定义：

```text
USE_STDPERIPH_DRIVER,STM32F40_41xxx
```

`STM32F40_41xxx` 对应 STM32F407 系列，不能删。

### 3.3 FreeRTOS 源码分组

Keil 工程中新增了 `FreeRTOS` 分组，包含：

```text
list.c
queue.c
tasks.c
heap_4.c
port.c
```

当前是精简版，没有加入：

```text
timers.c
event_groups.c
stream_buffer.c
croutine.c
```

如果以后启用软件定时器、事件组、消息缓冲区，需要再加入对应源码，并修改 `FreeRTOSConfig.h`。

## 4. FreeRTOSConfig.h 关键配置

当前使用抢占式调度：

```c
#define configUSE_PREEMPTION                    1
```

系统 tick 为 1ms：

```c
#define configTICK_RATE_HZ                      ( ( TickType_t ) 1000 )
```

CPU 时钟使用 CMSIS 变量：

```c
#define configCPU_CLOCK_HZ                      ( SystemCoreClock )
```

FreeRTOS 堆大小：

```c
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 16 * 1024 ) )
```

当前禁用了软件定时器：

```c
#define configUSE_TIMERS                        0
```

当前只保留了 `vTaskDelay()`：

```c
#define INCLUDE_vTaskDelay                      1
```

## 5. 中断处理移植

FreeRTOS 在 Cortex-M 上需要接管三个异常：

```text
SVC_Handler
PendSV_Handler
SysTick_Handler
```

在 `FreeRTOSConfig.h` 中映射为：

```c
#define vPortSVCHandler                         SVC_Handler
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler
```

因此，用户自己的 `stm32f4xx_it.c` 中不能再定义这三个函数，否则会链接时报重复定义。

当前已经从：

```text
Project\STM32F4xx_StdPeriph_Templates\stm32f4xx_it.c
```

移除了：

```c
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
```

SysTick 现在由 FreeRTOS 负责，不再使用裸机模板里的 `TimingDelay_Decrement()`。

## 6. main.c 启动流程

当前 `main.c` 的启动流程是：

```text
SystemInit()
  -> main()
      -> NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4)
      -> xTaskCreate()
      -> vTaskStartScheduler()
      -> FreeRTOS 开始调度
```

当前测试任务：

```c
static void HeartbeatTask(void *argument)
{
  (void)argument;

  for (;;)
  {
    ulHeartbeat++;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}
```

这个任务不依赖具体 LED 引脚，只用于确认调度器能正常运行。

## 7. 编译验证

使用 Keil 命令行验证：

```text
D:\l\UV4\UV4.exe -b D:\Rainwork\Project\STM32F4xx_StdPeriph_Templates\MDK-ARM\Project.uvprojx -t STM32F407VET6 -o D:\Rainwork\Project\STM32F4xx_StdPeriph_Templates\MDK-ARM\build_freertos.log
```

当前验证结果：

```text
"STM32F407VET6\STM32F407VET6.axf" - 0 Error(s), 0 Warning(s).
```

程序大小：

```text
Code=3452 RO-data=424 RW-data=124 ZI-data=17572
```

## 8. 常见问题

### 8.1 找不到 FreeRTOS.h

检查 Keil include path 是否包含：

```text
../../../Middlewares/FreeRTOS/Source/include
```

### 8.2 找不到 portmacro.h

检查 Keil include path 是否包含：

```text
../../../Middlewares/FreeRTOS/Source/portable/RVDS/ARM_CM4F
```

### 8.3 SVC/PendSV/SysTick 重复定义

原因是 `stm32f4xx_it.c` 里还保留了：

```c
SVC_Handler
PendSV_Handler
SysTick_Handler
```

解决方法：删除用户文件里的这三个函数，让 FreeRTOS port 接管。

### 8.4 进入 HardFault

优先检查：

```text
NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4)
configPRIO_BITS
configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
configLIBRARY_LOWEST_INTERRUPT_PRIORITY
```

当前配置：

```c
#define configPRIO_BITS                         4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY 15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5
```

### 8.5 xTaskCreate 失败

通常是 FreeRTOS heap 不够。

当前堆大小：

```c
#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( 16 * 1024 ) )
```

可以根据任务数量和栈大小适当调大。

## 9. 后续扩展

### 启用软件定时器

修改：

```c
#define configUSE_TIMERS                        1
```

并在 Keil 工程 FreeRTOS 分组中加入：

```text
Middlewares\FreeRTOS\Source\timers.c
```

同时配置：

```c
#define configTIMER_TASK_PRIORITY
#define configTIMER_QUEUE_LENGTH
#define configTIMER_TASK_STACK_DEPTH
```

### 启用事件组

在 Keil 工程 FreeRTOS 分组中加入：

```text
Middlewares\FreeRTOS\Source\event_groups.c
```

### 启用消息缓冲区

在 Keil 工程 FreeRTOS 分组中加入：

```text
Middlewares\FreeRTOS\Source\stream_buffer.c
```

## 10. 当前移植结论

当前工程已经完成 FreeRTOS 精简移植：

```text
Keil 工程可打开
FreeRTOS 源码引用完整
中断移植层无重复定义
ARMCC5 编译通过
0 Error(s), 0 Warning(s)
```
