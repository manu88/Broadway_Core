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
    
    /*
    std::thread *getThreadByID( std::thread::id  id)
    {
        for (auto &i : s_threadsPool )
        {
            if (i.get_id() == id )
                return &i;
        }
        return nullptr;
    }
     */
    

    
private:
    
    void threadEnded();


    volatile bool       m_isRunning;
    volatile bool       m_shouldStop;
    std::string         m_threadName;
    
    /* thread pool */
    std::thread *m_thread;
    
    static  std::vector< Thread* > s_threadsPool ;
    
    static int s_threadCount;
};

#endif /* defined(__Broadway_test__Thread__) */
