/***********************************************************************************************************************
* File Name    : bsp_canopen_timer.c
* Version      : v1.0
* Description  : canopen协议的定时器
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#ifndef _BSP_CANOPEN_TIMER_H_
#define _BSP_CANOPEN_TIMER_H_

#ifdef __cplusplus
   extern "C" {
#endif

#include "main.h"
#include "canfestival.h"
#include "tim.h"



//该文件的接口配置 根据cubeMX设置进行修改
#define CANOPEN_TIMx              (TIM_HandleTypeDef *)&htim4
#define CANOPEN_TIM_PERIOD        65535          //定时周期 与cubeMX中设置相同

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void TIMx_DispatchFromISR(void);


/***********************************************/
#ifdef __cplusplus
}
#endif

#endif /*__BSP_CANOPEN_TIMER_H */



