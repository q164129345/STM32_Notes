/***********************************************************************************************************************
* File Name    : bsp_canopen_timer.c
* Version      : v1.0
* Description  : canopen协议的定时器
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_canopen_timer.h"
#include "stdio.h"

/* 定时器TIM相关变量 */
static TIMEVAL last_counter_val = 0;
static TIMEVAL elapsed_time = 0;

/************************************************
函数名称 ： setTimer
功	 能 ： Set the timer for the next alarm.
参	 数 ： value --- 参数
返 回 值 ： 无
作	 者 ： strongerHuang
*************************************************/
void setTimer(TIMEVAL value)
{
    uint32_t timer = __HAL_TIM_GetCounter(CANOPEN_TIMx); // Copy the value of the running timer 
    elapsed_time += timer - last_counter_val;
    last_counter_val = CANOPEN_TIM_PERIOD - value;
    __HAL_TIM_SetCounter(CANOPEN_TIMx, CANOPEN_TIM_PERIOD - value);
    HAL_TIM_Base_Start_IT(CANOPEN_TIMx);
}

/************************************************
函数名称 ： getElapsedTime
功	 能 ： Return the elapsed time to tell the Stack how much time is spent since last call.
参	 数 ： 无
返 回 值 ： (消逝的)时间
作	 者 ： strongerHuang
*************************************************/
TIMEVAL getElapsedTime(void)
{
    uint32_t timer = __HAL_TIM_GetCounter(CANOPEN_TIMx); // Copy the value of the running timer
    if(timer < last_counter_val)
    timer += CANOPEN_TIM_PERIOD;
    TIMEVAL elapsed = timer - last_counter_val + elapsed_time;
    return elapsed;
}

  
/************************************************
 函数名称 ： TIMx_DispatchFromISR
 功	 能 ： 定时调度(从定时器中断)
 参	 数 ： 无
 返 回 值 ： 无
 作	 者 ： strongerHuang
*************************************************/
void TIMx_DispatchFromISR(void)
{
    last_counter_val = 0;
    elapsed_time = 0;
    TimeDispatch();
}


/* 因为FreeRTOS的已经定义了HAL_TIM_PeriodElapsedCallback，所以这里要备注掉，
 * 然后把TIMx_DispatchFromISR()放到main.c的HAL_TIM_PeriodElapsedCallback里
 */
 
//  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//  {
//  	
//  	  if (htim == CANOPEN_TIMx )
//	  {
//		  TIMx_DispatchFromISR();
//		  //printf("进入定时器tim2\r\n");
//	  }
//	  
//
//
//  }

