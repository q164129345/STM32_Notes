/***********************************************************************************************************************
* File Name    : bsp_can.c
* Version      : v1.0
* Description  : can 总线驱动
* Creation Date: 2023.3.08
* Author       : wallace.zhang
* node         : 
***********************************************************************************************************************/
#include "bsp_can.h"
#include "can.h"
#include "Slave1.h"
#include "bsp_canopen.h"

CAN_MsgNode g_CAN_Rec = {0,};
static CAN_RxHeaderTypeDef CAN1_RecHeader;    //CAN1接收处理单元的对象句柄

/*****************************************************************************************************
* describe: 配置CAN的滤波器、关联RXFIFO、开启RXFIFO中断
* input parameter: 
* output parameter: 无
* retval:无
* note:
*****************************************************************************************************/
void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef  sFilterConfig;

    sFilterConfig.FilterBank = 0;                       //设置滤波器组0
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;   //屏蔽位模式
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;  //32位宽
    sFilterConfig.FilterIdHigh = 0x0000;                //相当于不过滤CAN ID
    sFilterConfig.FilterIdLow = 0x0000;                 //相当于不过滤CAN ID
    sFilterConfig.FilterMaskIdHigh = 0x0000;            //相当于不过滤CAN ID
    sFilterConfig.FilterMaskIdLow = 0x0000;             //相当于不过滤CAN ID
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;  //使用RX_FIFO0
    sFilterConfig.FilterActivation = ENABLE;            //激活滤波器
    sFilterConfig.SlaveStartFilterBank = 14;            //当只使用CAN1时，该位无效。当开启双路CAN时，这个值等于CAN2的滤波器组（本代码CAN2的滤波器组是14）

    /* 配置滤波器 */
    if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
   {
        while(1);
    } 
   /* 开启CAN1控制器 */
   if (HAL_CAN_Start(&hcan) != HAL_OK)
   {
        while(1);
   }
   /* 使能CAN1中断的方式与RX_FIFO通道 */
   if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
   {
        while(1);
   }
}

/*****************************************************************************************************
* describe: can 发送数据底层接口
* input parameter: 
* @std_id-> 标准ID
* @ext_id-> 拓展ID
* @msg-> 要发送的数据指针
* @len-> 发送的数据长度
* output parameter: 无
* retval: 0 or 1
* note:
-  底层接口，最大只允许发送8个字节
*****************************************************************************************************/
uint8_t CAN_SendData(uint32_t std_id, uint8_t *msg,uint8_t len,uint32_t rtr)
{
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef canTxHeader;
    canTxHeader.StdId = std_id;
    canTxHeader.IDE = CAN_ID_STD;
    canTxHeader.RTR = rtr;    //数据帧或者远程帧
    canTxHeader.DLC = len;

    if(HAL_CAN_AddTxMessage(&hcan, &canTxHeader, msg, &TxMailbox) != HAL_OK)
        return 1;

    /* 这样写法不太高效，后续需要改为信号量来处理 */
    /* 确保Mailboxes完全是空的时候，才离开函数 */
    // wait can mailbox is free
    while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan) != 3) 
    {
        HAL_Delay(1);
    }

    return 0;
}

/*****************************************************************************************************
* describe: can 发送数据接口
* input parameter: 
* @std_id-> 标准ID
* @ext_id-> 拓展ID
* @msg-> 要发送的数据指针
* @len-> 发送的数据长度
* output parameter: 无
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
* describe: can1中断执行函数
* input parameter: 
* @hcan-> can操作句柄
* output parameter: 无
* retval: 无
* note:
*****************************************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    uint8_t rxBuf[8] = {0,};
    memset(g_CAN_Rec.data,0,8);                                         //清除上一次的报文内容

    if(hcan->Instance == CAN1)
    {
        if(HAL_OK == HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&CAN1_RecHeader,rxBuf))  //从FIFO0获取CAN报文
        {
            g_CAN_Rec.length = CAN1_RecHeader.DLC;                      //获取报文的长度
            memcpy(g_CAN_Rec.data,rxBuf,g_CAN_Rec.length);              //获取报文的内容
            g_CAN_Rec.sid = CAN1_RecHeader.StdId;                       //获取标准帧ID
            g_CAN_Rec.rtr = (_FRAME_TYPE)CAN1_RecHeader.RTR;            //获取帧类型
            /* 这里可以导入ringbuffer或者canopen */
            CANopen_Get_MSG(&CANopen_Slave,&g_CAN_Rec);
        }
    }
    /* 使能CAN1中断的方式与RX_FIFO通道 */
    HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/*****************************************************************************************************
* describe: 测试CAN发送
* input parameter: 
* output parameter: 无
* retval:无
* note:
*****************************************************************************************************/
void CAN_TEST_Send(void)
{
    uint8_t data[8] = {1,2,3,4,5,6,7,8};
    CAN_SendData(0x100,data,0x08,CAN_RTR_DATA);
}

/*****************************************************************************************************
* describe: CAN模块初始化
* input parameter: 
* output parameter: 无
* retval:无
* note:
*****************************************************************************************************/
void CAN_Module_Init(void)
{
    CAN_Filter_Config();
}









