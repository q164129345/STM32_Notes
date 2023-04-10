/***********************************************************************************************************************
* File Name    : bsp_can.h
* Version      : v1.0
* Description  : can ��������
* Creation Date: 2023.3.08
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#include "main.h"

#ifdef __cplusplus  
extern "C" {  
#endif
    
/* ֡���� */
typedef enum
{
    DATA_FRAME   = CAN_RTR_DATA,   //����֡
    REMOTE_FRAME = CAN_RTR_REMOTE, //Զ��֡
}_FRAME_TYPE;

typedef struct
{
    uint8_t           data[8];   //���� 
    __IO _FRAME_TYPE  rtr;       //֡����
    __IO uint16_t     length;    //CAN���ĵĳ��� 
    __IO uint32_t     sid;       //��׼֡ID

}CAN_MsgNode;

/* �ⲿ���� */
void CAN_Filter_Config(void);
uint8_t CAN_SendData(uint32_t std_id, uint8_t *msg,uint8_t len,uint32_t rtr);
uint8_t CAN_SendMsg(uint32_t std_id, uint8_t* msg,uint32_t len);
void CAN_Module_Init(void);

/* ���Է��͵ĺ��� */
void CAN_TEST_Send(void);

#ifdef __cplusplus
} 
#endif

#endif


