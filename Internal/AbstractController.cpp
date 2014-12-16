//
//  AbstractController.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 18/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <time.h>

#include "AbstractController.h"
#include "../Scheduler/Scheduler.h"
#include "../Log/Log.h"



std::unordered_set< AbstractController *> AbstractController::s_controllers;

AbstractController::AbstractController( const std::string &name ) :
_isReady ( false ),
m_controllerName( name )
{
    s_controllers.insert( this );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

AbstractController::~AbstractController()
{
    s_controllers.erase( this);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::allControllersReady()
{
                
    return countReadyControllers() == static_cast< int >( s_controllers.size() );
}

/*static*/ int AbstractController::countReadyControllers()
{
    int count = 0;
    for (auto i : s_controllers )
    {
        if( i->isReady() || i->isInactive() )

            
            count++;

    }
    
    return count;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::allControllersUnReady()
{
    for (auto i : s_controllers )
    {
        if(  i->isReady() && i->isActive() )
            return false;
    }
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForAllControllersToBeReady(int timeOut /*SECS*/)
{
    clock_t start = clock();
    double time = 0;
    
    Log::log("Wait for %li controller(s) ... " , (s_controllers.size() - countReadyControllers() ) );
    
    while ( (time <= timeOut)  )
    {
        if ( allControllersReady() )
        {
            Log::log("All controllers are OK");
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    Log::log("warning, time out failed: ");

    if ( allControllersReady() )
        Log::log(" but all controllers seem ready ");
    
    else
    {
        Log::log(" _NOT_ all controllers are ready ");
        for ( auto i : s_controllers )
        {
            if( (!i->isReady() ) )
                Log::log("\n %s is _NOT_ ready" , i->m_controllerName.c_str() );
        }
    }
    
    // timeout failed
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForAllControllersToFinish(int timeOut /*SECS*/)
{
    clock_t start = clock();
    double time = 0;
    
    Log::log("Wait for %li controller(s) to finish ... " , countReadyControllers()  );
    
    while ( (time <= timeOut )  )
    {
        if ( allControllersUnReady() )
        {
            Log::log("All controllers' tasks are Finished");
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    Log::log("warning, time out failed: ");
    
    if ( allControllersUnReady() )
        Log::log(" but all controllers are done ");
    
    else
    {
        Log::log(" _NOT_ all controllers are done ");
        
        for ( auto i : s_controllers )
        {
            if( (i->isReady() ) )
                Log::log("\n %s is still ready" , i->m_controllerName.c_str() );
        }
        
    }
    
    // timeout failed
    return false;

}