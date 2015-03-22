//
//  Event.h
//  
//
//  Created by Manuel Deneu on 12/08/14.
//
//

#ifndef ____Event__
#define ____Event__

#include <iostream>

#include "../Internal/Element.h"

/* **** **** **** **** **** **** */

typedef enum
{
    Event_Timer   = 0,   // implemented by TimedEvent
    
    Event_Input   = 100, // implemented by GpioEvent
    
    Event_Network = 200,
    Event_OSC     = 201,
    
    Event_GET     = 230,
    
    Event_Video   = 300,   // implemented by GXVideo
    Event_Action  = 400    // implemented in GXUI system
    
} EventType;


/* **** **** **** **** **** **** */


//! Event
/********************************************//**
*
*   Base class for any type of Event with a callback.
*   You have to register a Selector witch will be preformed when the event is fired.
*                                                   
*   \sa GpioEvent OscEvent
*
*   \relates Transition
***********************************************/
class Event : public virtual Element
{
public:
    Event( EventType type );
    
    virtual ~Event();
    
    /**/
    
    const EventType &getType() const
    {
        return _type;
    }
    
    bool isTimer() const
    {
        return _type == Event_Timer;
    }
    
    bool isVideo() const
    {
        return _type == Event_Video;
    }
    
    bool isInput() const
    {
        return _type == Event_Input;
    }
    
    bool isAction() const
    {
        return _type == Event_Action;
    }
    
    /* Safe cast */
    
    template<typename T>
    static T event_cast( Event* event)
    {
        return reinterpret_cast<T>(event);
    }

    /**/
    
protected:
    
    EventType _type;

private:

    
    
    
};

#endif /* defined(____Event__) */
