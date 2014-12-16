//
//  OMXThreadLock.h
//  
//
//  Created by Manuel Deneu on 12/08/14.
//
//

#ifndef _OMXThreadLock_h
#define _OMXThreadLock_h

#include "Thread.h"

/*

    Scoped Lock for Thread class
 
 */
class ThreadLock
{
public:
    ThreadLock( Thread* threadToLock) :
        m_thread ( threadToLock )
    {
        m_thread->Lock();
    }
    
    ~ThreadLock()
    {
        m_thread->UnLock();
    }
private:
    Thread* m_thread;
    
};

#endif
