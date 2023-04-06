/***********************************************************************************************************************
* File Name    : bsp_canopen.c
* Version      : v1.0
* Description  : canopenЭ��
* Creation Date: 2023.4.01
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_canopen.h"

/* �ⲿ���� */
extern CO_Data Slave1_Data;

/* �ڲ����� */
static TimerCallback_t init_callback;
//s_BOARD MasterBoard = {"1", "1M"};                                    

CO_Node CANopen_Slave;       /* CANopen����1 */

/**
  * ��������: ��ʼ��CANopen����
  * �������: void
  * ����ֵ��  void
  * ˵��:
  */
void CANopen_handler_init(void)
{
    /* ��ʼ��CANopen_Master1���� */
    CANopen_Slave.node_id = 0x01;                   /* Node_ID */
    CANopen_Slave.object_DIC = &Slave1_Data;        /* �����ֵ� */
    CANopen_Slave.object_DIC->canHandle = (CAN_PORT)CANopen_Slave.node_id;  /* ���ýӿں���canSend��ͨ�� */
}

/**
  * ��������: ��canopenЭ��ı��ģ���FDCAN����ȥ��
  * �������: Message *m
  * ����ֵ��  void
  * ˵��:
  */
static uint8_t canopen_To_CAN_Send(Message *m)
{
    uint32_t can_rtr = 0;
    
    if(m->rtr == 1)
        can_rtr = CAN_RTR_REMOTE;  //Զ��֡
    else
        can_rtr = CAN_RTR_DATA;    //����֡
    
    CAN_SendData(m->cob_id,m->data,m->len,can_rtr);  //��CAN���ͳ�ȥ
    
    return 0xFF;
}


/**
  * ��������: ����NMT������
  * �������: data�������ֵ�ṹ��
              nmt:  NMT������
  * �� �� ֵ: ��
  * ˵    ��:
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
  * ��������: CANopen��Node-ID��ʼ��
  * �������: notused���ṹ�� m������
  * �� �� ֵ: ��
  * ˵    ��: 
  *         1. ֻ��ʼ��FDCAN1��CANopen Node,�����ҪFDCAN2��CANopen Node����Ҫ���ļ���Object_dir��Ӷ����ļ�(.c��.h)
  *            Ȼ�󣬽����������CANopen �ڵ�2��ʼ���ı�עɾ�����ɡ�
  */
void InitNodes(CO_Data* d, UNS32 id)
{
    /****************************** INITIALISATION MASTER *******************************/
    /* Canopen �ڵ�1 ��ʼ�� */
    //printf("CANopen Node of CAN1 is going to initialize.\r\n");
    setNodeId(CANopen_Slave.object_DIC,CANopen_Slave.node_id);
    /* Master1 init */
    setState(CANopen_Slave.object_DIC,Initialisation);
    setState(&Slave1_Data, Pre_operational);
    //printf("CANopen Node of CAN1 was successfully initialized.\r\n");
}


/**
  * ��������: ������ʱ��
  * �������:
  *           
  * ����ֵ��
  *           
  *
  * ˵��:
  *
  */
void StartTimerLoop(TimerCallback_t _init_callback) 
{
    init_callback = _init_callback;
    SetAlarm(NULL, 0, init_callback, 0, 0);
}

/**
  * ��������: ��ʼ��Canopen�ڵ�
  * �������:
  * ����ֵ��
  * ˵��:
  */
//void CANopen_Init(void *parameter)
void CANopen_Init(void)
{
    //printf("Starting to Init the CANopen object. \n");
    CANopen_handler_init();
    StartTimerLoop(&InitNodes);
}


/**
  * ��������: canfestival�Ľӿں���(canSend)
  * �������: notused���ṹ�� m������
  * �� �� ֵ: ��
  * ˵    ��:
  *       1. ͨ����ڲ���canHandler������ʹ��FDCAN1Ӳ������FDCAN2Ӳ��
  *       2. ��һ����ڲ�����Ĭ����CO_Data�ṹ��ĳ�ԱcanHandle
  */
unsigned char canSend(CAN_PORT canHandle, Message *m)
{
    /* ��������һ��canopen Node��Ҫ���ͱ��� */
    if(CANopen_Slave.object_DIC->canHandle == canHandle)
    {
        return canopen_To_CAN_Send(m);
    }
    
    /* ���Լ�����·CAN */
    return 0;
}

/**
  * ��������: �����յ���CAN���ģ��ƽ���canopenЭ��
  * �������: _CANMSG *msg
  * ����ֵ��  void
  * ˵��:
  */
void CANopen_Get_MSG(struct co_node *CANopen_Handler,CAN_MsgNode *msg)
{
    if(&CANopen_Slave == CANopen_Handler)
    {
        CANopen_Slave.RxMSG.cob_id = (UNS16)msg->sid;            /* ��ȡcob id */
        CANopen_Slave.RxMSG.len =    (UNS8)msg->length;          /* ��ȡ���ĳ��� */
        CANopen_Slave.RxMSG.rtr =    (UNS8)msg->rtr;             /* ��ȡ���ĵ����� */
        memcpy(CANopen_Slave.RxMSG.data,(uint8_t *)msg->data,8); /* ��ȡ8Ϊ���� */

        /* canopen�ӿں��� */
        canDispatch(&Slave1_Data,&CANopen_Slave.RxMSG);
    }

}














