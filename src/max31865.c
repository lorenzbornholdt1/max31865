#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "max31865.h"
static void setBias(Max31865 * cfg,bool enable) ;

static void setBias(Max31865 * cfg,bool enable) {
	if (enable) {
		SETBIT(cfg->controlBits, 7);
	} else {
		CLEARBIT(cfg->controlBits, 7);
	}	
}

void setControlBits(Max31865 *cfg, bool filter50hz, bool conversionMode, bool oneShot, bool enableBias, uint8_t nmbrOfWires, uint8_t faultCycle)
{

    cfg->controlBits = cfg->controlBits | (filter50hz ? 0x01 : 0);
    cfg->controlBits = cfg->controlBits | (conversionMode ? 0x40 : 0);
    cfg->controlBits = cfg->controlBits | (oneShot ? 0x20 : 0);
    if (nmbrOfWires == 3)
    {
        cfg->controlBits = cfg->controlBits | 0x10;
    }
    cfg->controlBits = cfg->controlBits | (faultCycle & 0b00001100);
    setBias(cfg,enableBias );

    uint8_t cmd[] = {cfg->controlBits};

    cfg->spiWrite(WRITE_REGISTER(MAX_REG_CONFIG), cmd,1);
}

float readResistance(Max31865 *cfg){
    float resistance =  0;
    uint8_t cmd[] = {0b10100000};
    uint8_t read_reg_u8[2]={0,2};
	uint16_t read_reg_u16  = 0;
    setBias(cfg,true );

    

    cfg->spiWrite(WRITE_REGISTER(MAX_REG_CONFIG), cmd,1);

    int err = cfg->spiRead((MAX_REG_RTD_MSB), read_reg_u8, 2);
    if (err < 0)
	{
		return -1;
	}
    read_reg_u16 = read_reg_u8[0];
	read_reg_u16 <<= 8;
	read_reg_u16 |= read_reg_u8[1];

	if (TESTBIT(read_reg_u16, 0))
	{
		return -1;
	}
    read_reg_u16 = read_reg_u16 >> 1;
    resistance = (float)read_reg_u16;
	resistance /=32768;
	resistance *= cfg->resistanceReference;
        cmd[0] = 0b100000;

    

    cfg->spiWrite(WRITE_REGISTER(MAX_REG_CONFIG), cmd,1);
    return resistance;

}

float calculateTemperature(Max31865 *cfg, float resistance){
float Z1 = 0, Z2 =0 , Z3=0, Z4=0, Rt=0, temp=0;

    Z1 = -RTD_A;
    Z2 = RTD_A * RTD_A - (4 * RTD_B);
    Z3 = (4 * RTD_B) / cfg->resistanceAtZero;
    Z4 = 2 * RTD_B;

    temp = Z2 + (Z3 * Rt);
    temp = (sqrt(temp) + Z1) / Z4;

    if (temp >= 0) return temp;

    
    Rt /= cfg->resistanceAtZero;
    Rt *= 100;      // normalize to 100 ohm

    float rpoly = Rt;

    temp = -242.02;
    temp += 2.2228 * rpoly;
    rpoly *= Rt;  // square
    temp += 2.5859e-3 * rpoly;
    rpoly *= Rt;  // ^3
    temp -= 4.8260e-6 * rpoly;
    rpoly *= Rt;  // ^4
    temp -= 2.8183e-8 * rpoly;
    rpoly *= Rt;  // ^5
    temp += 1.5243e-10 * rpoly;

    return temp;

}