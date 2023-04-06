/***********************************************************************************************************************
* File Name    : bsp_canopen_timer.c
* Version      : v1.0
* Description  : canopenЭ��Ķ�ʱ��
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_canopen_timer.h"
#include "stdio.h"

/* ��ʱ��TIM��ر��� */
static TIMEVAL last_counter_val = 0;
static TIMEVAL elapsed_time = 0;

/************************************************
�������� �� setTimer
��	 �� �� Set the timer for the next alarm.
��	 �� �� value --- ����
�� �� ֵ �� ��
��	 �� �� strongerHuang
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
�������� �� getElapsedTime
��	 �� �� Return the elapsed time to tell the Stack how much time is spent since last call.
��	 �� �� ��
�� �� ֵ �� (���ŵ�)ʱ��
��	 �� �� strongerHuang
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
 �������� �� TIMx_DispatchFromISR
 ��	 �� �� ��ʱ����(�Ӷ�ʱ���ж�)
 ��	 �� �� ��
 �� �� ֵ �� ��
 ��	 �� �� strongerHuang
*************************************************/
void TIMx_DispatchFromISR(void)
{
    last_counter_val = 0;
    elapsed_time = 0;
    TimeDispatch();
}


/* ��ΪFreeRTOS���Ѿ�������HAL_TIM_PeriodElapsedCallback����������Ҫ��ע����
 * Ȼ���TIMx_DispatchFromISR()�ŵ�main.c��HAL_TIM_PeriodElapsedCallback��
 */
 
//  void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//  {
//  	
//  	  if (htim == CANOPEN_TIMx )
//	  {
//		  TIMx_DispatchFromISR();
//		  //printf("���붨ʱ��tim2\r\n");
//	  }
//	  
//
//
//  }

