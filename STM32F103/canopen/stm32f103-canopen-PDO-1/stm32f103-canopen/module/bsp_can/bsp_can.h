/***********************************************************************************************************************
* File Name    : bsp_can.h
* Version      : v1.0
* Description  : can 总线驱动
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
    
/* 帧类型 */
typedef enum
{
    DATA_FRAME   = CAN_RTR_DATA,   //数据帧
    REMOTE_FRAME = CAN_RTR_REMOTE, //远程帧
}_FRAME_TYPE;

typedef struct
{
    uint8_t           data[8];   //数据 
    __IO _FRAME_TYPE  rtr;       //帧类型
    __IO uint16_t     length;    //CAN报文的长度 
    __IO uint32_t     sid;       //标准帧ID

}CAN_MsgNode;

/* 外部函数 */
void CAN_Filter_Config(void);
uint8_t CAN_SendData(uint32_t std_id, uint8_t *msg,uint8_t len,uint32_t rtr);
uint8_t CAN_SendMsg(uint32_t std_id, uint8_t* msg,uint32_t len);
void CAN_Module_Init(void);

/* 测试发送的函数 */
void CAN_TEST_Send(void);

#ifdef __cplusplus
} 
#endif

#endif


