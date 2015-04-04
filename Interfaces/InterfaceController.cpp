//
//  GpioController.cpp
//  
//
//  Created by Manuel Deneu on 08/08/14.
//
//

#include <stdio.h>
#include <assert.h>

#include <sys/time.h>

#ifdef TARGET_RASPBERRY_PI
    #include <bcm2835.h>
#endif

#include "InterfaceController.h"

#include "../Internal/Element.h"
#include "../Internal/ThreadLock.h"
#include "../Log/Log.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// empty impl
void InterfaceControllerDelegate::inputChanged( const InterfaceEvent *event )
{
    /* base class 'inputChanged' called */
    assert(false);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


InterfaceController::InterfaceController():
    AbstractController ( "Interface controller" ),
    Thread             ( "Interface Thread"),
    m_delegate         ( nullptr )
{
    className = "Interface controller";
    if (! init())
    {
        Log::log("\n error while initializing gpio interface");
    }
}

InterfaceController::~InterfaceController()
{
    stopThread();
    
    removeAllInputs();
    
    deInit();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::init()
{
    return GpioEvent::init();
}


void InterfaceController::deInit()
{
    return GpioEvent::deInit();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::start()
{
    return startThread();
}

bool InterfaceController::stop()
{


    
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent* InterfaceController::addGpioInput(const int pinNumber  , GPioInputType typeOfInput )
{
    ThreadLock lock(this); // scopedLock

    wakeUpThread();
    
    if ( !getGpioEventByPin( pinNumber ) )
    {
        GpioEvent *event = new GpioEvent(pinNumber , typeOfInput);
        m_inputs.insert( event );

        return event;
    }


    return nullptr ;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent* InterfaceController::getGpioEventByID( int _id)
{
    for ( auto i : m_inputs )
    {        
        if ( i->isGpioEvent() && ( i->getElementId() == _id) )
        {
            return  dynamic_cast< GpioEvent*>( i) ;
        }
        
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool InterfaceController::removeGpioInput(const int pinNumber)
{
    ThreadLock lock(this); // scopedLock
    
    wakeUpThread();
    
    GpioEvent *eventToRemove  = nullptr;
    
    if ( ( eventToRemove = getGpioEventByPin( pinNumber ) ) != nullptr )
    {
        ScopedLock ctlLock(getControllerMutex() );
        delete eventToRemove;
        
        m_inputs.erase( eventToRemove );
        
        return true;
    }
    
    return false;
}

void InterfaceController::removeAllInputs()
{
    ScopedLock lock( getControllerMutex() );
    
    wakeUpThread();
    
    for ( auto i : m_inputs )
    {
        i->cleanup();
        delete i;
    }
    
    m_inputs.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GpioEvent* InterfaceController::getGpioEventByPin( const unsigned int pin)
{
    for ( auto i : m_inputs )
    {
        if ( i->isGpioEvent() )
        {
            GpioEvent * gpio = dynamic_cast< GpioEvent*>( i );
            
            if ( gpio->getPin() == pin)
                return gpio;
        }
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void InterfaceController::listActivesInput()
{

}

void InterfaceController::sendGpo(const int pinNumber , const GpioState state)
{
    if (getGpioEventByPin( pinNumber))
        Log::log("WARNING : trying to use a GPIO Pin registered as input for output... DANGEROUS!");
    else
        GpioEvent::setGpio(pinNumber, state);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SerialEvent* InterfaceController::addSerial( const std::string &port)
{
    ThreadLock lock(this);
    
    wakeUpThread();
    
    SerialEvent* event = new SerialEvent( port );
    m_inputs.insert( event );
    
    event->openPort();
    
    return event;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

SerialEvent* InterfaceController::getSerialEventByPort( const std::string &port)
{
    for ( auto i : m_inputs)
    {
        if (i->isSerialEvent() )
        {
            SerialEvent *event = dynamic_cast<SerialEvent*>( i );
            if ( event->getPort() == port )
                return event;
        }
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

CanEvent* InterfaceController::addCanConnexion( const std::string &interface)
{
    ThreadLock lock(this);
    
    wakeUpThread();
    
    CanEvent* event = new CanEvent( interface );
    m_inputs.insert( event );
    
    event->connect();
    
    return event;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Loop : read registered inputs pins and call the delegate on changes
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void InterfaceController::mainLoop()
{
    while ( !threadShouldStop() )
    {
        ScopedLock lock( getControllerMutex()  );
        
        if ( m_inputs.empty() )
        {
            printf("\n WAIT FOREVER IN INTERFACES CTRLR \n");
            wait( lock );
            
            printf("\n after lock");
        }


        for ( auto i : m_inputs )
        {
            
            if ( i->changed() &&  m_delegate->delegateReadyForController( this )  )
            {
                m_delegate->inputChanged ( i );
            }
        }
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void InterfaceController:: run()
{
    setReady();
    mainLoop();
    setUnReady();    
}



