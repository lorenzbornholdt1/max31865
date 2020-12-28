
#ifndef _H_MAX31865
#define _H_MAX31865

#include <stdint.h>

#define MAX_REG_CONFIG                  0x00
#define MAX_REG_RTD_MSB                 0x01
#define MAX_REG_RTD_LSB                 0x02
#define MAX_REG_HIGH_FAULT_THR_MSB      0x03
#define MAX_REG_HIGH_FAULT_THR_LSB      0x04
#define MAX_REG_LOW_FAULT_THR_MSB       0x05
#define MAX_REG_LOW_FAULT_THR_LSB       0x06
#define MAX_REG_FAULT_STATUS            0x07
#define WRITE_REGISTER(reg)                     ( (reg) | 0x80 )
#define RTD_A         3.9080e-3
#define RTD_B         -5.870e-7

#ifndef BIT
#define BIT(n)  (1UL << (n))
#endif

#define SETBIT(x,y)        (x) |= BIT(y)
#define CLEARBIT(x,y)      (x) &= ~BIT(y)
#define TESTBIT(data,pos)  ((0u == (data & BIT(pos)))?0u:1u)

typedef int8_t (*max31865fptr_t)(uint8_t addr, uint8_t *Data, uint32_t len);

typedef struct {
    uint8_t controlBits;
    uint16_t resistanceReference;
    uint16_t resistanceAtZero;
    max31865fptr_t spiWrite;
    max31865fptr_t spiRead;
}Max31865;

void setControlBits(Max31865 *cfg, bool filter50hz, bool conversionMode, bool oneShot, bool enableBias, uint8_t nmbrOfWires, uint8_t faultCycle);
float readResistance(Max31865 *cfg);
float calculateTemperature(Max31865 *cfg, float resistance);

#endif