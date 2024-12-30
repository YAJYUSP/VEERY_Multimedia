/*
 * app_DRV2605L.c
 *
 *  Created on: Oct 17, 2021
 *  Author: Stephen Sheldon
 */

#include <drv2605l.h>
#include "simi2c.h"
#include "timer.h"


/**
 * Writes to 8 bit register on DRV2605L
 * @param reg		The number of the register to be written to.
 * @param data	The value to write to the register.
 */
static void drv2605l_write(uint8_t reg, uint8_t data)
{
	bsp_simi2c_write_len (I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&data);
}

/**
 * Reads an 8 bit register on the DRV2605L.
 * @param reg		The number of the register to be written to.
 * @param buff	THe value to write to the register.
 */
static void drv2605l_read(uint8_t reg, uint8_t *buff)
{
	bsp_simi2c_read_len (I2C_ADDR,reg,I2C_MEMADD_SIZE_8BIT,buff);
}

/**
 * Writes to the Mode Register.
 * Write 0x00 to get out of standby and use internal trigger (using GO command)
 * Write 0x01 to get out of standby + use External Trigger (edge triggered)
 * Write 0x02 to get out of standby + External Trigger (level triggered)
 * Write 0x03 to get out of standby + PWM input and analog output
 * Write 0x04 to use Audio-to-Vibe
 * Write 0x05 to use real time playback
 * Write 0x06 to perform a diagnostic - result stored in diagnostic bit in register 0x00
 * Write 0x07 to run auto calibration
 * @param mode Value associated with mode.
 */
static void drv2605l_set_mode(uint8_t mode)
{
	drv2605l_write(MODE_REG, mode);
}

/**
 * Selects the type of motor that's to be used (ERM or LRA).
 * Utilizes the Feedback Control Register.
 * @param val Corresponding val for either ERM or LRA motor.
 */
static void drv2605l_motor_select(uint8_t val)
{
	drv2605l_write(FEEDBACK_REG, val);
}

/**
 * Sets the library register to the specified TouchSense 2200 library.
 * @param lib TS2200 Library
 */
static void drv2605l_set_library(uint8_t lib)
{
	drv2605l_write(LIB_REG, lib);
}


/**
 * Returns the value stored in the status register (0x00).
 * @return Value stored in status register.
 */
static uint8_t drv2605l_get_status(void)
{
	uint8_t status;
	drv2605l_read(STATUS_REG, &status);
	return status;
}


/**
 * Checks the GO bit to see if there is currently waveform sequence being played
 * @return 1 if there is currently a waveform sequence being played
 * 				 0 if no waveform sequence is being played
 */
static uint8_t drv2605l_waveform_active(void) {
	uint8_t go_status;
	drv2605l_read(GO_REG, &go_status);
	return go_status;
}



/************************************************************************************************/

/**
 * Takes the STM32 I2C_HandleTypeDef to be utilized in communicating
 * with the DRV2605L
 * @param hi2cInit
 */
void bsp_drv2605l_init(void)
{
	drv2605l_set_mode(0x0); // Set mode to internal trigger input
	drv2605l_motor_select(0xB6); // LRA Motor
	drv2605l_set_library(0x06); // Select LRA library. 1-5 & 7 for ERM motors, 6 for LRA motors
}

/**
 * Sets the Go Register to execute the sequence of waveforms stored
 * within the sequence register.
 */
void bsp_drv2605l_go(void)
{
	drv2605l_write(GO_REG, 0x01);
}

/**
 * Sets the Go Register to stop executing the sequence of waveforms
 * stored within the sequence register.
 */
void bsp_drv2605l_stop(void)
{
	drv2605l_write(GO_REG, 0x0);
}

/**
 * There are 8 sequence register than can queue up to 8 waveforms. Sets the specified
 * sequence register with the specified waveform. See Page 60 of datasheet for list
 * of waveforms.
 * @param seq Sequence register to set.
 * @param wav Wave number to set seq reg to. There are 123 waveform effects.
 */
void bsp_drv2605l_set_waveform(uint8_t seq, uint8_t wav)
{
	drv2605l_write(WAVESEQ1+seq, wav);
}


