#include "main.h"
#include "simi2c.h"
#include "timer.h"



//IIC����д
//addr:������ַ
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//    ����,�������
uint8_t bsp_simi2c_write_len(uint8_t dev_addr, uint8_t reg,uint8_t len,uint8_t *buf)
{
    uint8_t i;
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|0);//����������ַ+д����
    if(IIC_Wait_Ack())	//�ȴ�Ӧ��
    {
//        IIC_Stop();
//        return 1;
    }
    IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    for(i=0; i<len; i++)
    {
        IIC_Send_Byte(buf[i]);	//��������
        if(IIC_Wait_Ack())		//�ȴ�ACK
        {
//            IIC_Stop();
//            return 1;
        }
    }
    IIC_Stop();
    return 0;
}
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//    ����,�������
uint8_t bsp_simi2c_read_len(uint8_t dev_addr, uint8_t reg,uint8_t len,uint8_t *buf)
{
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|0);//����������ַ+д����
    if(IIC_Wait_Ack())	//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|1);//����������ַ+������
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    while(len)
    {
        if(len==1)*buf=IIC_Read_Byte(0);//������,����nACK
        else *buf=IIC_Read_Byte(1);		//������,����ACK
        len--;
        buf++;
    }
    IIC_Stop();	//����һ��ֹͣ����
    return 0;
}
//IICдһ���ֽ�
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//    ����,�������
uint8_t bsp_simi2c_write_byte(uint8_t dev_addr, uint8_t reg,uint8_t data)
{
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|0);//����������ַ+д����
    if(IIC_Wait_Ack())	//�ȴ�Ӧ��
    {
//        IIC_Stop();
//        return 1;
    }
    IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    IIC_Send_Byte(data);//��������
    if(IIC_Wait_Ack())	//�ȴ�ACK
    {
//        IIC_Stop();
//        return 1;
    }
    IIC_Stop();
    return 0;
}
//IIC��һ���ֽ�
//reg:�Ĵ�����ַ
//����ֵ:����������
uint8_t bsp_simi2c_read_byte(uint8_t dev_addr, uint8_t reg)
{
    uint8_t res;
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|0);//����������ַ+д����
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    IIC_Send_Byte(reg);	//д�Ĵ�����ַ
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    IIC_Start();
    IIC_Send_Byte((dev_addr<<1)|1);//����������ַ+������
    IIC_Wait_Ack();		//�ȴ�Ӧ��
    res=IIC_Read_Byte(0);//��ȡ����,����nACK
    IIC_Stop();			//����һ��ֹͣ����
    return res;
}




/*
*********************************************************************************************************
*	�� �� ��: IIC_CheckDevice
*	����˵��: ���IIC�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    �Σ�_Address���豸��IIC���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t IIC_CheckDevice(uint8_t _Address)
{
    uint8_t ucAck;
     /* ��һ��ֹͣ�ź�, ��λIIC�����ϵ������豸������ģʽ */
    IIC_Stop();
    IIC_Start();		/* ���������ź� */
    /* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
    IIC_Send_Byte(_Address|IIC_WR);
    ucAck = IIC_Wait_Ack();	/* ����豸��ACKӦ�� */
    IIC_Stop();			/* ����ֹͣ�ź� */

    return ucAck;
}


/*
*********************************************************************************************************
*	�� �� ��: IIC_SendByte
*	����˵��: CPU��IIC�����豸����8bit����
*	��    �Σ�_ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Send_Byte(uint8_t _ucByte)
{
    uint8_t i;
    /* �ȷ����ֽڵĸ�λbit7 */
    for (i = 0; i < 8; i++)
    {
        if (_ucByte & 0x80)
        {
            IIC_SDA_1();
        }
        else
        {
            IIC_SDA_0();
        }
        IIC_Delay();
        IIC_SCL_1();
        IIC_Delay();
        IIC_SCL_0();
        if (i == 7)
        {
            IIC_SDA_1(); // �ͷ�����
        }
        _ucByte <<= 1;	/* ����һ��bit */
        IIC_Delay();
    }
}
 
/*
*********************************************************************************************************
*	�� �� ��: IIC_ReadByte
*	����˵��: CPU��IIC�����豸��ȡ8bit����
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t IIC_Read_Byte(uint8_t ack)
{
    uint8_t i;
    uint8_t value;

    /* ������1��bitΪ���ݵ�bit7 */
    value = 0;
		IIC1_SDA_IN();
    for (i = 0; i < 8; i++)
    {
        value <<= 1;
        IIC_SCL_1();
        IIC_Delay();
        if (IIC_SDA_READ())
            value++;
        IIC_SCL_0();
        IIC_Delay();
    }
    if(ack==0)
        IIC_NAck();
    else
        IIC_Ack();
		IIC1_SDA_OUT();
    return value;
}


/*
*********************************************************************************************************
*	�� �� ��: IIC_Start
*	����˵��: CPU����IIC���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Start(void)
{
    /* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾIIC���������ź� */
    IIC_SDA_1();
    IIC_SCL_1();
    IIC_Delay();
    IIC_SDA_0();
    IIC_Delay();
    IIC_SCL_0();
    IIC_Delay();
}

/*
*********************************************************************************************************
*	�� �� ��: IIC_Start
*	����˵��: CPU����IIC����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Stop(void)
{
    /* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾIIC����ֹͣ�ź� */
    IIC_SDA_0();
    IIC_SCL_1();
    IIC_Delay();
    IIC_SDA_1();
}


/*
*********************************************************************************************************
*	�� �� ��: IIC_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t IIC_Wait_Ack(void)
{
    uint8_t re;

		IIC1_SDA_IN();
	
    IIC_SDA_1();	/* CPU�ͷ�SDA���� */
    IIC_Delay();
    IIC_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
    IIC_Delay();
    if (IIC_SDA_READ())	/* CPU��ȡSDA����״̬ */
    {
        re = 1;
    }
    else
    {
        re = 0;
    }
    IIC_SCL_0();
    IIC_Delay();
		
		IIC1_SDA_OUT();
    return re;
}

/*
*********************************************************************************************************
*	�� �� ��: IIC_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Ack(void)
{
    IIC_SDA_0();	/* CPU����SDA = 0 */
    IIC_Delay();
    IIC_SCL_1();	/* CPU����1��ʱ�� */
    IIC_Delay();
    IIC_SCL_0();
    IIC_Delay();
    IIC_SDA_1();	/* CPU�ͷ�SDA���� */
}

/*
*********************************************************************************************************
*	�� �� ��: IIC_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_NAck(void)
{
    IIC_SDA_1();	/* CPU����SDA = 1 */
    IIC_Delay();
    IIC_SCL_1();	/* CPU����1��ʱ�� */
    IIC_Delay();
    IIC_SCL_0();
    IIC_Delay();
}


