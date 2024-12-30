#ifndef _MUTEX_H
#define _MUTEX_H

#include <stdint.h>

typedef struct
{
	//mutex instance owner, only mutex owner could access the h/w.
	uint8_t mutex_owner;
	//boolean var, which decides the accessibility of current owner.
	//0 is unaccessable, 1 is accessable
	uint8_t accessibility;
	//0 is unlocked, 1 is locked
	uint8_t locked;
	
}mutex_t;

typedef enum
{
	W25QXX,
	NRF24,
	ST25DV,
	MPU6050
}mutex_device_e;

extern mutex_t i2c_mutex, spi_mutex;


uint8_t mutex_tryLock(mutex_t *instance, mutex_device_e device);
uint8_t mutex_tryUnlock(mutex_t *instance, mutex_device_e device);
uint8_t mutex_getDeviceAcc(mutex_t *instance, mutex_device_e device);


#endif
