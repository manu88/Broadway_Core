//
//  GpioPlateformImpl.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 16/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//




#include "GpioPlateformImpl.h"

#ifdef TARGET_RASPBERRY_PI
#include <bcm2835.h>

#elif defined TARGET_CUBIE2
#include "SunxiGpio.h"

volatile void* GpioPlateformImplementation::s_gpio_map = NULL;
unsigned int GpioPlateformImplementation::s_sunxi_pio_base = -1;
#endif





GpioPlateformImplementation::GpioPlateformImplementation( const int pin, const GPioInputType type ):
_pin        ( pin  ),
_inputType  ( type )
{
    internalPinSetupInput( _inputType );
    
}

GpioPlateformImplementation::~GpioPlateformImplementation()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GpioPlateformImplementation::setGpio( const int pin , const GpioState state)
{

    internalPinSetupOutput( pin );
    internalSetOutput( pin ,  state);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioState GpioPlateformImplementation::read()
{

    return internalRead();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Internal routines, specific to each target.
    Implemented plateforms are:
     - RaspberryPI ( via bcm2835 lib )
     - Cubieboard  ( via sunxi defs  )
     - dummy-that-does-nothing for MacOS so I can code :-) 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Plateform specific init routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef TARGET_RASPBERRY_PI
/*static*/ bool GpioPlateformImplementation::init()
{
    printf("\n INIT BCM2835");
    return bcm2835_init();
}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
/*static*/ bool GpioPlateformImplementation::init()
{
    int fd;
    unsigned int addr_start, addr_offset;
    unsigned int PageSize, PageMask;
    
    
    fd = open("/dev/mem", O_RDWR);
    if(fd < 0) {
        return false;
    }
    
    PageSize = sysconf(_SC_PAGESIZE);
    PageMask = (~(PageSize-1));
    
    addr_start = SW_PORTC_IO_BASE & PageMask;
    addr_offset = SW_PORTC_IO_BASE & ~PageMask;
    
    s_gpio_map = (void *)mmap(0, PageSize*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr_start);
    if( s_gpio_map == MAP_FAILED)
    {
        return false;
    }
    
    s_sunxi_pio_base = (unsigned int)s_gpio_map;
    s_sunxi_pio_base += addr_offset;
    
    close(fd);
    
    
    
    return true;

}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
/*static*/ bool GpioPlateformImplementation::init()
{
    
}

/* **** **** **** **** **** **** */

#elif defined __APPLE__
/*static*/ bool GpioPlateformImplementation::init()
{
    
    return true;
}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Plateform specific deInit routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef TARGET_RASPBERRY_PI
/*static*/ void GpioPlateformImplementation::deInit()
{
    bcm2835_close();

}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
/*static*/ void GpioPlateformImplementation::deInit()
{
    unsigned int PageSize;
    
    if ( s_gpio_map == NULL)
        return;
    
    PageSize = sysconf(_SC_PAGESIZE);
    munmap((void*) s_gpio_map, PageSize*2);
    

}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
/*static*/ void GpioPlateformImplementation::deInit()
{
    
}

/* **** **** **** **** **** **** */

#elif defined __APPLE__
/*static*/ void GpioPlateformImplementation::deInit()
{
    
}
#endif


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Plateform specific input setup routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
#ifdef TARGET_RASPBERRY_PI
void GpioPlateformImplementation::internalPinSetupInput( GPioInputType type )
{
    
    if ( _inputType == InputPullUp )
        
        bcm2835_gpio_set_pud( _pin, BCM2835_GPIO_PUD_UP);
    
    else if ( _inputType == InputPullDown )
        bcm2835_gpio_fsel( _pin, BCM2835_GPIO_PUD_DOWN );
    
    else if ( _inputType == InputDirect )
        bcm2835_gpio_fsel( _pin, BCM2835_GPIO_FSEL_INPT);

    
}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
void GpioPlateformImplementation::internalPinSetupInput( GPioInputType type )
{
    const int pinNum = SUNXI_GPD(  _pin );
    
    const unsigned int val = 0; // 0 = input , 1 = output
    
    unsigned int cfg;
    unsigned int bank   = GPIO_BANK( pinNum );
    unsigned int index  = GPIO_CFG_INDEX( pinNum);
    unsigned int offset = GPIO_CFG_OFFSET( pinNum);
    
    if( s_sunxi_pio_base == 0)
        return ;
    
    
    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *) s_sunxi_pio_base )->gpio_bank[bank];
    
    
    cfg = *(&pio->cfg[0] + index);
    cfg &= ~(0xf << offset);
    cfg |= val << offset;
    
    *(&pio->cfg[0] + index) = cfg;
    
    return ;

}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
void GpioPlateformImplementation::internalPinSetupInput( GPioInputType type )
{
    
}
/* **** **** **** **** **** **** */


#elif defined __APPLE__
void GpioPlateformImplementation::internalPinSetupInput( GPioInputType type )
{
    

}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Plateform specific output setup routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
#ifdef TARGET_RASPBERRY_PI
/*static*/ void GpioPlateformImplementation::internalPinSetupOutput( const int pin )
{
    bcm2835_gpio_fsel( pin , BCM2835_GPIO_FSEL_OUTP);
}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
/*static*/ void GpioPlateformImplementation::internalPinSetupOutput( const int pin )
{
    const int pinNum = SUNXI_GPD(  pin );
    
    const unsigned int val = 1; // 0 = input , 1 = output
    
    unsigned int cfg;
    unsigned int bank   = GPIO_BANK( pinNum );
    unsigned int index  = GPIO_CFG_INDEX( pinNum );
    unsigned int offset = GPIO_CFG_OFFSET( pinNum );
    
    if( s_sunxi_pio_base == 0)
        return ;
    
    
    struct sunxi_gpio *pio = &((struct sunxi_gpio_reg *) s_sunxi_pio_base )->gpio_bank[bank];
    
    
    cfg = *(&pio->cfg[0] + index);
    cfg &= ~(0xf << offset);
    cfg |= val << offset;
    
    *(&pio->cfg[0] + index) = cfg;
    

    
    return ;

}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
/*static*/ void GpioPlateformImplementation::internalPinSetupOutput( const int pin )
{
    
}

