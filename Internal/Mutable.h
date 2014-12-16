//
//  Mutable.h
//  Broadway_test
//
//  Created by Manuel Deneu on 02/11/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_Mutable_h
#define Broadway_test_Mutable_h

#include <mutex>

struct MutableBase
{
    std::mutex mutex;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

template <typename T>
struct Mutable : MutableBase
{
public:
    Mutable( T* pointer):
    m_ptr ( pointer )
    {
        
    }
    
    ~Mutable()
    {
        delete m_ptr;
    }
    
    
    T * ptr(T & obj) { return &obj; } //turn reference into pointer!
    T * ptr(T * obj) { return obj; } //obj is already pointer, return it!
    
    T& operator* ()
    {
        return *m_ptr;
    }
    
    T* operator->() const
    {
        return  m_ptr ;
    }
    
    
private:
    T *m_ptr;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

struct MutableLock
{
    MutableLock(MutableBase *instance) : m_instance( instance )
    {
        m_instance->mutex.lock();
    }
    
    ~MutableLock()
    {
        m_instance->mutex.unlock();
    }
    
    MutableBase *m_instance;
    
};



#endif
