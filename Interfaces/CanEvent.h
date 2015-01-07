//
//  CanEvent.h
//  Broadway_core
//
//  Created by Manuel Deneu on 06/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__CanEvent__
#define __Broadway_core__CanEvent__

#include <iostream>

#include "InterfaceEvent.h"

class CanEvent : public InterfaceEvent
{
public:
    
    CanEvent( const std::string &interface );
    ~CanEvent();
    
protected:
    
private:
    
    std::string _interfName;
    
};

#endif /* defined(__Broadway_core__CanEvent__) */
