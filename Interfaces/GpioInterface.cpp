//
//  GpioInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <sys/time.h>

#ifdef TARGET_RASPBERRY_PI
#include <bcm2835.h>
#endif
#include "GpioInterface.h"

long GpioEvent::s_debounceDelay = 10;  // default value


GpioEvent::GpioEvent(int pinToUse , GPioInputType typeOfInput) :
InterfaceEvent( Event_Gpio ),

pin                ( pinToUse  ),
state              ( undefined ),
m_lastState        ( undefined ),
m_lastDebounceTime (0          )
{
    className = "GpioEvent";
    
#ifdef TARGET_RASPBERRY_PI
    if ( typeOfInput == InputPullUp )
        
        bcm2835_gpio_set_pud( pin, BCM2835_GPIO_PUD_UP);
    
    else if ( typeOfInput == InputPullDown )
        bcm2835_gpio_fsel( pin, BCM2835_GPIO_PUD_DOWN );
    
    else if ( typeOfInput == InputDirect )
        bcm2835_gpio_fsel( pin, BCM2835_GPIO_FSEL_INPT);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent::~GpioEvent()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GpioEvent::changed()
{
    
#ifdef TARGET_RASPBERRY_PI
    GpioState newState = (GpioState) bcm2835_gpio_lev( pin );
#else
    GpioState newState = undefined;
#endif
    
    
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

/*static*/ long GpioEvent::millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int) (tv.tv_sec) * 1000 + (tv.tv_usec)/1000;
}
