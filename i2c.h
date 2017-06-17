#ifndef __I2C_H 
#define __I2C_H
#include "Touch_DEF.h"

extern void I2C0_IRQHandler( void );
extern uint32_t I2CInit( uint32_t I2cMode );
uint8_t I2C_WriteNByte(uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);
uint8_t I2C_ReadNByte (uint8_t sla, uint8_t suba_type, uint32_t suba, uint8_t *s, uint32_t num);


#endif
