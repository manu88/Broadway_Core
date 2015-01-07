//
//  CanPlateformImpl.h
//  Broadway_core
//
//  Created by Manuel Deneu on 07/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__CanPlateformImpl__
#define __Broadway_core__CanPlateformImpl__

#include <iostream>

#ifdef HAVE_LINUX_CAN
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#include "../../Internal/Object.h"


/* Boiler plate for can_frame */

#ifndef HAVE_LINUX_CAN

typedef unsigned int  __u32;
typedef unsigned char __u8;
typedef __u32 canid_t;

struct can_frame
 {
 canid_t can_id;              // 32 bit CAN_ID + EFF/RTR/ERR flags
 __u8    can_dlc;           // data length code: 0 .. 8
 __u8    data[8] __attribute__((aligned(8)));
 };
#endif

/* **** **** **** **** **** */



class CanPlateformImpl : public Object
{
    friend class CanEvent;
    
public:
    
    CanPlateformImpl( const std::string &interface );
    ~CanPlateformImpl();
    
    bool createSocket();
    bool bind();
    
    bool closeSocket();
    
    bool sendFrame( unsigned int frameID , int length, const unsigned char* frameData);
    
private:
    std::string _interfName;
    
    bool _isConnected; // isbound
    int  _socket;
    
};

#endif /* defined(__Broadway_core__CanPlateformImpl__) */
