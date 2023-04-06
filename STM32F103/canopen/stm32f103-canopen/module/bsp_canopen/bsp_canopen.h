/***********************************************************************************************************************
* File Name    : bsp_canopen.h
* Version      : v1.0
* Description  : canopen协议
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#ifndef _BSP_CANOPEN_H_
#define _BSP_CANOPEN_H_

#ifdef __cplusplus
   extern "C" {
#endif

#include "tim.h"
#include "main.h"
#include "canfestival.h"
#include "Slave1.h"
#include "bsp_can.h"
#include "bsp_canopen_timer.h"
#include "applicfg.h"
#include "can_struct.h"

#define NMT_COB_ID_BASE 0x700
#define CLIENT_TO_SERVER_COB_ID_BASE 0x600   //Receive SDO
#define SERVER_TO_CLIENT_COB_ID_BASE 0x580   //Transmit SDO
#define TPDO1_COB_ID_BASE 0x180
#define TPDO2_COB_ID_BASE 0X280
#define TPDO3_COB_ID_BASE 0x380
#define TPDO4_COB_ID_BASE 0x480
#define RPDO1_COB_ID_BASE 0x200
#define RPDO2_COB_ID_BASE 0x300
#define RPDO3_COB_ID_BASE 0x400
#define RPDO4_COB_ID_BASE 0x500

/* 快速SDO指令 */
#define EX_SDO_WRITE_1_BYTE 0x2F
#define EX_SDO_WRTIE_2_BYTE 0x2B
#define EX_SDO_WRTIE_3_BYTE 0x27
#define EX_SDO_WRTIE_4_BYTE 0x23
#define EX_SDO_READ_1_BYTE 0x4F
#define EX_SDO_READ_2_BYTE 0x4B
#define EX_SDO_READ_3_BYTE 0x47
#define EX_SDO_READ_4_BYTE 0x43
#define EX_SDO_WRITE_SUCCESS 0x60
#define EX_SDO_EXCEPTIONAL_RESPONSE 0x80

/* 解析报文状态机的状态 */
typedef enum unpack_status
{
    wait_msg_head,
    wait_msg_id,
    wait_msg_len,
    wait_msg_rtr,
    wait_msg_buffer,
    wait_msg_end,

}_UNPACK_STATUS;

/* CANOPEN NMT管理报文     */
typedef enum NMT_module_control
{
    START_REMOTE_NODE           = 0x01,
    STOP_REMOTE_NODE            = 0x02,
    ENTER_PRE_OPERATIONAL_STATE = 0x80,
    RESET_NODE                  = 0x81,
    RESET_COMMUNICATION         = 0x82,
    
}_NMT_MODULE_CONTORL;

/* canopen结构体 */
typedef struct co_node
{
    uint32_t  node_id;      /* 站点号 */
    Message RxMSG;          /* 接收的报文 */
    CO_Data *object_DIC;    /* 对象字典 */

}CO_Node;


/* 私有函数 */
static uint8_t canopen_To_CAN_Send(Message *m);

/* 公有函数 */
void CANopen_Init(void);
unsigned char canSend(CAN_PORT notused, Message *m);
void CANopen_Get_MSG(struct co_node *CANopen_Handler,CAN_MsgNode *msg);
void send_NMT_Manage_Message(CO_Data* data,_NMT_MODULE_CONTORL nmt);

/* CANopen对象 */
extern CO_Node CANopen_Slave;

#ifdef __cplusplus
}
#endif


#endif /*__BSP_CANOPEN_H */





