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

#include "Impl/GpioPlateformImpl.h"




class GpioEvent :  public InterfaceEvent
{
public :
    GpioEvent(int pinToUse , GPioInputType typeOfInput);
    
    ~GpioEvent();
    
    static bool init()
    {
        return GpioPlateformImplementation::init();
    }
    
    static void deInit()
    {
        GpioPlateformImplementation::deInit();
    }
    
    
    
    virtual bool changed();
    GpioState read();
    
    int pin;
    GpioState state;
    
    static void setGpio( const int pin , const GpioState state);
    
    static void setDebounceTime( long delay )
    {
        s_debounceDelay = delay;
    }
    
    virtual void cleanup()
    {
        
    }
    
private:
    
    // actual plateform specific implementation
    GpioPlateformImplementation _impl;
    
    GpioState m_lastState;
    
    long      m_lastDebounceTime;
    static long s_debounceDelay;
    
    // should use Schedulers's
    static long millis();
};



#endif /* defined(__Broadway_core__GpioInterface__) */
