//
//  SerialInterface.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 15/12/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <termios.h>
#include <unistd.h>

#include "SerialInterface.h"

#include "../Internal/FileSystem.h"
#include "../Log/Log.h"

SerialEvent::SerialEvent( const std::string port ) :
InterfaceEvent( Event_Serial ),

_isOpen     ( false ),
_port       ( port  ),
_fd         ( -1    ),
_hasChanged ( false ),

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

    _fd = open( _port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    
    if ( _fd == -1)
    {
        Log::log(" ERROR : unable to open port '%s' ." , _port.c_str() );        
        return false;
    }
    
    
    // ok
    
    Log::log("Port '%s' is open" , _port.c_str() );
    

    _isOpen = true;
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::closePort()
{
    if (_isOpen)
    {
        int ret = close( _fd );
        
        if ( ret != 0 )
        {
            Log::log(" ERROR : unable to close port '%s' ." , _port.c_str() );
            return false;
        }
        
        _isOpen = false;
            Log::log("Port '%s' is close ." , _port.c_str() );            
            
    }
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void SerialEvent::setSpeed ( const SerialSpeed speed )
{
    if ( speed != _speed)
    {
        _speed = speed;
        
        struct termios options;
    
        tcgetattr( _fd, &options);
        
        /*
         * Set the baud rates to 19200...
         */
        
        cfsetispeed(&options, _speed);
        cfsetospeed(&options, _speed);
        
        options.c_cflag |= (CLOCAL | CREAD);
        
        tcsetattr( _fd, TCSANOW, &options);

    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::changed()
{
    return _hasChanged =  fcntl( _fd, F_SETFL, FNDELAY) >0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::string SerialEvent::readDatas()
{
    std::string ret;

    char buf = '\0';
    long  n = -1;

  
    char buffer[2];
    n = read( _fd, buffer, sizeof(buffer));
    
    if ( n<=0 )
        return "";
  
//    printf("size %i" , n);

    return  std::string(buffer);// ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SerialEvent::writeOnPort( const char* datas)
{
    if ( !_isOpen )
        return false;
    
    int numWritten = ( int ) write( _fd, datas , strlen( datas ) );
    return numWritten > 0;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ const std::vector<std::string> SerialEvent::getSerialDevicesList()
{
#ifdef __APPLE__
    
    auto list =  FileSystem::getFilesListFromFolder("/dev/",true ,"tty." );
    
#elif defined __unix__

    auto list =  FileSystem::getFilesListFromFolder("/dev/",true ,"tty" );
    
#endif
    
    return list;
}


