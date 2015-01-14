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

//#include "../JSMachine/JSMachine.h"
#include "../Internal/Element.h"

/* **** **** **** **** **** */

//! \enum Transition  Define the change of state on witch the callback will occur.
typedef enum
{
    Undefined       = -1, /**< Undefined. This state only appear on Event's contruction. 
                               Use Event::setTransition() to set the transition to the desired state   
                           */
    Changed         = 0,  // default
    Rising          = 1,  // front montant
    Falling         = 2,  // front descendant
    EndOfVideo      = 3,  // Fin de stream
    MessageReceived = 4   // Message reçu
    
}Transition; //!< Transition type



class Selector;
class JSMachine;


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
    Event( );
    virtual ~Event();
    

    void setTransition( const Transition transition)
    {
        _transition = transition;
    }

    Transition getTypeOfTransition() const
    {
        return _transition;
    }


    
protected:
    Transition  _transition;
private:

    
    
    
};

#endif /* defined(____Event__) */
