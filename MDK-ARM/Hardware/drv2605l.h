/*
 * app_DRV2605L.h
 *
 *  Created on: Oct 17, 2021
 *  Author: Stephen Sheldon
 */

#ifndef DRV2605L_H_
#define DRV2605L_H_

#include "stm32f4xx_hal.h"


/** I2C address of DRV2605L **/
#define I2C_ADDR 0x5A

/**
 * Status Register (0x00): The Device ID is bits 7-5. For DRV2605L it should only be 7 or 111.
 * Bits 4 and 2 are reserved.
 * Bit 3 is the diagnostic result. This flag stores the result of the auto-calibration routine. You want this to be 0 (auto-calibration passed).
 * Bit 1 is the over temp flag. You want this to be 0 (device is functioning normally).
 * Bit 0 is the over current flag. You want this to be 0 (no overcurrent event is detected)
 */
#define STATUS_REG 0x00

/**
 * Mode Register (0x01)
 * Default 01000000 -- Need to get it out of standby mode
 * Set to 0000 0000=0x00 to use Internal Trigger
 * Set to 0000 0001=0x01 to use External Trigger (edge mode) (like a switch on the IN pin)
 * Set to 0000 0010=0x02 to use External Trigger (level mode)
 * Set to 0000 0011=0x03 to use PWM input and analog output
 * Set to 0000 0100=0x04 to use Audio-to-Vibe
 * Set to 0000 0101=0x05 to use Real-time playback (RTP mode)
 * Set to 0000 0110=0x06 to perform a diagnostic test - result stored in Diagnostic bit in register 0x00
 * Set to 0000 0111=0x07 to run auto calibration
 */
#define MODE_REG 0x01

/**
 * Feedback Control Register (0x1A)
 * Bit 7: 0 for ERM, 1 for LRA -- Default is 0. This bit should be set prior to running auto calibration.
 * Bits 6-4 control brake factor
 * Bits 3-2 control the loop gain
 * Bits 1-0 control the BEMF gain
 */
#define FEEDBACK_REG 0x1A

/**
 * Real-Time Playback Register (0x02)
 * There are 6 ERM libraries
 */
#define RTP_REG 0x02

/**
 * Library Selection Register (0x03)
 * 0: Empty
 * 1: TS2200 Library A
 * 2: TS2200 Library B
 * 3: TS2200 Library C
 * 4: TS2200 Library D
 * 5: TS2200 Library E
 * 6: LRA Library
 * 7: TS2200 Library F
 */
#define LIB_REG 0x03

/** Waveform Sequencer Register (0x04 to 0x0B) **/
#define WAVESEQ1 0x04 // Bit 7: Set this to include a wait time between playback.
#define WAVESEQ2 0x05
#define WAVESEQ3 0x06
#define WAVESEQ4 0x07
#define WAVESEQ5 0x08
#define WAVESEQ6 0x09
#define WAVESEQ7 0x0A
#define WAVESEQ8 0x0B

/**
 * GO Register (0x0C)
 * Set to 0000 001=0x01 to set the GO bit.
 */
#define GO_REG 0x0C

/**
 * Overdrive Time Offset Register (0x0D)
 * Only useful in open loop mode.
 */
#define OVERDRIVE_REG 0x0D

/** Sustain Time Offset, Positive Register (0x0E) **/
#define SUSTAINOFFSETPOS_REG 0x0E

/** Sustain Time Offset, Negative Register (0x0F) **/
#define SUSTAINOFFSETNEG_REG 0x0F

/** Brake Time Offset Register (0x10) **/
#define BREAKTIME_REG 0x10

/** Audio-to-Vibe Control Register (0x11) **/
#define AUDIOCTRL_REG 0x11

/** Audio-to-Vibe Minimum Input Level Register (0x12) **/
#define AUDMINLVL_REG 0x12

/** Audio-to-Vibe Maximum Input Level Register (0x13) **/
#define AUDMAXLVL_REG 0x13

/** Audio-to-Vibe Minimum Output Level Register (0x14) **/
#define AUDMAXDRIVE_REG 0x14

/** Audio-to-Vibe Maximum Output Level Register (0x15) **/
#define AUDMINDRIVE_REG 0x15

/** Rated Voltage Register (0x16) **/
#define RATEDVOLTAGE_REG 0x16

/** Overdrive Clamp Voltage Register (0x17) **/
#define OVERDRIVECLAMP_REG 0x17

/** Auto-Calibration Compensation-Result Register (0x18) **/
#define COMPRESULT_REG 0x18

/** Auto-Calibration Back-EMF Result Register (0x19) **/
#define BACKEMF_REG 0x19

/**
 * Control1 Register (0x1B)
 * For AC coupling analog inputs and controlling drive time.
 */
#define CONTROL1_REG 0x1B

/**
 * Control2 Register (0x1C)
 * See page 48 of DRV2605L datasheet
 */
#define CONTROL2_REG 0x1C

/**
 * Control3 Register (0x1D)
 * See page 51 of DRV2605L datasheet
 */
#define CONTROL3_REG 0x1D

/**
 * Control4 Register (0x1E)
 * See page 52 of DRV2605L datasheet
 */
#define CONTROL4_REG 0x1E

/**
 * Control5 Register (0x1F)
 * See page 53 of DRV2605L datasheet
 */
#define CONTROL5_REG 0x1F

/**
 * LRA Open Loop Period Register (0x20)
 * This register sets the period to be used for driving an LRA when
 * open loop mode is elected.
 */
#define OLP_REG 0x20

/**
 * V(BAT) Voltage-Monitor Register (0x21)
 * This bit provides a real-time reading of the supply voltage at
 * the VDD pin. The device must be actively sending a waveform to take
 * a reading. Vdd=Vbatt[7:0]*5.6V/255
 */
#define VBATMONITOR_REG 0x21

/**
 * LRA Resonance-Period Register (0x22)
 * This bit reports the measurement of the LRA resonance period.
 */
#define LRARESPERIOD_REG 0x22

void bsp_drv2605l_init(void);
void bsp_drv2605l_set_waveform(uint8_t seq, uint8_t wav);
void bsp_drv2605l_go(void);
void bsp_drv2605l_stop(void);


#endif /* DRV2605L_H_ */
