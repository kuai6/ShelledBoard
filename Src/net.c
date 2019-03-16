#include "main.h"
#include "net.h"

#include <Ethernet/socket.h>


////////////////////////////////////////////////
// Shared Buffer Definition for LOOPBACK TEST //
////////////////////////////////////////////////
#define DATA_BUF_SIZE   2048
static uint8_t gDATABUF[DATA_BUF_SIZE];

///////////////////////////////////
// Default Network Configuration //
///////////////////////////////////
static wiz_NetInfo gWIZNETINFO = {
        .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
        .ip = {10, 10, 10, 5},
        .sn = {255,255,255,0},
        .gw = {10, 10, 10, 1},
        .dns = {0,0,0,0},
        .dhcp = NETINFO_STATIC
};

static SPI_HandleTypeDef *hspi = NULL;

static void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(hspi, buff, len, HAL_MAX_DELAY);
}

static void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(hspi, buff, len, HAL_MAX_DELAY);
}



static uint8_t wizchip_read() {
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

static void wizchip_write(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}


void wizchip_select(void) {
    HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_RESET);
}

void wizchip_deselect(void) {
    HAL_GPIO_WritePin(GPIOB, SPI2_CS_Pin, GPIO_PIN_SET);
}


void network_init(SPI_HandleTypeDef *phspi)
{
    uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};
    hspi = phspi;
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // First of all, Should register SPI callback functions implemented by user for accessing WIZCHIP //
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Chip selection call back */

#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
    reg_wizchip_cs_cbfunc(wizchip_select, wizchip_select);  // CS must be tried with LOW.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif


    /* SPI Read & Write callback function */
    reg_wizchip_spi_cbfunc(wizchip_read, wizchip_write);


    /* WIZCHIP SOCKET Buffer initialize */
    if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1)
    {
        //init fail
        while(1);
    }


    uint8_t tmpstr[6];

    ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);

    ctlwizchip(CW_GET_ID,(void*)tmpstr);
}


