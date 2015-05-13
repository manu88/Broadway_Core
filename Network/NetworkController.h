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
#include <set>

#include "../Log/Log.h"
#include "../Config.h"

#include <oscpack/osc/OscReceivedElements.h>
#include <oscpack/osc/OscPacketListener.h>
#include <oscpack/osc/OscOutboundPacketStream.h>
#include <oscpack/ip/UdpSocket.h>
#include <oscpack/ip/PacketListener.h>

#include "../Internal/ThreadLock.h"
#include "../Internal/Object.h"

#include "../Internal/AbstractDelegate.h"

#include "../Scheduler/Event.h"

#include "../Internal/AbstractController.h"





/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class NetworkController;     // forward
class NetworkControllerLock; // forward


class NetworkControllerDelegate : public Object , public virtual AbstractDelegate
{
    friend class NetworkController;
public:
    virtual ~NetworkControllerDelegate() {}
    
    
protected:
    NetworkControllerDelegate() {}
    
    virtual void oscReceived(const std::string &ipAddress , const int port,const std::string & addressPattern, const VariantList &arguments) = 0;

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
    
    bool sendOSC( const std::string &ip , const int port , const std::string &address , const VariantList &vars = {} , bool broadcast = false );
    
    bool addPort    ( int port );
    void removePort ( int port );
    
    void removeAllSockets();
    
    void ProcessMessage( const osc::ReceivedMessage& m, const IpEndpointName& remoteEndpoint );
    
    
    inline static VariantList getVariantListFromOSCMsg( const osc::ReceivedMessage& m )
    {
        VariantList ret;
        
        try
        {
            
            
            for (auto it = m.ArgumentsBegin() ; it != m.ArgumentsEnd() ; it++ )
            {
                if ( (it)->IsFloat() )
                {
                    ret.push_back( (it)->AsFloat() );
                }
                
                else if ( (it)->IsInt32() )
                {
                    ret.push_back( (int) (it)->AsInt32() );
                }
                
                else if ( (it)->IsInt64() )
                {
                    ret.push_back( (int) (it)->AsInt64() );
                }
                
                else if ( (it)->IsString() )
                {
                    char chars[] = "\"";
                    std::string str = (it)->AsString();
                    for (unsigned int i = 0; i < strlen(chars); ++i)
                    {
                        str.erase (std::remove(str.begin(), str.end(), chars[i]), str.end());
                    }
                    ret.push_back( str );
                }
                else
                {
                    // No parsing method for this type, to be added ...
                    DEBUG_ASSERT( false );
                }
                
                //            it++;
                
            }
            
        }
        
        catch( osc::Exception& e )
        {
            Log::log("\n error while parsing message: %s " , e.what() );
        }

        
        return ret;
    }

    
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

static inline osc::OutboundPacketStream& operator<<( osc::OutboundPacketStream& p, const Variant& val)
{
    if ( val.isInt())
        p << val.getInt();
    
    else if ( val.isString() )
        p << val.getString().c_str();
    
    else if ( val.isFloat() )
        p << val.getFloat();
    
    else if ( val.isDouble() )
        p << val.getDouble();
    
    
    else if ( val.isBool() )
    {
        p << val.getBool();
    }
    
    else if ( val.isList() )
    {
        for ( const Variant &v : val.getList() )
            p<< v;
    }
    
    return p;

}

namespace NetworkTools
{
//    const std::unique_ptr< Variant > getArrayFromOSCMessage(const osc::ReceivedMessage& m);
    bool changeIpAddressTo(const std::string ip , const std::string netmask , const std::string interface );

    const std::set< std::string > getInterfacesList();
    const std::string getIpAddress( const std::string &interfaceName, bool ipV6 = false);

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
