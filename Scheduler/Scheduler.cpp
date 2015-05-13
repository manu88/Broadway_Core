//
//  Scheduler.cpp
//  
//
//  Created by Manuel Deneu on 11/08/14.
//
//

#include <thread>
#include "Scheduler.h"

#include "../Log/Log.h"
#include "../Network/NetworkController.h"

int TimedEvent::s_timerCount = 0;
int TimedEvent::s_nextId     = 0;



Scheduler::Scheduler() :
    AbstractController( "Scheduler" ),
    Thread("Scheduler"),

    queue(comparator)
{

}

Scheduler::~Scheduler()
{

    removeAllEvents();
    

}

bool Scheduler::listenForMouseInput()
{

    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Scheduler::registerTimedEvent( Timecode start , Timecode period , bool oneShot )
{
    
    return createImpl( TimedEvent( Clock::now() + Duration( start.getInMs() ), Duration( period.getInMs() )  ));
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Scheduler::isAlreadyRegistered( TimedEvent* eventToFind )  
{
    
    return exists( eventToFind->getElementId() );
    
}

bool Scheduler::exists(int id) 
{
    ScopedLock lock( getControllerMutex() );
    
    return active.find(id) != active.end();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

TimedEvent* Scheduler::getEventByID( int _id)
{
    ScopedLock lock( getControllerMutex() );
    auto it = active.find( _id ) ;//!= active.end();
    
    if (it != active.end() )
        return &it->second;
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
bool Scheduler::removeEvent( int id )
{
    return destroy( id );
}

bool Scheduler::removeEvent( TimedEvent* event )
{
    if ( event == nullptr)
        return false;

    return removeEvent( event->getElementId() );
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Scheduler::removeAllEvents()
{
    destroyAll();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Scheduler::start()
{
    _otherThread = true;
    
    return startThread();
}

void Scheduler::startFromThisThread()
{
    _otherThread = false;
    
    Thread::setThreadID();
    run();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Scheduler::stop()
{
//    ScopedLock lock( getControllerMutex() );

    wakeUpThread();
    
    removeAllEvents();
    
    if ( _otherThread )
        return stopThread();

    else
    {
        sendAsyncStop();
//        Controllers::waitForControllerToFinish( this );
        

        return true;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Scheduler::run()
{
    setReady();
    
    while ( !threadShouldStop() )
    {
        ScopedLock lock( getControllerMutex() );

        if (queue.empty() )
        {
            // Wait (forever) for work

            waitUntil( lock , Clock::now() + Duration( 1000 ) );
            //wait( lock );
            if ( threadShouldStop() )
                break;
        }
        
        
        if ( !queue.empty() )
        {
         
            auto firstInstance = queue.begin();
            TimedEvent& instance = *firstInstance;
            auto now = Clock::now();
            if (now >= instance.next)
            {
                queue.erase(firstInstance);
                
                // Mark it as running to handle racing destroy
                instance.running = true;
                
                // Call the handler
                lock.unlock();
                
                if ( m_delegate->delegateReadyForController(this ) )
                    m_delegate->scheduledEventReceived( instance );
                
                lock.lock();
                
                if ( threadShouldStop() )
                {
                    break;
                }
                else if (!instance.running)
                {
                    // Running was set to false, destroy was called
                    // for this Instance while the callback was in progress
                    // (this thread was not holding the lock during the callback)
                    active.erase(instance.getElementId() );
                }
                else
                {
                    instance.running = false;
                    
                    // If it is periodic, schedule a new one
                    if (instance.period.count() > 0)
                    {
                        instance.next = instance.next + instance.period;
                        queue.insert(instance);
                    }
                    
                    else
                    {
                        active.erase(instance.getElementId() );
                    }
                }
            }
            else
            {
                // Wait until the timer is ready or a timer creation notifies
//                _wakeUp.wait_until(lock, instance.next);
                waitUntil( lock, instance.next );
                
            }
        }
    }
    
    
    setUnReady();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Scheduler::createImpl( TimedEvent && item)
{
    ScopedLock lock( getControllerMutex() );

    auto iter = active.emplace(item.timerId, std::move(item));
    queue.insert(iter.first->second);
    
//    _wakeUp.notify_all();
    wakeUpThread();
    
    return item.timerId; //getElementId();
}

bool Scheduler::destroy( int id)
{
    ScopedLock lock( getControllerMutex() );
    auto i = active.find(id);
    
    if (i == active.end())
        return false;
    
    else if (i->second.running)
    {
        // A callback is in progress for this Instance,
        // so flag it for deletion in the worker
        i->second.running = false;
    }
    else
    {
        queue.erase( std::ref( i->second ) );
        active.erase( i );
    }
    
    wakeUpThread();
    
    return true;
}

bool Scheduler::destroyAll()
{
    if (!calledFromThisThread())
        ScopedLock lock( getControllerMutex() );
    
    for ( auto &i : active )
    {
    
        if (i.second.running)
        {
            // A callback is in progress for this Instance,
            // so flag it for deletion in the worker
            i.second.running = false;
        }
        else
        {
            queue.erase(std::ref( i.second ) );
            
        }
    }
    
    active.clear();
        
    wakeUpThread();
    
    return true;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ long long Scheduler::blockThreadFor(const Timecode & duration )
{
    return blockThreadFor( duration.getInMs() );
}

/*static*/ long long Scheduler::blockThreadFor(const unsigned long ms )
{

    const auto t0 = std::chrono::high_resolution_clock::now();
    
    std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
    
    const auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono:: milliseconds total_ms = std::chrono::duration_cast< std::chrono::milliseconds>(t1 - t0);
    

    return total_ms.count();
}





