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

#endif /* defined(__Broadway_core__GpioPlateformImpl__) */
