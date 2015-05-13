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
#include "../Internal/AbstractDelegate.h"


#include "SerialInterface.h"
#include "InterfaceEvent.h"
#include "GpioInterface.h"
#include "CanEvent.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class InterfaceControllerDelegate : public virtual AbstractDelegate
{
    friend class InterfaceController;
    
public:
    virtual ~InterfaceControllerDelegate() {}
    
protected:
    InterfaceControllerDelegate(){}
    
    virtual void inputChanged( const InterfaceEvent *event ) = 0;
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
    
    bool start();
    bool stop();
    
    /* GPIO part */
    
    void setNoPoll()
    {
        setInactive();
    }
    
    GpioEvent* addGpioInput(const int pinNumber , GPioInputType typeOfInput);
    bool removeGpioInput(const int pinNumber);
    
    GpioEvent* getGpioEventByID( int _id);
    
    void removeAllInputs();
    
    void sendGpo(const int pinNumber , const GpioState state);
    
    GpioEvent* getGpioEventByPin( const unsigned int pin);

    /* Serial part */
    
    SerialEvent *addSerial( const std::string &port);
    SerialEvent *getSerialEventByPort( const std::string &port);
    
    /* Can connections */
    
    CanEvent *addCanConnexion( const std::string &interface);

    
    void listActivesInput();
    
    
    
private:
    
    void run();
    

    void mainLoop();

    InterfaceControllerDelegate *m_delegate;
    
    std::unordered_set< InterfaceEvent* > m_inputs;
    

    

};

#endif /* defined(____GpioController__) */
