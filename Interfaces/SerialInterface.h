//
//  SerialInterface.h
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__SerialInterface__
#define __Broadway_core__SerialInterface__

#include <termios.h> // for speed_t def
#include <iostream>
#include "InterfaceEvent.h"

typedef enum
{
    Serial_0, // unspecified
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
    
    static const std::vector<std::string> getSerialDevicesList();
    
    SerialEvent( const std::string port );
    ~SerialEvent();

    bool setSpeed ( const SerialSpeed speed );
    
    SerialSpeed getSpeed() const
    {
        return _speed;
    }
    
    const std::string getPort() const
    {
        return _port;
    }
    
    bool changed();
    const Variant read() const;
    
    unsigned int getBytesToRead() const
    {
        return _bytesToRead;
    }
    
    void setBytesToRead( unsigned int num )
    {
        _bytesToRead = num;
    }
    
    bool send( const char* datas);
    
    /* Ugly Impl*/
    bool send( int val);
    
    void cleanup()
    {
        closePort();
    }
    
    bool openPort();
    bool closePort();
    
protected:

    static speed_t getInternalSpeed( const SerialSpeed speed );
    static SerialSpeed getSerialSpeed( speed_t internalSpeed );
    
private:
    bool          _isOpen;
    std::string   _port;
    int           _fd;
    
    unsigned int _bytesToRead;
    
    mutable int   _numBytes;
    
    bool _hasChanged;
    
    SerialSpeed _speed;
    
};

#endif /* defined(__Broadway_core__SerialInterface__) */
