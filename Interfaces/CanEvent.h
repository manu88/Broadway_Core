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
#include "Impl/CanPlateformImpl.h"

/*
    The actual implementation is in Impl/CanPlateformImpl.cpp and relies
    on linux-can for unix system
 */

class CanEvent : public InterfaceEvent
{
public:
    
    CanEvent( const std::string &interface );
    ~CanEvent();
    
    const std::string &getInterfaceName() const noexcept
    {
        return _impl._interfName;
    }
    
    bool isConnected() const noexcept
    {
        return _impl._isConnected;
    }
    
    bool connect();
    bool close();
    
    bool sendFrame( unsigned int frameID , int length, const unsigned char* frameData)
    {
        return _impl.sendFrame( frameID, length , frameData );
    }
    
protected:
    bool changed();
    void cleanup();
    
    
private:
    // actual implementation
    CanPlateformImpl _impl;

    
};

#endif /* defined(__Broadway_core__CanEvent__) */
