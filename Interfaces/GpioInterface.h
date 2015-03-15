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


typedef unsigned int GPioPin;


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
    
    
    
    bool changed();
    GpioState read();
    
    unsigned int pin;
    GpioState state;
    
    static void setGpio( const int pin , const GpioState state);
    
    void setDebounceTime( long delay )
    {
        _debounceDelay = delay;
    }
    
    long getDebounceTime() const
    {
        return _debounceDelay;
    }
    
    void cleanup()
    {
    }
    
private:
    
    // actual plateform specific implementation
    GpioPlateformImplementation _impl;
    
    GpioState _lastState;
    
    long      _lastDebounceTime;
    long      _debounceDelay;
    
};



#endif /* defined(__Broadway_core__GpioInterface__) */
