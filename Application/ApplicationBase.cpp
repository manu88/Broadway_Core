//
//  ApplicationBase.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "ApplicationBase.h"



ApplicationBase::ApplicationBase( const std::string &fileConfig ) :
_fileConfig    ( fileConfig ),
_isInitialized ( false ),
_shouldQuit    ( false ),
_hasQuit       ( false ),

_didStartEvent ( -1 )
{
    _scheduler.setDelegate( this );
}

ApplicationBase::~ApplicationBase()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::parseConfig()
{
    _appData.clear();
    
    if ( !_appData.parseFile(_fileConfig, '=') )
        return false;
    
    //
        
    return true;
}

bool ApplicationBase::saveConfig() const
{
    return _appData.saveToFile( _fileConfig , '=' );
}

bool ApplicationBase::reloadConfig()
{
    return parseConfig();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::applicationWillStart()
{
    return true;
}

void ApplicationBase::applicationDidStart()
{
    
}

void ApplicationBase::applicationWillStop()
{
    
}

void ApplicationBase::applicationDidStop()
{
    
}
/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */


bool ApplicationBase::initializeApp()
{
    if (!parseConfig() )
    {
        return false;
    }
    
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::start()
{
    if ( !initializeApp() )
        return false;
    

    if ( !applicationWillStart() )
        return false;
    
    
    _didStartEvent = _scheduler.registerTimedEvent( Timecode(0,0,0,100), 0, true);
    
    _scheduler.startFromThisThread();
    
    applicationDidStop();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::releaseApp()
{
    applicationWillStop();
    
    _scheduler.stop();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::sendCommand( const std::string & addressPattern, const VariantList &arguments)
{
    if ( addressPattern == "/quit" )
    {
        sendQuitSignal();
        return true;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

void ApplicationBase::scheduledEventReceived( Event &event)
{
    if ( event.isTimer() && (reinterpret_cast<TimedEvent*>(&event))->timerId == _didStartEvent )
    {
         applicationDidStart();
        _isInitialized   = true;
    }
    
    else
        eventReceived( event );
    
}



void ApplicationBase::eventReceived( const Event &event) {}
