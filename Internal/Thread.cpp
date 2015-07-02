//
//  Thread.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <algorithm>
#include "../Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Thread.h"



int Thread::s_threadCount = 0;

std::vector< Thread*  > Thread::s_threadsPool = std::vector< Thread* >();

Thread::Thread( const std::string threadName ) :
    _isRunning  ( false ),
    _shouldStop ( false ),
    _thread     ( nullptr )
{
    className = "Thread";
    
    _threadName = threadName;
    
    s_threadsPool.push_back(this);
    
    s_threadCount++;
}

Thread::~Thread()
{

    DEBUG_ASSERT( m_thread == nullptr );
    
    auto iter = std::find( s_threadsPool.begin(), s_threadsPool.end(), this);
    
    if (iter != s_threadsPool.end())
        s_threadsPool.erase(iter);
    
    s_threadCount--;
    
    
    if ( s_threadCount <= 0)
    {
        DEBUG_ASSERT( s_threadsPool.size() == 0);
    }
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool Thread::startThread()
{

    if (!_isRunning)
    {
        
        _thread = new std::thread(  std::thread ( [](Thread *self)
                                               {
                                                   // flag as ok
                                                   self->_isRunning = true;

                                                   self->_shouldStop = false;
                                                   
                                                   self->setThreadID();
                                                   self->run();
                                                   
                                                   // after thread's task
                                                   self->threadEnded();
                                                   
                                               } , this));
        
        return true;
        
    }
    
    return false;
}

void Thread::setThreadID()
{
    _threadID = std::this_thread::get_id();
}

bool Thread::stopThread()
{
    _shouldStop = true;
    
    if ( _thread )
    {
        wakeUpThread();
        


        _thread->join();
        
        delete _thread;
        
        _thread = nullptr;
        
        return true;
    }
    
    return false;
    
}



/* **** **** **** **** **** **** **** **** **** **** **** **** */

void Thread::waitForCreation()
{

    while ( _isRunning == false)
    {
     
    }

}

bool Thread::isRunning() const
{
    return  _isRunning;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

void Thread::Lock()
{
    if( !_isRunning)
    {
        
        return;
    }
    
//    m_mutex.lock();

}

void Thread::UnLock()
{
    if( !_isRunning)
    {

        return;
    }
    
//    m_mutex.unlock();
}

void Thread::threadEnded()
{
    _isRunning = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool Thread::calledFromThisThread() const noexcept
{
    return std::this_thread::get_id() == _threadID;// _thread->get_id();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

