//
//  SerialInterface.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__SerialInterface__
#define __Broadway_core__SerialInterface__

#include <iostream>
#include "InterfaceEvent.h"

typedef enum
{
    Serial_1200,
    Serial_2400,
    Serial_4800,
    Serial_9600,
    Serial_19200,
    Serial_38400,
    Serial_57600,
    Serial_115200
    
} SerialSpeed;

class SerialEvent : public InterfaceEvent
{
public:
    SerialEvent( const std::string port );
    ~SerialEvent();
    
    bool openPort();
    bool closePort();
    
    void setSpeed ( const SerialSpeed speed );
    
    SerialSpeed getSpeed() const
    {
        return _speed;
    }
    
    const std::string getPort() const
    {
        return _port;
    }
    
    bool changed();
    const std::string readDatas();
    
    static const std::vector<std::string> getSerialDevicesList();
    
    bool send( const char* datas);
    
    /* Ugly Impl*/
    bool send( int val);
    
    void cleanup()
    {
        closePort();
    }
    
private:
    bool        _isOpen;
    std::string _port;
    int         _fd;
    
    bool _hasChanged;
    
    SerialSpeed _speed;
    
};

#endif /* defined(__Broadway_core__SerialInterface__) */
