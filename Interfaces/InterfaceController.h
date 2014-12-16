//
//  GpioController.h
//  
//
//  Created by Manuel Deneu on 08/08/14.
//
//

#ifndef ____GpioController__
#define ____GpioController__

#include <iostream>
#include <unordered_set>

#include "../Internal/Thread.h"
#include "../Scheduler/Event.h"
#include "../Internal/AbstractController.h"


#include "SerialInterface.h"
#include "InterfaceEvent.h"
#include "GpioInterface.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InterfaceControllerDelegate
{
    friend class InterfaceController;
    
public:
    virtual ~InterfaceControllerDelegate() {}
    
protected:
    InterfaceControllerDelegate(){}
    
    virtual void inputChanged( const int pin , const GpioState state ) = 0;    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InterfaceController :  public AbstractController ,
                        public Thread
{
public:

    
    InterfaceController();
    ~InterfaceController();
    
    void setDelegate( InterfaceControllerDelegate* delegate)
    {
        m_delegate = delegate;
    }
    
    bool init();
    void deInit();
    
    virtual bool start();
    virtual bool stop();
    
    
    GpioEvent* addGpioInput(const int pinNumber , GPioInputType typeOfInput);
    bool removeGpioInput(const int pinNumber);
    
    GpioEvent* getGpioEventByID( int _id);
    
    void removeAllGpioInputs();
    
    void sendGpo(const int pinNumber , const GpioState state);
    
    GpioEvent* getGpioEventByPin( const int pin);
    
    void listActivesInput();
    
    
    
private:
    
    virtual void run();
    

    void mainLoop();

    InterfaceControllerDelegate *m_delegate;
    
    std::unordered_set< GpioEvent* > m_inputs;
    

};

#endif /* defined(____GpioController__) */
