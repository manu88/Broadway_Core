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

class SerialInterface : public InterfaceEvent
{
public:
    SerialInterface( const std::string port );
    ~SerialInterface();
    
    bool openPort();
    bool closePort();
    
    virtual bool changed();
    const std::string readDatas();
    
    static const std::vector<std::string> getSerialDevicesList();
    
    bool writeOnPort( const char* datas);
    
private:
    bool        _isOpen;
    std::string _port;
    int         _fd;
    
    bool _hasChanged;
    
};

#endif /* defined(__Broadway_core__SerialInterface__) */
