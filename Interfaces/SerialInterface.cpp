//
//  SerialInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/ioctl.h>

#include "SerialInterface.h"

#include "../Internal/FileSystem.h"
#include "../Log/Log.h"

SerialEvent::SerialEvent( const std::string port ) :
InterfaceEvent( Event_Serial ),

_isOpen      ( false ),
_port        ( port  ),
_fd          ( -1    ),
_bytesToRead ( 1     ),
_numBytes    ( 0     ),
_hasChanged  ( false ),

_speed      ( Serial_9600 )
{
    
}

SerialEvent::~SerialEvent()
{
    closePort();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::openPort()
{
    if ( _isOpen )
        return true;

    _fd = open( _port.c_str(), O_RDWR | O_NONBLOCK);
    
    if ( _fd == -1)
        Log::log(" ERROR : unable to open port '%s' ." , _port.c_str() );


    _isOpen = _fd != -1;
    return _isOpen;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::closePort()
{
    if (_isOpen)
    {
        int ret = close( _fd );
        
        if ( ret != 0 )
            return false;
        
        _isOpen = false;
            
    }
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::setSpeed ( const SerialSpeed speed )
{
    if ( speed != _speed)
    {
        _speed = speed;
        
        struct termios options;
        
        
    
        tcgetattr( _fd, &options);
        
        cfsetispeed( &options, getInternalSpeed( _speed ) );
        cfsetospeed( &options, getInternalSpeed( _speed ) );
        
        options.c_cflag = CS8|CREAD|CLOCAL;// |= (CLOCAL | CREAD);

        options.c_iflag=0;
        options.c_oflag=0;
        
        options.c_lflag=0;
        options.c_cc[VMIN]=1;
        options.c_cc[VTIME]=5;

        const int ret =  tcsetattr( _fd, TCSANOW, &options);
        
        
        if ( getSerialSpeed( cfgetispeed( &options ) ) !=  _speed )
        {
            Log::log("Error ispeed");
        }
        
        if ( getSerialSpeed( cfgetospeed( &options ) ) != _speed )
        {
            Log::log("Error ospeed");            
        }

        
        if ( ret == -1 )
        {

            
            return false;
        }

    }
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ speed_t SerialEvent::getInternalSpeed( const SerialSpeed speed )
{
    switch (speed)
    {
        case Serial_1200:
            return B1200;
        case Serial_2400:
            return B2400;
        case Serial_4800:
            return B4800;
        case Serial_9600:
            return B9600;
        case Serial_19200:
            return B19200;
        case Serial_38400:
            return B38400;
        case Serial_57600:
            return B57600;
        case Serial_115200:
            return B115200;


            
        default:
            break;
    }
    
    return B0; // default
}

/*static*/ SerialSpeed SerialEvent::getSerialSpeed( speed_t internalSpeed )
{
    switch ( internalSpeed )
    {
        case B1200:
            return Serial_1200;
        case B2400:
            return Serial_2400;
        case B4800:
            return Serial_4800;
        case B9600:
            return Serial_9600;
        case B19200:
            return Serial_19200;
        case B38400:
            return Serial_38400;
        case B57600:
            return Serial_57600;
        case B115200:
            return Serial_115200;
            
            
            
        default:
            break;
    }
    
    return Serial_0;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::changed()
{
    ioctl( _fd, FIONREAD, &_numBytes);

    return _hasChanged =  _numBytes  >= _bytesToRead;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const Variant SerialEvent::read() const
{
    if ( _numBytes > 0)
    {

        
        char in[ _numBytes+1 ];
        ::read( _fd, in, _numBytes );
        in[ _numBytes ] = 0;
        
        _numBytes = 0;
        return Variant( in );
    }
    
    return Variant::null();
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::send( const char* datas)
{
    if ( !_isOpen )
        return false;
    
    int numWritten = ( int ) ::write( _fd, datas ,strlen( datas ) );
    
    
    return numWritten > 0;
    
}

bool SerialEvent::send( int val)
{
    char buf[4];
    sprintf(buf, "%i" , val);

    return send( buf );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const std::vector<std::string> SerialEvent::getSerialDevicesList()
{
#ifdef __APPLE__
    
    auto list =  FileSystem::getFilesListFromFolder("/dev/",true ,true,"tty."  );
    
#elif defined __unix__

    auto list =  FileSystem::getFilesListFromFolder("/dev/",true, true ,"tty" );
    
#endif
    
    return list;
}


