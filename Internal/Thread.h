//
//  Thread.h
//  Broadway_test
//
//  Created by Manuel Deneu on 17/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__Thread__
#define __Broadway_test__Thread__

#include <thread>
#include <condition_variable>

#include "Object.h"
#include "../Scheduler/Timecode.h"


typedef std::unique_lock<std::mutex>    ScopedLock;

class Thread : public virtual Object
{
public:
    Thread( const std::string threadName);
    ~Thread();
    
    bool startThread();
    bool stopThread();
    bool isRunning() const;


    void Lock();
    void UnLock();
    
    /* **** **** **** **** */
    
    inline void wakeUpThread() noexcept
    {
        _wakeUp.notify_all();
        
    }
    
    inline void wait ( ScopedLock &lock)
    {
        _wakeUp.wait( lock );
        
    }
    
    inline void waitUntil( ScopedLock &lock , std::chrono::time_point<std::chrono::steady_clock> timestamp )
    {
        _wakeUp.wait_until( lock , timestamp );
    }

    /* **** **** **** **** */    
    
    bool calledFromThisThread() const;
    
    static int getThreadCount()
    {
        return s_threadCount;
    }
    
    static void sleepFor( const Timecode &tc);
    
protected:
    // user defined main() thread
    virtual void run() = 0;
    
    bool threadShouldStop() const
    {
        return m_shouldStop;
    }
    
    void sendAsyncStop()
    {
        m_shouldStop = true;
    }

    
    void waitForCreation();
    
    
    

    
private:
    
    void threadEnded();


    volatile bool       m_isRunning;
    volatile bool       m_shouldStop;
    
    std::condition_variable _wakeUp;
    
    std::string         m_threadName;
    
    /* thread pool */
    std::thread *m_thread;
    
    static  std::vector< Thread* > s_threadsPool ;
    
    static int s_threadCount;
};

#endif /* defined(__Broadway_test__Thread__) */
