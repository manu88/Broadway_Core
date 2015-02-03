//
//  DisplayController.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 22/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <assert.h>
#include "DisplayController.h"
#include "../Internal/ThreadLock.h"
#include "../Log/Log.h"

#include "GXPath.h"


/*static*/ DisplayController* DisplayController::s_instance = nullptr;


DisplayController::DisplayController() :
AbstractController   ( "Display controller" ),
Thread               ( "GUI"   ),

_impl( this ),

_displayIsOn         ( false ),
_delegate            ( nullptr ),
m_currentElement     ( nullptr ),
m_shouldClearContext ( true    ),
m_frameRate          ( 0 )
{

    className = "Display controller";
    
#ifdef USE_OPENMAXIL
    m_RBP.Initialize();
    m_OMX.Initialize();
#endif

    s_instance = this;
    
    setLayer(1);

}

DisplayController::~DisplayController()
{
    stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::start()
{
    return startThread();
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::stop()
{
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::removeElement( GXElement* element)
{
    ThreadLock l(this);
    element->removeFromParent();
    
}

void DisplayController::setDisplayedElement( GXElement* element)
{
    ThreadLock l(this);    
    m_currentElement = element;

    m_currentElement->setParentElement( this );
    
    m_currentElement->setBounds( getBounds() );
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearScreen()
{
    m_shouldClearContext = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::update()
{
    ThreadLock l(this);
    
    if (m_currentElement)
        m_currentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::run()
{
//    initializeEGL();
    

    _impl.initPlateform();

    
  
    DisplayInformations mode = _impl.getCurrentDisplayInformations();
    

    
    printf("\n ==== MODE  =====" );
    printf("\n\t W = %i H = %i" , mode.size.width , mode.size.height );
    printf("\n\t native %s" , mode.native? "YES" : "NO ");
    printf("\n\t framerate : %i" , mode.framerate);
    printf("\n\t aspectRatio : %f " , mode.aspectRatio);
    
    _impl.initDisplay();

    setBounds(0, 0, mode.size.width , mode.size.height);
    
    setReady();
    clearContext();
    
    _delegate->displayDidChange( DISPLAY_ACTIVE );
    
    while ( !threadShouldStop() )
    {

        m_frameRateClock.reset();
        
        if ( needsDisplay() )
        {
            setUpdated();
        }
        
        if (m_shouldClearContext )
        {
            clearContext();
            updateContext();
            m_shouldClearContext = false ;
        }
        
        else if (m_currentElement)
        {
            if ( !m_currentElement->isPrepared() )
            {
                Lock();
                m_currentElement->prepare();
                UnLock();
            }
            
            if ( m_currentElement->shouldBeRemoved() )
            {
                Lock();
                m_currentElement->cleanUp();
                m_currentElement = nullptr;
                UnLock();
                
                break;
            }
            
            if ( m_currentElement->m_changed_flag && m_currentElement->m_callChangedOnGUIThread)
            {
                Lock();
                m_currentElement->changed();
                m_currentElement->m_changed_flag = false;
                UnLock();
                
            }
            
            else //if ( m_currentElement->needsDisplay() )
            {

                if ( threadShouldStop() )
                    break;
                
                Lock();                
                m_currentElement->paint( _updateRect , _anim );
                
                m_currentElement->setUpdated();
                
                UnLock();
                
                updateContext();
                
            }
            
            
        }
        

        m_frameRate = m_frameRateClock.elapsedTimeInMs();



    }
    

    if ( m_currentElement )
    {
        Lock();
        m_currentElement->cleanUp();
        UnLock();
    }
    setUnReady();    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::displayChangeNotification( DisplayNotification notification )
{
    printf("\n Got a notification from display");
    
    if ( notification & HDMI_UNPLUGGED )
        printf("\n HDMI is Unplugged");
    
    if ( notification & HDMI_ATTACHED )
        printf("\n HDMI is Attached");
    
    if ( notification & HDMI_DVI )
        printf("\n HDMI is DVI");
    
    if ( notification & HDMI_HDMI )
        printf("\n HDMI is in HDMI mode and ON");
    
    
    if ( _delegate )
        _delegate->displayDidChange( notification );
    
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::setVideoModeTo( const DisplayInformations &mode)
{

    
    return true;
}

void DisplayController::init()
{
    assert( calledFromThisThread() );
    


    
}






/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::updateContext()
{
    
    assert( calledFromThisThread() );
    
    _impl.update();
    


}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearContext()
{
    assert( calledFromThisThread() );
    
    GXPath::clearRect( getBounds(), makeColor(0, 0, 0) );
    
}





/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::paint( const GXRect &rect, GXAnimation* anim )
{
    
}
void DisplayController::prepareRessources()
{
    
}
void DisplayController::deleteRessources()
{
    
}

void DisplayController::changed()
{
    
}

