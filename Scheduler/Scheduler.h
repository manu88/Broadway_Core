//
//  Scheduler.h
//  
//
//  Created by Manuel Deneu on 11/08/14.
//
//

#ifndef ____Scheduler__
#define ____Scheduler__

#include <iostream>
#include <unordered_map>

//#include <sys/time.h>
//#include <chrono>
//#include <mutex>
//#include <condition_variable>

#include "Event.h"
#include "../Internal/Object.h"
#include "../Internal/Thread.h"
#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"

#include "Timecode.h"
#include "Event.h"
#include "TimeDefs.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


/*  Internal Timer */

typedef std::chrono::steady_clock       Clock;
typedef std::chrono::time_point<Clock>  Timestamp;
typedef std::chrono::milliseconds       Duration;


class TimedEvent : public Event
{
public:
    TimedEvent() :
    Event   ( Event_Timer ),
    running ( false),
    timerId ( ++s_nextId )
    {
        s_timerCount++;

    }
    
    ~TimedEvent()
    {
        s_timerCount--;
        
        if ( s_timerCount<=0 )
        {
            s_timerCount = 0;
           // s_nextId = 0;
        }
    }
    
    TimedEvent(Timestamp next_, Duration period_ ) noexcept :
    Event   ( Event_Timer ),
    next    ( next_    ),
    period  ( period_  ),
    running ( false    ),
    timerId ( ++s_nextId )
    {
    }
    
    TimedEvent( TimedEvent const& r) = delete;
    
    TimedEvent( TimedEvent&& r) noexcept :
    Event   ( Event_Timer ),    
    next    ( r.next    ),
    period  ( r.period  ),
    running ( r.running ),
    timerId ( r.timerId )
    {
    }
    
    TimedEvent& operator=( TimedEvent const& r) = delete;
    
    TimedEvent& operator=( TimedEvent&& r)
    {
        if (this != &r)
        {
            next    = r.next;
            period  = r.period;
            running = r.running;
            timerId = r.timerId;
        }
        return *this;
    }
    
    
    Timestamp   next;
    Duration    period;
    bool        running;
    int         timerId;
    
private:
    static int s_timerCount;
    static int s_nextId;
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Chrono : public Object
{
public:
    
    Chrono()
    {}
    
    ~Chrono()
    {}
    
    void reset()
    {
        _start = std::chrono::high_resolution_clock::now();
    }
    
    inline long long  elapsedTimeInMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _start ).count();
    }
    
private:
    
    std::chrono::high_resolution_clock::time_point _start;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class SchedulerDelegate : public virtual AbstractDelegate
{
    friend class Scheduler;
public:
    virtual ~SchedulerDelegate() {}

    virtual void scheduledEventReceived( Event &event) = 0;
protected:
    SchedulerDelegate() {}
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Scheduler : public AbstractController,
                  public Thread
{

public:

    Scheduler();
    ~Scheduler();
    
    void setDelegate( SchedulerDelegate *delegate)
    {
        m_delegate = delegate;
    }

    // timers handle
    int registerTimedEvent( Timecode start , Timecode period , bool oneShot );
    TimedEvent* addTimer(  Timecode start , Timecode period , bool oneShot );
    
    bool isAlreadyRegistered( TimedEvent* eventToFind );
    
    
    TimedEvent* getEventByID( int _id);
    
    bool removeEvent( int id );
    bool removeEvent( TimedEvent* event );
    void removeAllEvents();
    

    /* Globals functions */
    static double getTimeInMs()
    {
        struct timeval t;
        
        gettimeofday( &t, 0 );
        
        return ((double)t.tv_sec*1000.) + ((double)t.tv_usec / 1000.);
    }
    
    static Timecode getCurrentTC()
    {
        Timecode current;
        current.setToCurrentTime();
        
        return current;
    }
    
    
    /* **** **** */
    
    Timecode getRunningTC()
    {
        Timecode current;
        current.setToCurrentTime();
        
        return current - _deltaTC;
    }
    
    void setRunningTC( const Timecode tc  )
    {
        ScopedLock lock(  getControllerMutex() );
        
        
        _deltaTC = tc;
    }
    
    
    
    // block for a certain duration
    static long long blockThreadFor(const Timecode & duration );
    static long long blockThreadFor(const unsigned long ms );
    
    bool start();
    bool stop();
    
    void startFromThisThread();
    
private:
    
    void run();
    
    bool destroy(int id);
    bool destroyAll();
    bool exists( int id);
    int createImpl( TimedEvent && item);
    
    SchedulerDelegate *m_delegate;


    std::unordered_map<int, TimedEvent > active;
    
    // Comparison functor to sort the timer "queue" by Instance::next
    struct NextActiveComparator
    {
        bool operator()(const TimedEvent &a, const TimedEvent &b) const
        {
            return a.next < b.next;
        }
    };
    
    NextActiveComparator comparator;
            
    // Queue is a set of references to Instance objects, sorted by next
    typedef std::reference_wrapper< TimedEvent > QueueValue;

    typedef std::multiset<QueueValue, NextActiveComparator> Queue;
    Queue queue;
            

    Timecode _deltaTC;
    
    
    bool _otherThread;
    
};
            
            



#endif /* defined(____Scheduler__) */
