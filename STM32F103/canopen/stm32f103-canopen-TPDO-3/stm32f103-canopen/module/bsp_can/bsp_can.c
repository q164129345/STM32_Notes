/***********************************************************************************************************************
* File Name    : bsp_can.c
* Version      : v1.0
* Description  : can ��������
* Creation Date: 2023.3.08
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_can.h"
#include "can.h"
#include "Slave1.h"
#include "bsp_canopen.h"

CAN_MsgNode g_CAN_Rec = {0,};
static CAN_RxHeaderTypeDef CAN1_RecHeader;    //CAN1���մ���Ԫ�Ķ�����

/*****************************************************************************************************
* describe: ����CAN���˲���������RXFIFO������RXFIFO�ж�
* input parameter: 
* output parameter: ��
* retval:��
* note:
*****************************************************************************************************/
void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef  sFilterConfig;

    sFilterConfig.FilterBank = 0;                       //�����˲�����0
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;   //����λģʽ
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  //32λ��
    sFilterConfig.FilterIdHigh = 0x0000;                //�൱�ڲ�����CAN ID
    sFilterConfig.FilterIdLow = 0x0000;                 //�൱�ڲ�����CAN ID
    sFilterConfig.FilterMaskIdHigh = 0x0000;            //�൱�ڲ�����CAN ID
    sFilterConfig.FilterMaskIdLow = 0x0000;             //�൱�ڲ�����CAN ID
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  //ʹ��RX_FIFO0
    sFilterConfig.FilterActivation = ENABLE;            //�����˲���
    sFilterConfig.SlaveStartFilterBank = 14;            //��ֻʹ��CAN1ʱ����λ��Ч��������˫·CANʱ�����ֵ����CAN2���˲����飨������CAN2���˲�������14��

    /* �����˲��� */
    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
   {
        while(1);
    } 
   /* ����CAN1������ */
   if (HAL_CAN_Start(&hcan) != HAL_OK)
   {
        while(1);
   }
   /* ʹ��CAN1�жϵķ�ʽ��RX_FIFOͨ�� */
   if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
   {
        while(1);
   }
}

/*****************************************************************************************************
* describe: can �������ݵײ�ӿ�
* input parameter: 
* @std_id-> ��׼ID
* @ext_id-> ��չID
* @msg-> Ҫ���͵�����ָ��
* @len-> ���͵����ݳ���
* output parameter: ��
* retval: 0 or 1
* note:
-  �ײ�ӿڣ����ֻ������8���ֽ�
*****************************************************************************************************/
uint8_t CAN_SendData(uint32_t std_id, uint8_t *msg,uint8_t len,uint32_t rtr)
{
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef canTxHeader;
    canTxHeader.StdId = std_id;
    canTxHeader.IDE = CAN_ID_STD;
    canTxHeader.RTR = rtr;    //����֡����Զ��֡
    canTxHeader.DLC = len;

    if(HAL_CAN_AddTxMessage(&hcan, &canTxHeader, msg, &TxMailbox) != HAL_OK)
        return 1;

    /* ����д����̫��Ч��������Ҫ��Ϊ�ź��������� */
    /* ȷ��Mailboxes��ȫ�ǿյ�ʱ�򣬲��뿪���� */
    // wait can mailbox is free
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3) 
    {
        HAL_Delay(1);
    }

    return 0;
}

/*****************************************************************************************************
* describe: can �������ݽӿ�
* input parameter: 
* @std_id-> ��׼ID
* @ext_id-> ��չID
* @msg-> Ҫ���͵�����ָ��
* @len-> ���͵����ݳ���
* output parameter: ��
* retval: 0 or 1
* note:
*****************************************************************************************************/
uint8_t CAN_SendMsg(uint32_t std_id, uint8_t* msg,uint32_t len)
{
    uint8_t sendLen;
    uint8_t *t_msg = msg;

    if (msg == NULL || len == 0) {
        return 1;
    }

    do {
        sendLen = len > 8 ? 8:len;

        if (CAN_SendData(std_id, t_msg, sendLen,CAN_RTR_DATA) == 1)
            return 1;


        len -= sendLen;
        t_msg += sendLen;
    }while (len);

    return 0;
}

/*****************************************************************************************************
* describe: can1�ж�ִ�к���
* input parameter: 
* @hcan-> can�������
* output parameter: ��
* retval: ��
* note:
*****************************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t rxBuf[8] = {0,};
    memset(g_CAN_Rec.data,0,8);                                         //�����һ�εı�������

    if(hcan->Instance == CAN1)
    {
        if(HAL_OK == HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN1_RecHeader,rxBuf))  //��FIFO0��ȡCAN����
        {
            g_CAN_Rec.length = CAN1_RecHeader.DLC;                      //��ȡ���ĵĳ���
            memcpy(g_CAN_Rec.data,rxBuf,g_CAN_Rec.length);              //��ȡ���ĵ�����
            g_CAN_Rec.sid = CAN1_RecHeader.StdId;                       //��ȡ��׼֡ID
            g_CAN_Rec.rtr = (_FRAME_TYPE)CAN1_RecHeader.RTR;            //��ȡ֡����
            /* ������Ե���ringbuffer����canopen */
            CANopen_Get_MSG(&CANopen_Slave,&g_CAN_Rec);
        }
    }
    /* ʹ��CAN1�жϵķ�ʽ��RX_FIFOͨ�� */
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*****************************************************************************************************
* describe: ����CAN����
* input parameter: 
* output parameter: ��
* retval:��
* note:
*****************************************************************************************************/
void CAN_TEST_Send(void)
{
    uint8_t data[8] = {1,2,3,4,5,6,7,8};
    CAN_SendData(0x100,data,0x08,CAN_RTR_DATA);
}

/*****************************************************************************************************
* describe: CANģ���ʼ��
* input parameter: 
* output parameter: ��
* retval:��
* note:
*****************************************************************************************************/
void CAN_Module_Init(void)
{
    CAN_Filter_Config();
}









