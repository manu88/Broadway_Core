//
//  IPCSocket.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 31/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>



#include "IPCSocket.h"

IPCSocket::IPCSocket( const std::string &name) :
_socketName( name ),

_fp ( nullptr ),
_socket( -1 )
{
    
}

IPCSocket::~IPCSocket()
{
    
}