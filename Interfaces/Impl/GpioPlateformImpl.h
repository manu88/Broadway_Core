//
//  GpioPlateformImpl.h
//  Broadway_core
//
//  Created by Manuel Deneu on 16/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__GpioPlateformImpl__
#define __Broadway_core__GpioPlateformImpl__

#include "../../Internal/Object.h"

/* **** **** **** **** **** */

typedef unsigned int GPioPin;

/* **** **** **** **** **** */

typedef enum
{
    Gpio_Input,
    Gpio_Ouput
} GpioType;

/* **** **** **** **** **** */

typedef enum
{
    undefined = -1,
    low       = 0,
    high      = 1
    
} GpioState;



/* **** **** **** **** **** */

typedef enum
{
    InputDirect     = 0,
    InputPullDown   = 1,
    InputPullUp     = 2
    
} GPioInputType;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

typedef enum
{
    SpiCS_0    = 0,
    SpiCS_1    = 1,
    SpiCS_2    = 2,
    SpiCS_NONE = 3 // do it yourself!
    
} SpiChipSelect;

typedef enum
{
    SpiMSB = 0,
    SpiLSB = 1
    
} SpiBitOrder;

typedef enum
{
    SpiMode_0 = 0,
    SpiMode_1 = 1,
    SpiMode_2 = 2,
    SpiMode_3 = 3
    
} SpiDataMode;

typedef enum
{
    SPI_CLOCK_DIVIDER_65536 = 0,
    SPI_CLOCK_DIVIDER_32768 = 32768,
    SPI_CLOCK_DIVIDER_16384 = 16384,
    SPI_CLOCK_DIVIDER_8192  = 8192,
    SPI_CLOCK_DIVIDER_4096  = 4096,
    SPI_CLOCK_DIVIDER_2048  = 2048,
    SPI_CLOCK_DIVIDER_1024  = 1024,
    SPI_CLOCK_DIVIDER_512   = 512,
    SPI_CLOCK_DIVIDER_256   = 256,
    SPI_CLOCK_DIVIDER_128   = 128,
    SPI_CLOCK_DIVIDER_64    = 64,
    SPI_CLOCK_DIVIDER_32    = 32,
    SPI_CLOCK_DIVIDER_16    = 16,
    SPI_CLOCK_DIVIDER_8     = 8,
    SPI_CLOCK_DIVIDER_4     = 4,
    SPI_CLOCK_DIVIDER_2     = 2,
    SPI_CLOCK_DIVIDER_1     = 1
    
} SpiClockDivider;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class GpioPlateformImplementation : public Object
{
public:
    GpioPlateformImplementation( const int pin, const GPioInputType type );
    ~GpioPlateformImplementation();
    
    // plateform specifics
    static bool init();
    static void deInit();
    
    GpioState read();
    
    static void setGpio( const int pin , const GpioState state);

    
private:
    int           _pin;
    GPioInputType _inputType;

    // plateform specifics
    GpioState   internalRead();
    void internalPinSetupInput( GPioInputType type );
    static void internalPinSetupOutput( const int pin );
    static void internalSetOutput( const int pin , const GpioState state );
    
    #ifdef TARGET_CUBIE2
    static volatile void *s_gpio_map;
    static unsigned int s_sunxi_pio_base;
    #endif
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class SpiPlateformImplementation : public Object
{
public:
    SpiPlateformImplementation( SpiChipSelect cs);
    ~SpiPlateformImplementation();
    
    uint8_t writeRead( const uint8_t &data );
    void writeReadMult( const uint8_t *send , uint8_t *rec , uint32_t length);
    
    void setCsPolarity( GpioState state);
    void setChipSelect( SpiChipSelect cs);
    
    static void setBitOrder     ( SpiBitOrder order);
    static void setDataMode     ( SpiDataMode mode);
    static void setClockDivider ( SpiClockDivider divider);
    
private:
    SpiChipSelect _cs;
    
    static int s_count;
    
    // default SPI params
    
    static SpiBitOrder     s_order;
    static SpiDataMode     s_mode;
    static SpiClockDivider s_divider;
    
};

#endif /* defined(__Broadway_core__GpioPlateformImpl__) */
