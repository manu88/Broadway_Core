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
    m_isRunning  ( false ),
    m_shouldStop ( false ),
    m_thread     ( nullptr )
{
    className = "Thread";
    
    m_threadName = threadName;
    
    s_threadsPool.push_back(this);
    
    s_threadCount++;
}

Thread::~Thread()
{

    DEBUG_ASSERT( m_thread == nullptr );
    
    auto iter = std::find(s_threadsPool.begin(), s_threadsPool.end(), this);
    
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

    if (!m_isRunning)
    {
        
        m_thread = new std::thread(  std::thread ( [](Thread *self)
                                               {
                                                   // flag as ok
                                                   self->m_isRunning = true;
//                                                   self->m_threadID = std::this_thread::get_id();
                                                   self->m_shouldStop = false;
                                                   
                                                   self->run();
                                                   
                                                   // after thread's task
                                                   self->threadEnded();
                                                   
                                               } , this));
        
        return true;
        
    }
    
    return false;
}

bool Thread::stopThread()
{
    
    if ( m_thread )
    {
        m_shouldStop = true;
//        getThreadByID( m_threadID )->join();
        m_thread->join();
        
        delete m_thread;
        m_thread = nullptr;
        
        return true;
    }
    
    return false;
    
}



/* **** **** **** **** **** **** **** **** **** **** **** **** */

void Thread::waitForCreation()
{

    while (m_isRunning == false)
    {
     
    }

}

bool Thread::isRunning() const
{
    return m_isRunning;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

void Thread::Lock()
{
    if( !m_isRunning)
    {
        
        return;
    }
    
//    m_mutex.lock();

}

void Thread::UnLock()
{
    if( !m_isRunning)
    {

        return;
    }
    
//    m_mutex.unlock();
}

void Thread::threadEnded()
{
    m_isRunning = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool Thread::calledFromThisThread() const
{
    return std::this_thread::get_id() == m_thread->get_id();
}

/*static*/ void Thread::sleepFor( const Timecode &tc)
{
    std::this_thread::sleep_for (std::chrono::milliseconds( tc.getInMs() ) );
}

