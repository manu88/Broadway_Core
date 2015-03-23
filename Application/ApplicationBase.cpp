//
//  ApplicationBase.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "ApplicationBase.h"



ApplicationBase::ApplicationBase( const std::string &fileConfig ) :
_fileConfig ( fileConfig ),
_shouldQuit ( false ),
_hasQuit    ( false )
{
    
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
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool ApplicationBase::releaseApp()
{
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

