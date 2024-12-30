#include "stm32f4xx_hal.h"
#include "main.h"
#include "mutex.h"

//hardware resource mutex(i2c/spi)

mutex_t i2c_mutex, spi_mutex;


//尝试把某互斥锁实例锁于某设备，0成功，1失败
uint8_t mutex_tryLock(mutex_t *instance, mutex_device_e device)
{
	//当目前互斥锁实例已经被某个设备上锁，则退出
	if(instance->locked)
		return 1;
	//不管互斥锁实例被谁拥有，只要没有上锁，就上锁于当前设备
	//上锁后当前设备具有访问硬件的权限
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


//尝试把某互斥锁实例于某设备解锁，相当于放弃控制权，0成功，1失败
uint8_t mutex_tryUnlock(mutex_t *instance, mutex_device_e device)
{
	//若实例不属于当前设备，则不进行任何操作
	if(instance->mutex_owner != device)
		return 1;
	//当目前已被自己上锁，则解锁
	else if(instance->locked)
	{
		instance->locked = 0;
		instance->accessibility = 0;
		return 0;
	}
}


//获取某设备在某互斥锁实例中的可访问性，1不可访问，0可访问
uint8_t mutex_getDeviceAcc(mutex_t *instance, mutex_device_e device)
{
	return(instance->accessibility && instance->mutex_owner == device);
}

