/***********************************************************************************************************************
* File Name    : bsp_canopen_timer.c
* Version      : v1.0
* Description  : canopenЭ��Ķ�ʱ��
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



//���ļ��Ľӿ����� ����cubeMX���ý����޸�
#define CANOPEN_TIMx              (TIM_HandleTypeDef *)&htim4
#define CANOPEN_TIM_PERIOD        65535          //��ʱ���� ��cubeMX��������ͬ

//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void TIMx_DispatchFromISR(void);


/***********************************************/
#ifdef __cplusplus
}
#endif

#endif /*__BSP_CANOPEN_TIMER_H */



