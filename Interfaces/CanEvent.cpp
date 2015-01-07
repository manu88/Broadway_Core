//
//  CanEvent.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 06/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "CanEvent.h"


CanEvent::CanEvent( const std::string &interface ):
InterfaceEvent( Event_CAN ),

_interfName ( interface )
{
    
}

CanEvent::~CanEvent()
{
    
}