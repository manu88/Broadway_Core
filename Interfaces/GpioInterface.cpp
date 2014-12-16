//
//  GpioInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <sys/time.h>

#include "GpioInterface.h"

long GpioEvent::s_debounceDelay = 10;  // default value


GpioEvent::GpioEvent(int pinToUse , GPioInputType typeOfInput) :
InterfaceEvent( Event_Gpio ),
_impl( pinToUse , typeOfInput ),

pin                ( pinToUse  ),
state              ( undefined ),
m_lastState        ( undefined ),
m_lastDebounceTime (0          )
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
    
    if (newState != m_lastState)
    {
        m_lastDebounceTime = millis();
    }
    
    if ( (millis() - m_lastDebounceTime) > s_debounceDelay)
    {
        if (newState != state)
        {
            state = newState;
            m_lastState = newState;
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
    
    m_lastState = newState;
    
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioState GpioEvent::read()
{
    return state;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static */long GpioEvent::millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int) (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}

