//
//  NetworkController.cpp
//  
//
//  Created by Manuel Deneu on 14/08/14.
//
//


#include <assert.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <errno.h>

#include <string>
#include <algorithm>

/**/

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>


#include <stdlib.h>
#include <stdint.h>
#include <iostream>
/**/

#include "../Internal/ThreadLock.h"

#include "NetworkController.h"

#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/osc/OscTypes.h>

#include <oscpack/ip/UdpSocket.h>

#include "../Log/Log.h"
#include "../Config.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

NetworkController::NetworkController() :
    AbstractController( "Network controller" ),
    Thread("Network")
{


}

NetworkController::~NetworkController()
{
    
    removeAllSockets();
    stop();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::start()
{

    unlockThread();
    return startThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::stop()
{
    _lockThread = false;
    sendAsyncStop();
    
    _multiplexer.AsynchronousBreak();

    return stopThread();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::run()
{
    while ( !threadShouldStop() )
    {
        setReady();
        
        // doesn't return until m_multiplexer.AsynchronousBreak(); is called
        // see NetworkControllerLock 
        _multiplexer.Run();

        
        if ( threadShouldStop() )
        {
            break;
        }
        

        while ( _lockThread )
        {
        
        }

    }

    setUnReady();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkController::addPort(int port)
{
    NetworkControllerLock l(this);
    
    if ( getSocketForPort( port ) != nullptr )
    {
        Log::log("\n socket for port %i already exists" , port);
        return true;
    }

    try
    {
        SocketUdpIn *socket = new SocketUdpIn( port );
        _socketList.insert( socket );
        

        
        socket->Bind( IpEndpointName( IpEndpointName::ANY_ADDRESS, port ) );
        _multiplexer.AttachSocketListener( socket, this );
    }
    catch (...)
    {

        Log::log(" unable to bind socket!");
        return false ;
    }

    

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::removePort ( int port )
{
    SocketUdpIn *socket = getSocketForPort( port );
    
    if ( socket )
    {
        NetworkControllerLock l(this);
        

        internalDeleteSocket( socket );
        
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::internalDeleteSocket( SocketUdpIn* socket )
{
    _multiplexer.DetachSocketListener( socket , this );
    

    
    delete socket;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::removeAllSockets()
{
    NetworkControllerLock l(this);
    for (auto i : _socketList )
    {
        internalDeleteSocket( i );
    }
    
    _socketList.clear();
    

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SocketUdpIn* NetworkController::getSocketForPort( int port )
{
    for (auto i : _socketList )
    {
        if ( i->getPort() == port )
            return  i;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


bool NetworkController::sendOSC( const std::string &ip , const int port , const std::string &address , const ArgumentsArray &vars )
{
    // try catch runtime error
    try
    {    

        UdpTransmitSocket transmitSocket( IpEndpointName( ip.c_str() , port ) );
        
        char buffer[ Output_Buffer_Size ];
        osc::OutboundPacketStream p( buffer, Output_Buffer_Size );
        
        p << osc::BeginMessage( address.c_str() );
    

        for (int i = 0; i< vars.getSize() ; i++)
        {
            if ( vars.isType< int >(i))
                p << vars.getValueAtIndex< int >( i );
            
            else if ( vars.isType< std::string >(i))
                p << vars.getValueAtIndex< std::string >( i ).c_str() ;
            
            else if ( vars.isType< const char* >(i))
                p << vars.getValueAtIndex<const char* >( i ) ;
            
            else if ( vars.isType< float >(i))
                p << vars.getValueAtIndex< float >( i );
            
            else if ( vars.isType< double >(i))
                p << (float) vars.getValueAtIndex< double >( i );
            
            else
                Log::log("error while parsing ArgumentsArray -> OSCMessage, type not handled");
            
        }



        p<< osc::EndMessage;
        
        transmitSocket.Send( p.Data(), p.Size() );
        
        return true;
        
    }
    catch (...)
    {
        Log::log("\n caught execption on Osc::send");
    }
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void NetworkController::ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint )
{
    char ip[IpEndpointName::ADDRESS_STRING_LENGTH ];
    
    remoteEndpoint.AddressAsString( ip );
    
//    ArgumentsArray *array =  ;
    
    _delegate->oscReceived( ip, remoteEndpoint.port , m.AddressPattern(), *NetworkTools::getArrayFromOSCMessage(m) );
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    NetworkTools namespace
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkTools::changeIpAddressTo(const std::string ip , const std::string netmask , const std::string interface)
{
    struct ifreq ifr;
    const char * name =  interface.c_str();
    
    int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    
    strncpy(ifr.ifr_name, name, IFNAMSIZ);
    
    ifr.ifr_addr.sa_family = AF_INET;
    
    if ( inet_pton(AF_INET, ip.c_str() , ifr.ifr_addr.sa_data + 2) !=1 )
    {
        printf("\n %s :  unable to set ip to %s " , interface.c_str() ,ip.c_str() );
        return false;
    }
    
    if ( ioctl(fd, SIOCSIFADDR, &ifr) == -1)
    {
        printf("\n error while setting ip address : errno =  %i" , errno );
        return false;        
    }

    
    if ( inet_pton(AF_INET, netmask.c_str(), ifr.ifr_addr.sa_data + 2) != 1)
    {
        printf("\n %s : unable to set netmask to %s " , interface.c_str() ,netmask.c_str() );
        return false;
    }
    
    if ( ioctl(fd, SIOCSIFNETMASK, &ifr) == -1)
    {
        printf("\n error while setting netmask: errno =  %i" , errno );
        return false;        
    }
    
    
    if ( ioctl(fd, SIOCGIFFLAGS, &ifr) == -1)
    {
        printf("\n error : %i" , errno );
        return false;        
    }
    
    
    strncpy(ifr.ifr_name, name, IFNAMSIZ);
    
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
    
    if ( ioctl(fd, SIOCSIFFLAGS, &ifr) == -1)
    {
        printf("\n error : %i" , errno );
        return false;        
    }

    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool NetworkTools::isConnected()
{

    

    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::unique_ptr< ArgumentsArray > NetworkTools::getArrayFromOSCMessage(const osc::ReceivedMessage& m)
{
    
    ArgumentsArray* array = new ArgumentsArray();
    try
    {
        
        
        for (auto it = m.ArgumentsBegin() ; it != m.ArgumentsEnd() ; it++ )
        {
            if ( (it)->IsFloat() )
            {
                array->addValue< float >( (it)->AsFloat() );
            }
            
            else if ( (it)->IsInt32() )
            {
                array->addValue< int >( (it)->AsInt32() );
            }
            
            else if ( (it)->IsInt64() )
            {
                array->addValue< int >( (int) (it)->AsInt64() );
            }
            
            else if ( (it)->IsString() )
            {
                char chars[] = "\"";
                std::string str = (it)->AsString();
                for (unsigned int i = 0; i < strlen(chars); ++i)
                {
                    str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
                }
                array->addValue< std::string >( str );
            }
            else
            {
                /* No parsing method for this type, to be added ...*/
                DEBUG_ASSERT( false );
            }
            
            //            it++;
            
        }
        
    }
    
    catch( osc::Exception& e )
    {
        Log::log("\n error while parsing message: %s " , e.what() );
    }
    
    return std::unique_ptr< ArgumentsArray > ( array );
    
}



