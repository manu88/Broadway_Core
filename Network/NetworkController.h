/*!
 *  \brief     Network Controller
 *  \details   This class is used to controller network Events such as Osc and Udp packets
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       Broadcast messages don't work on Unix( tested on RPI only)
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef ____NetworkController__
#define ____NetworkController__

#include <iostream>
#include <unordered_set>

#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/ip/PacketListener.h>

#include "../Internal/ThreadLock.h"
#include "../Internal/Object.h"

#include "../Scheduler/Event.h"
#include "../Data/ArgumentsArray.h"

#include "../Internal/AbstractController.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class OSCEvent : public Event
{
public :
    OSCEvent(const std::string &_ip , const int _port , const std::string &_address , const ArgumentsArray *_vars) :
    Event( Event_OSC   ),
    ip      ( _ip      ),
    port    ( _port    ),
    address ( _address )
    {}
    
    ~OSCEvent();
    
    const std::string ip;
    const int         port;
    const std::string address;
    
    ArgumentsArray *vars;
    
};



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class NetworkController;     // forward
class NetworkControllerLock; // forward


class NetworkControllerDelegate : public Object
{
    friend class NetworkController;
public:
    virtual ~NetworkControllerDelegate() {}
    
    
protected:
    NetworkControllerDelegate() {}
    
    virtual void oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const ArgumentsArray &arguments) = 0;

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class SocketUdpIn : public UdpSocket , private Object
{
public:
    SocketUdpIn( int port) :
        m_port( port )
    {
        className = "SocketUdpIn";
        SetAllowReuse( true );
    }
    
    ~SocketUdpIn(){}
    
    int getPort() const
    {
        return m_port;
    }
    
private:
    int m_port;
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

//! Another Controller, dedicated to network activities. Based on the simple yet powerfull oscpack (link?)
/********************************************//**
*
*   This class lets you register instances of OscEvent so that you can be notified when a certain message is received.
*   You can also simply send messages via the (?).
*
*   Note : this class use a thread to poll sockets, and every methods are _NOT_ guarranted thread safe!
*   But, good news, you can use a scoped lock : NetworkControllerLock.
*
*   Usage:
*   \code
*   NetworkController *instance =  ...
*
*   void foo()
*   {
*       NetworkControllerLock lock( instance );
*
*       // some stuffs ...
*
*   } // the lock is automaticaly released when it goes out of scope.
*   \endcode                                               
*
*    \sa NetworkControllerLock                                            
***********************************************/
class NetworkController : public AbstractController,
                          public Thread,
                          public osc::OscPacketListener
{
public:
    NetworkController();
    ~NetworkController();
    
    void setDelegate( NetworkControllerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    friend class NetworkControllerLock;

    
    //! Starts the internal thread.
    bool start();
    bool stop();
    
    bool sendOSC( const std::string &ip , const int port , const std::string &address , const ArgumentsArray &vars , bool broadcast = false );
    
    bool addPort    ( int port );
    void removePort ( int port );
    
    void removeAllSockets();
    
    void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );

    
private:
    
    enum { Output_Buffer_Size = 1024  };
    
    SocketUdpIn* getSocketForPort( int port );
    void internalDeleteSocket( SocketUdpIn* socket );
    
    void lockThread()
    {
        _lockThread = true;
    }
    
    void unlockThread()
    {
        _lockThread = false;
    }
    
    void run();
    
    NetworkControllerDelegate   *_delegate;
    
    volatile bool _lockThread;
    
    SocketReceiveMultiplexer _multiplexer;

    std::unordered_set<SocketUdpIn*> _socketList;
    

};

namespace NetworkTools
{
    const std::unique_ptr< ArgumentsArray > getArrayFromOSCMessage(const osc::ReceivedMessage& m);
    bool changeIpAddressTo(const std::string ip , const std::string netmask , const std::string interface );
    bool isConnected();
};




//!  Scoped Lock For NetworkController
/********************************************//**
*
*   In order to temporary stop oscPack's SocketReceiveMultiplexer (internally used in NetworkController),
*   and lock the thread, you can simply use an instance of this class.
*
*   Usage:
*   \code
*   NetworkController *instance =  ... 
*                                                  
*   void foo()
*   {
*       NetworkControllerLock lock( instance );
*
*       // some stuffs ...
*
*   } // the lock is automaticaly released when it goes out of scope.
*   \endcode                                               
*
*    \sa NetworkControllerLock                                            
***********************************************/
class NetworkControllerLock
{
public:
    NetworkControllerLock( NetworkController *controller ):
    _controller ( controller )
    {
        _controller->lockThread();
        _controller->_multiplexer.AsynchronousBreak();
    }
    
    ~NetworkControllerLock()
    {
        _controller->unlockThread();
    }
private:
    NetworkController* _controller;
};





#endif /* defined(____NetworkController__) */
