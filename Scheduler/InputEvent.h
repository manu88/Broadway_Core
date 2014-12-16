//
//  InputEvent.h
//  Broadway_test
//
//  Created by Manuel Deneu on 03/11/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__InputEvent__
#define __Broadway_test__InputEvent__

#include <iostream>
#include "Event.h"

typedef enum
{
    Input_Clic        = 1,
    Input_Release     = 2,
    Input_KeyPressed  = 3,
    Input_KeyReleased = 4,
    Input_Move        = 10

    
} InputType;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InputEvent : public Event
{
public:
    InputEvent();
    ~InputEvent();
    
protected:
    
private:
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class EventListener
{
public :
    EventListener();
    virtual ~EventListener();
    
    virtual bool pollInput() = 0;
    
protected:
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#endif /* defined(__Broadway_test__InputEvent__) */

