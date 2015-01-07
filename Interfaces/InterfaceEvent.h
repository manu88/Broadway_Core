//
//  InterfaceEvent.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__InterfaceEvent__
#define __Broadway_core__InterfaceEvent__

#include <iostream>

#include "../Internal/Element.h"



typedef enum
{
    Event_Gpio,   // GPIO HIGH/LOW state
    Event_I2C,    // I2C device
    Event_SPI,    // SPI device
    Event_Serial,  // Serial Interface
    Event_CAN
    
} InterfaceEventType;


class InterfaceEvent : public Element
{
public:
    virtual ~InterfaceEvent();
    
    // subclasses need to overide this
    virtual bool changed() = 0;
    // and define a read() method they can call later
    
    // this method will be called before destruction by InterfaceController instance
    virtual void cleanup() = 0;
    
    /* **** */
    
    InterfaceEventType getType() const
    {
        return _type;
    }
    
    /* **** */
    
    bool isGpioEvent() const
    {
        return _type == Event_Gpio;
    }
    
    bool isSerialEvent() const
    {
        return _type == Event_Serial;
    }
    
    bool isCanEvent() const
    {
        return _type == Event_CAN;
    }
    
protected:
    
    InterfaceEvent( InterfaceEventType type);
    
private:
    
    InterfaceEventType _type;
    
};


#endif /* defined(__Broadway_core__InterfaceEvent__) */
