#include <zephyr.h>
#include <device.h>
#include <drivers/spi.h>
#include <drivers/gpio.h>
#include <devicetree.h>
#include <string.h>

#include <pinmux/stm32/pinmux_stm32.h>
#include "max31865.h"

static struct device *spiDevice = NULL;
struct spi_config spiCfg;
struct spi_cs_control csCtrl;

int8_t mySpiRead(uint8_t addr, uint8_t *Data, uint32_t len)
{
    int ret = 0;
    addr &= 0x7F; //Make sure topbits not set
    const struct spi_buf tx_buf = {
        .buf = &addr,
        .len = 1};
    const struct spi_buf_set tx = {
        .buffers = &tx_buf,
        .count = 1};
    struct spi_buf rx_buf[] = {
        {
            .buf = &addr,
            .len = 1,
        },
        {.buf = Data,
         .len = len}};
    const struct spi_buf_set rx = {
        .buffers = rx_buf,
        .count = 2};

    ret = spi_transceive(spiDevice, &spiCfg, &tx, &rx);

    return ret;
}

int8_t mySpiWrite(uint8_t addr, uint8_t *data, uint32_t len)
{
    uint8_t cmd[10];
    cmd[0] = addr;

    memcpy(&cmd[1], data, len);
    struct spi_buf bufs[] = {

        {.buf = cmd,
         .len = len + 1}};
    struct spi_buf_set tx = {
        .buffers = bufs,
        .count = 1};

    return spi_write(spiDevice, &spiCfg, &tx);
}


int initSPI()
{



    spiDevice = device_get_binding("SPI_1");

    if (!spiDevice)
    {
       
        return -ENODEV;
    }

    spiCfg.frequency = 1000000;
    spiCfg.operation =  SPI_OP_MODE_MASTER |SPI_MODE_CPHA | SPI_TRANSFER_MSB | SPI_WORD_SET(8);
    spiCfg.slave = (0);
    static const struct pin_config pin_config = {
		.pin_num = STM32_PIN_PA4,
		.mode = STM32_PINMUX_ALT_FUNC_5 | STM32_PUSHPULL_PULLUP,
	};

	stm32_setup_pins(&pin_config, 1);

    return 0;
}



void main(void)
{
    float temperature = -273.15;
    float resistance = 0;

    if (initSPI() < 0)
    {
        printf("Error\n");
    }
    else
    {
        printf("Start to measure Temperature\n");
        Max31865 max31865cfg;
        max31865cfg.spiRead = mySpiRead;
        max31865cfg.spiWrite = mySpiWrite;
        max31865cfg.resistanceAtZero = 100;
        max31865cfg.resistanceAtZero = 430;
        setControlBits(&max31865cfg, false, false, true, false, 2, 0);

        while(1){
            k_sleep(K_MSEC(1000));
            resistance = readResistance(&max31865cfg);
            temperature = calculateTemperature(&max31865cfg, resistance);
            printf("Resitance: %.2f, Temperature (C): %.2f\n", temperature);

        }
    }
}