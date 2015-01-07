//
//  CanPlateformImpl.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 07/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>



#include "CanPlateformImpl.h"


CanPlateformImpl::CanPlateformImpl( const std::string &interface ):

_interfName  ( interface ),
_isConnected ( false ),
_socket      ( -1 )
{
    
}

CanPlateformImpl::~CanPlateformImpl()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool CanPlateformImpl::createSocket()
{
    
#ifdef HAVE_LINUX_CAN
    if( ( _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
		perror("Error while opening socket");
		return false;
	}
    return true;
    
/* **** **** **** */
// No impl without linux-can yet ...
#else
    return false;
#endif

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool CanPlateformImpl::closeSocket()
{
#ifdef HAVE_LINUX_CAN
    
    return _isConnected =  close( _socket );
    
/* **** **** **** */
    // No impl without linux-can yet ...
#else
    return _isConnected = false;
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool CanPlateformImpl::bind()
{

	struct ifreq ifr;
    
    strcpy(ifr.ifr_name, _interfName.c_str() );

    
#ifdef HAVE_LINUX_CAN
    
    struct sockaddr_can addr;
    
    ioctl( _socket, SIOCGIFINDEX, &ifr);
    
    addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
    
    if( bind( _socket , (struct sockaddr *) &addr, sizeof(addr) ) < 0 )
    {
		perror("Error in socket bind");
		return _isConnected = false;
	}
    
    return _isConnected = true;
    
/* **** **** **** */
// No impl without linux-can yet ...
#else
    return _isConnected = false;

#endif
}

bool CanPlateformImpl::sendFrame( unsigned int frameID , int length, const unsigned char* frameData)
{
	int nbytes = 0;

    
#ifdef HAVE_LINUX_CAN
    struct can_frame frame;
    
    nbytes = write( _socket, &frame, sizeof(struct can_frame));
    
    return true;
    
    /* **** **** **** */
    // No impl without linux-can yet ...
#else
    return nbytes >= 0;
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */