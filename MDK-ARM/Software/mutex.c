#include "stm32f4xx_hal.h"
#include "main.h"
#include "mutex.h"

//hardware resource mutex(i2c/spi)

mutex_t i2c_mutex, spi_mutex;


//���԰�ĳ������ʵ������ĳ�豸��0�ɹ���1ʧ��
uint8_t mutex_tryLock(mutex_t *instance, mutex_device_e device)
{
	//��Ŀǰ������ʵ���Ѿ���ĳ���豸���������˳�
	if(instance->locked)
		return 1;
	//���ܻ�����ʵ����˭ӵ�У�ֻҪû���������������ڵ�ǰ�豸
	//������ǰ�豸���з���Ӳ����Ȩ��
	else if(!instance->locked)
	{
		if(instance->mutex_owner != device)
			instance->mutex_owner = device;
		instance->locked = 1;
	}
	if(instance->locked)
		instance->accessibility = 1;
	else
		instance->accessibility = 0;
	return 0;
}


//���԰�ĳ������ʵ����ĳ�豸�������൱�ڷ�������Ȩ��0�ɹ���1ʧ��
uint8_t mutex_tryUnlock(mutex_t *instance, mutex_device_e device)
{
	//��ʵ�������ڵ�ǰ�豸���򲻽����κβ���
	if(instance->mutex_owner != device)
		return 1;
	//��Ŀǰ�ѱ��Լ������������
	else if(instance->locked)
	{
		instance->locked = 0;
		instance->accessibility = 0;
		return 0;
	}
}


//��ȡĳ�豸��ĳ������ʵ���еĿɷ����ԣ�1���ɷ��ʣ�0�ɷ���
uint8_t mutex_getDeviceAcc(mutex_t *instance, mutex_device_e device)
{
	return(instance->accessibility && instance->mutex_owner == device);
}

