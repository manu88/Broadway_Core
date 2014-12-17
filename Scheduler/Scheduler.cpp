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
    ScopedLock lock(m_sync);
    
    return active.find(id) != active.end();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

TimedEvent* Scheduler::getEventByID( int _id)
{
    ScopedLock lock(m_sync);
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
    
    removeEvent( event->getElementId() );
    
    return true;
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Scheduler::removeAllEvents()
{
    destroyAll();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool Scheduler::start()
{

    
    return startThread();
}

bool Scheduler::stop()
{

    m_wakeUp.notify_all();
    
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void Scheduler::run()
{
    setReady();
    

    
    while ( !threadShouldStop() )
    {
        ScopedLock lock( m_sync);

        if (queue.empty() )
        {
            // Wait (forever) for work
            m_wakeUp.wait(lock);
        }
/*
        if ( ( m_mouseListener != nullptr ) && m_mouseListener->pollInput() )
            m_delegate->mouseEventReceived(m_mouseListener->getLastState() );
  */      
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
                    } else {
                        active.erase(instance.getElementId() );
                    }
                }
            }
            else
            {
                // Wait until the timer is ready or a timer creation notifies
             //   m_wakeUp.wait_until(lock, instance.next);
            }
        }
    }
    
    
    setUnReady();
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

int Scheduler::createImpl( TimedEvent && item)
{
    ScopedLock lock( m_sync);

    auto iter = active.emplace(item.getElementId(), std::move(item));
    queue.insert(iter.first->second);
    m_wakeUp.notify_all();
    return item.getElementId();
}

bool Scheduler::destroy( int id)
{
    ScopedLock lock( m_sync );
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
        queue.erase(std::ref(i->second));
        active.erase(i);
    }
    
    m_wakeUp.notify_all();
    return true;
}

bool Scheduler::destroyAll()
{
    ScopedLock lock( m_sync );
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
            queue.erase(std::ref(i.second));
            
        }
    }
    
    active.clear();
        
    m_wakeUp.notify_all();
    return true;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ long long Scheduler::blockThreadFor(const Timecode & duration )
{
    return blockThreadFor( duration.getInMs() );
}

/*static*/ long long Scheduler::blockThreadFor(const unsigned int ms )
{

    const auto t0 = std::chrono::high_resolution_clock::now();
    
    std::this_thread::sleep_for( std::chrono::milliseconds( ms ) );
    
    const auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono:: milliseconds total_ms = std::chrono::duration_cast< std::chrono::milliseconds>(t1 - t0);
    
    Log::log("this_thread_sleep %i ms - real = %i" , ms , total_ms.count() );

    return total_ms.count();
}





