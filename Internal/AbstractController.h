//
//  AbstractController.h
//  Broadway_test
//
//  Created by Manuel Deneu on 18/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__AbstractController__
#define __Broadway_test__AbstractController__

#include <iostream>
#include <unordered_set>
#include "Thread.h"


class AbstractController 
{
public:
    
    friend class Controllers;

    virtual bool start() = 0;
    virtual bool stop() = 0;
    
    const std::string getName() const
    {
        return _controllerName;
    }
    
    bool isReady() const
    {
        return _isReady;
    }
    
    bool isInactive() const
    {
        return _isInactive;
    }
    
    bool isActive() const
    {
        return !_isInactive;
    }
    

    
protected:
    AbstractController(const std::string &name);
    virtual ~AbstractController();
    
    void setReady()
    {
        _isReady = true;
        _isInactive = false;
    }
    
    void setUnReady()
    {
        _isReady = false;
    }
    

    
    void setInactive()
    {
        _isInactive = true;
    }
    

    
    // use this mutex to lock thread to avoid race-conditions
    // in both controller's thread and accessing-var methods.
    // exemple:
    //
    // void foo()
    // {
    //      ScopedLock( getControllerMutex() );
    //
    //      // access some vars
    //
    //  } // lock is auto-released with the scope
    
    std::mutex &getControllerMutex()
    {
        return _sync;
    }
    


private:
    static bool waitForAllControllersToBeReady(int timeOut = 10 /*SECS*/);
    static bool waitForControllerToBeReady( AbstractController*controller , int timeOut = 10 /*SECS*/);
    static bool allControllersReady();
    
    static int countReadyControllers();
    
    static bool waitForAllControllersToFinish(int timeOut = 10 /*SECS*/);
    static bool allControllersUnReady();

    bool _isReady;
    bool _isInactive;
    
    std::string _controllerName;
    
    // data mutex. See getControllerMutex() above.
    std::mutex  _sync;
    

    static std::unordered_set< AbstractController *> s_controllers;
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Controllers
{
public:
    static bool waitForAllControllersToBeReady(int timeOut = 10 /*SECS*/)
    {
        return AbstractController::waitForAllControllersToBeReady( timeOut );
    }
    
    static bool waitForControllerToBeReady( AbstractController*controller , int timeOut = 10 /*SECS*/)
    {
        return AbstractController::waitForControllerToBeReady( controller , timeOut );
    }
    
    static bool allControllersReady()
    {
        return AbstractController::allControllersReady();
    }
    
    static int countReadyControllers()
    {
        return AbstractController::countReadyControllers();
    }
    
    static bool waitForAllControllersToFinish(int timeOut = 10 /*SECS*/)
    {
        return  AbstractController::waitForAllControllersToFinish( timeOut );
    }
    static bool allControllersUnReady()
    {
        return  AbstractController::allControllersUnReady();
    }
    
    
};

#endif /* defined(__Broadway_test__AbstractController__) */









