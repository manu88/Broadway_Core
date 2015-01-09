//
//  CanEvent.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 06/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "CanEvent.h"
#include "../Log/Log.h"

CanEvent::CanEvent( const std::string &interface ):
InterfaceEvent( Event_CAN ),

_impl( interface )
{
    
}

CanEvent::~CanEvent()
{
    
}

bool CanEvent::connect()
{
    if ( _impl.createSocket() )
    {
        if (!_impl.bind() )
        {
            Log::log("Unable to bind soket for CAN interface '%s'" , _impl._interfName.c_str() );
        }
    }
    else
        Log::log("Unable to create soket for CAN interface '%s'" , _impl._interfName.c_str() );
    
    return _impl._isConnected;
}

bool CanEvent::close()
{
    return _impl.closeSocket();
}

bool CanEvent::changed()
{
    return false;
}

void CanEvent::cleanup()
{
    close();
}
