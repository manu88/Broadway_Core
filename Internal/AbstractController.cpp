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
_isReady    ( false ),
_isInactive ( false ),
_controllerName( name )
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
    
    while ( (time <= timeOut)  )
    {
        if ( allControllersReady() )
        {
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    

    

    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForControllerToBeReady( AbstractController*controller , int timeOut )
{
    clock_t start = clock();
    double time = 0;
    

    
    while ( (time <= timeOut)  )
    {
        if ( controller->isReady() || controller->isInactive() )
        {
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool waitForControllerToFinish( AbstractController*controller , int timeOut /*= 10 SECS*/)
{
    clock_t start = clock();
    double time = 0;
    
    
    
    while ( (time <= timeOut)  )
    {
        if ( controller->isReady() || controller->isInactive() )
        {
            return true;
        }
        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ bool AbstractController::waitForAllControllersToFinish(int timeOut /*SECS*/)
{
    clock_t start = clock();
    double time = 0;
    

    
    while ( (time <= timeOut )  )
    {
        if ( allControllersUnReady() )
            return true;

        
        
        time = ( clock() - start)/(double)CLOCKS_PER_SEC;
    }
    
    return false;

}