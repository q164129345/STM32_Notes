/***********************************************************************************************************************
* File Name    : bsp_canopen.c
* Version      : v1.0
* Description  : canopen协议
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_canopen.h"

/* 外部变量 */
extern CO_Data Slave1_Data;

/* 内部变量 */
static TimerCallback_t init_callback;
//s_BOARD MasterBoard = {"1", "1M"};                                    

CO_Node CANopen_Slave;       /* CANopen对象1 */

/**
  * 函数功能: 初始化CANopen对象
  * 输入参数: void
  * 返回值：  void
  * 说明:
  */
void CANopen_handler_init(void)
{
    /* 初始化CANopen_Master1对象 */
    CANopen_Slave.node_id = 0x01;                   /* Node_ID */
    CANopen_Slave.object_DIC = &Slave1_Data;        /* 对象字典 */
    CANopen_Slave.object_DIC->canHandle = (CAN_PORT)CANopen_Slave.node_id;  /* 设置接口函数canSend的通道 */
}

/**
  * 函数功能: 将canopen协议的报文，从FDCAN发出去。
  * 输入参数: Message *m
  * 返回值：  void
  * 说明:
  */
static uint8_t canopen_To_CAN_Send(Message *m)
{
    uint32_t can_rtr = 0;
    
    if(m->rtr == 1)
        can_rtr = CAN_RTR_REMOTE;  //远程帧
    else
        can_rtr = CAN_RTR_DATA;    //数据帧
    
    CAN_SendData(m->cob_id,m->data,m->len,can_rtr);  //将CAN发送出去
    
    return 0xFF;
}


/**
  * 函数功能: 发送NMT管理报文
  * 输入参数: data：对象字典结构体
              nmt:  NMT管理报文
  * 返 回 值: 无
  * 说    明:
  *    
  */
void send_NMT_Manage_Message(CO_Data* data,_NMT_MODULE_CONTORL nmt)
{
    switch (nmt)
    {
        case START_REMOTE_NODE:
            masterSendNMTstateChange(data,0x00,nmt);
            break;
        case STOP_REMOTE_NODE:
            masterSendNMTstateChange(data,0x00,nmt);
            break;
        case ENTER_PRE_OPERATIONAL_STATE:
            masterSendNMTstateChange(data,0x00,nmt);
            break;
        case RESET_NODE:
            masterSendNMTstateChange(data,0x00,nmt);
            break;
        case RESET_COMMUNICATION:
            masterSendNMTstateChange(data,0x00,nmt);
            break;
    }  
}


/**
  * 函数功能: CANopen，Node-ID初始化
  * 输入参数: notused：结构体 m：数据
  * 返 回 值: 无
  * 说    明: 
  *         1. 只初始化FDCAN1的CANopen Node,如果需要FDCAN2的CANopen Node就需要在文件夹Object_dir添加对象文件(.c与.h)
  *            然后，将这个函数的CANopen 节点2初始化的备注删除即可。
  */
void InitNodes(CO_Data* d, UNS32 id)
{
    /****************************** INITIALISATION MASTER *******************************/
    /* Canopen 节点1 初始化 */
    //printf("CANopen Node of CAN1 is going to initialize.\r\n");
    setNodeId(CANopen_Slave.object_DIC,CANopen_Slave.node_id);
    /* Master1 init */
    setState(CANopen_Slave.object_DIC,Initialisation);
    setState(&Slave1_Data, Pre_operational);
    //printf("CANopen Node of CAN1 was successfully initialized.\r\n");
}


/**
  * 函数功能: 开启定时器
  * 输入参数:
  *           
  * 返回值：
  *           
  *
  * 说明:
  *
  */
void StartTimerLoop(TimerCallback_t _init_callback) 
{
    init_callback = _init_callback;
    SetAlarm(NULL, 0, init_callback, 0, 0);
}

/**
  * 函数功能: 初始化Canopen节点
  * 输入参数:
  * 返回值：
  * 说明:
  */
//void CANopen_Init(void *parameter)
void CANopen_Init(void)
{
    //printf("Starting to Init the CANopen object. \n");
    CANopen_handler_init();
    StartTimerLoop(&InitNodes);
}


/**
  * 函数功能: canfestival的接口函数(canSend)
  * 输入参数: notused：结构体 m：数据
  * 返 回 值: 无
  * 说    明:
  *       1. 通过入口参数canHandler，决定使用FDCAN1硬件还是FDCAN2硬件
  *       2. 第一个入口参数，默认是CO_Data结构体的成员canHandle
  */
unsigned char canSend(CAN_PORT canHandle, Message *m)
{
    /* 看看是哪一个canopen Node需要发送报文 */
    if(CANopen_Slave.object_DIC->canHandle == canHandle)
    {
        return canopen_To_CAN_Send(m);
    }
    
    /* 可以兼容两路CAN */
    return 0;
}

/**
  * 函数功能: 将接收到的CAN报文，移交给canopen协议
  * 输入参数: _CANMSG *msg
  * 返回值：  void
  * 说明:
  */
void CANopen_Get_MSG(struct co_node *CANopen_Handler,CAN_MsgNode *msg)
{
    if(&CANopen_Slave == CANopen_Handler)
    {
        CANopen_Slave.RxMSG.cob_id = (UNS16)msg->sid;            /* 获取cob id */
        CANopen_Slave.RxMSG.len =    (UNS8)msg->length;          /* 获取报文长度 */
        CANopen_Slave.RxMSG.rtr =    (UNS8)msg->rtr;             /* 获取报文的类型 */
        memcpy(CANopen_Slave.RxMSG.data,(uint8_t *)msg->data,8); /* 获取8为报文 */

        /* canopen接口函数 */
        canDispatch(&Slave1_Data,&CANopen_Slave.RxMSG);
    }

}














