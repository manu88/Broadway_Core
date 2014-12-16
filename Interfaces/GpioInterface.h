//
//  GpioInterface.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__GpioInterface__
#define __Broadway_core__GpioInterface__

#include "InterfaceEvent.h"

#include "../Internal/Element.h"
/* **** **** **** **** **** */

typedef enum
{
    undefined = -1,
    low       = 0,
    high      = 1
    
}GpioState;



/* **** **** **** **** **** */

typedef enum
{
    InputDirect     = 0,
    InputPullDown   = 1,
    InputPullUp     = 2
} GPioInputType;


/* **** **** **** **** **** */

class GpioEvent :  public InterfaceEvent
{
    public :
    GpioEvent(int pinToUse , GPioInputType typeOfInput);
    
    ~GpioEvent();
    
    virtual bool changed();
    GpioState read();
    
    int pin;
    GpioState state;
    
    
    static void setDebounceTime( long delay )
    {
        s_debounceDelay = delay;
    }
    
private:
    
    GpioState m_lastState;
    long      m_lastDebounceTime;
    
    static long s_debounceDelay;
    
    static long millis();
};



#endif /* defined(__Broadway_core__GpioInterface__) */