/* **** **** **** **** **** **** */

#elif defined __APPLE__
/*static*/ void GpioPlateformImplementation::internalPinSetupOutput( const int pin )
{
    
    
}

#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Plateform specific read routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef TARGET_RASPBERRY_PI
GpioState GpioPlateformImplementation::internalRead()
{
    return (GpioState) bcm2835_gpio_lev( _pin );
    
}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
GpioState GpioPlateformImplementation::internalRead()
{
    const int pinNum = SUNXI_GPD(  _pin );
    
    unsigned int dat;
    unsigned int bank = GPIO_BANK( pinNum );
    unsigned int num = GPIO_NUM( pinNum );
    
    if(s_sunxi_pio_base == 0)
        return undefined;

    
    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *) s_sunxi_pio_base )->gpio_bank[bank];
    
    dat = *(&pio->dat);
    dat >>= num;
    
    return ( GpioState ) (dat & 0x1);

}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
GpioState GpioPlateformImplementation::internalRead()
{
    return undefined;    
}

/* **** **** **** **** **** **** */

#elif defined __APPLE__
GpioState GpioPlateformImplementation::internalRead()
{
    return undefined;
}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 Plateform specific output routines
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef TARGET_RASPBERRY_PI
/*static*/ void GpioPlateformImplementation::internalSetOutput( const int pin , const GpioState state )
{
    bcm2835_gpio_write( pin, state);
}

/* **** **** **** **** **** **** */

#elif defined TARGET_CUBIE2
/*static*/ void GpioPlateformImplementation::internalSetOutput( const int pin , const GpioState state )
{
    const int pinNum = SUNXI_GPD(  pin );
    
    unsigned int bank = GPIO_BANK( pinNum );
    unsigned int num  = GPIO_NUM( pinNum );
    
    if( s_sunxi_pio_base == 0)
    {
        return ;
    }
    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *) s_sunxi_pio_base )->gpio_bank[bank];
    
    if( state == high )
        *(&pio->dat) |= 1 << num;
    else
        *(&pio->dat) &= ~(1 << num);
    

    
}

/* **** **** **** **** **** **** */

#elif defined TARGET_BEAGLEBONE_BLACK
/*static*/ void GpioPlateformImplementation::internalSetOutput( const int pin , const GpioState state )
{
    
}

/* **** **** **** **** **** **** */

#elif defined __APPLE__
/*static*/ void GpioPlateformImplementation::internalSetOutput( const int pin , const GpioState state )
{

}

#endif


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int SpiPlateformImplementation::s_count  = 0;

SpiBitOrder     SpiPlateformImplementation::s_order   = SpiMSB;
SpiDataMode     SpiPlateformImplementation::s_mode    = SpiMode_0;
SpiClockDivider SpiPlateformImplementation::s_divider = SPI_CLOCK_DIVIDER_65536;

SpiPlateformImplementation::SpiPlateformImplementation( SpiChipSelect cs):
_cs( cs )
{
    if( s_count++ == 0)
    {
        printf("\n SPI INIT");
#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_begin();
        
        bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
        bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
        bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_4096);
        bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
        bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
#endif
        /*
        setBitOrder( SpiMSB );
        setDataMode( SpiMode_0);
        setClockDivider( SPI_CLOCK_DIVIDER_4096);
        setChipSelect( _cs );
        setCsPolarity( low );
         */
    }


}

SpiPlateformImplementation::~SpiPlateformImplementation()
{
    if( --s_count <=0 )
    {
        s_count = 0;
        printf("\n SPI DEINIT");
#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_end();
#endif
        
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

uint8_t SpiPlateformImplementation::writeRead(const uint8_t &data )
{

#ifdef TARGET_RASPBERRY_PI
    return bcm2835_spi_transfer( data);

    
#elif defined __APPLE__
    return 0;
#endif
}

void SpiPlateformImplementation::writeReadMult( const uint8_t *send , uint8_t *rec , uint32_t length)
{
#ifdef TARGET_RASPBERRY_PI
    bcm2835_spi_transfernb( (char*) send ,(char*) rec , length);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SpiPlateformImplementation::setCsPolarity( GpioState state)
{

#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_setChipSelectPolarity( _cs, state );
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SpiPlateformImplementation::setChipSelect( SpiChipSelect cs)
{

#ifdef TARGET_RASPBERRY_PI

    bcm2835_spi_chipSelect( _cs );
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SpiPlateformImplementation::setBitOrder     ( SpiBitOrder order)
{
//    if (order != s_order)
    {
        s_order = order;
#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_setBitOrder( s_order);
#endif
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SpiPlateformImplementation::setDataMode     ( SpiDataMode mode)
{
//    if( mode != s_mode)
    {
        s_mode = mode;
#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_setDataMode(  s_mode);
#endif
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SpiPlateformImplementation::setClockDivider ( SpiClockDivider divider)
{
//    if( divider != s_divider )
    {
        s_divider = divider;
#ifdef TARGET_RASPBERRY_PI
        bcm2835_spi_setClockDivider( s_divider );
#endif
    }
}


