//
//  IPCSocket.h
//  Broadway_core
//
//  Created by Manuel Deneu on 31/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__IPCSocket__
#define __Broadway_core__IPCSocket__

#include <stdio.h>

#include "../Internal/Object.h"

class IPCSocket : public Object
{
public:
    IPCSocket( const std::string &name);
    ~IPCSocket();
    
private:
    
    std::string _socketName;
    
    FILE *_fp;
    int   _socket;
    
    
};


#endif /* defined(__Broadway_core__IPCSocket__) */
