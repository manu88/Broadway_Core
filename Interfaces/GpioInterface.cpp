//
//  GpioInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <sys/time.h>

#include "GpioInterface.h"
#include "../Scheduler/Scheduler.h"




GpioEvent::GpioEvent(int pinToUse , GPioInputType typeOfInput) :
InterfaceEvent( Event_Gpio ),
_impl( pinToUse , typeOfInput ),

_pin                ( pinToUse  ),
_state              ( undefined ),
_lastState        ( undefined ),

_debounceDelay     ( 10        ), // default val
_lastDebounceTime (0          )
{
    className = "GpioEvent";
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent::~GpioEvent()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GpioEvent::setGpio( const int pin , const GpioState state)
{
    GpioPlateformImplementation::setGpio( pin, state);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GpioEvent::changed()
{
    GpioState newState = _impl.read();
    
    if (newState != _lastState)
    {
        _lastDebounceTime = static_cast<long>( Scheduler::getTimeInMs() );
    }
    
    if ( ( Scheduler::getTimeInMs() - _lastDebounceTime) > _debounceDelay)
    {
        if (newState != _state)
        {
            _state = newState;
            _lastState = newState;
            /*
             if (    (               m_transition == Changed )
             || ( state == 0 && m_transition == Falling )
             || ( state == 1 && m_transition == Rising  )
             )
             //callSelector();
             */
            return true;
        }
    }
    
    _lastState = newState;
    
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioState GpioEvent::read()
{
    return _state;
}


