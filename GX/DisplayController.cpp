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

_currentMode( DisplayInformations::makeInvalid() ),

_displayIsOn         ( false ),
_delegate            ( nullptr ),

_clearColor ( makeColor( 0,0,0) ),

m_currentElement     ( nullptr ),
m_shouldClearContext ( true    ),
m_frameRate          ( 0 )
{

    className = "Display controller";


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
    
    wakeUpThread();
    
    m_currentElement = element;

    m_currentElement->setParentElement( this );
    
    m_currentElement->setBounds( getBounds() );
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::setCleanColor( GXColor color)
{
    _clearColor = color;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearScreen()
{
    ScopedLock lock( getControllerMutex()  );
    
    wakeUpThread();
    
    m_shouldClearContext = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::update()
{
    ScopedLock lock( getControllerMutex()  );
    
    wakeUpThread();
    
    if (m_currentElement)
        m_currentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::run()
{
    _impl.initPlateform();

    DisplayInformations mode = _impl.getCurrentDisplayInformations();
        
    _impl.initDisplay();

    setBounds(0, 0, mode.size.width , mode.size.height);
    
    setReady();
    clearContext();
    
    _delegate->displayDidChange( DISPLAY_ACTIVE );
    
    while ( !threadShouldStop() )
    {
        ScopedLock lock( getControllerMutex()  );
        
        if ( m_currentElement == nullptr)
        {
            wait( lock );
        }
        m_frameRateClock.reset();
        
        //Thread::sleepFor(Timecode(0,0,0,10));
        
        
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
            _impl.checkErrors();
            
            if ( !m_currentElement->isPrepared() )
            {
//                Lock();
                m_currentElement->prepare();
//                UnLock();
            }
            
            if ( m_currentElement->shouldBeRemoved() )
            {
//                Lock();
                m_currentElement->cleanUp();
                m_currentElement = nullptr;
//                UnLock();
                
                break;
            }
            
            if ( m_currentElement->m_changed_flag && m_currentElement->m_callChangedOnGUIThread)
            {
//                Lock();
                m_currentElement->changed();
                m_currentElement->m_changed_flag = false;
//                UnLock();
                
            }
            
            else if ( m_currentElement->needsDisplay() )
            {
                
                if ( threadShouldStop() )
                    break;
                

                
//                Lock();
                m_currentElement->paint( _updateRect , _anim );
                
                m_currentElement->setUpdated();
                
//                UnLock();
                
                updateContext();
                
            }
            
            
        }
        

        m_frameRate = m_frameRateClock.elapsedTimeInMs();



    }
    

    if ( m_currentElement )
    {
//        Lock();
        m_currentElement->cleanUp();
//        UnLock();
    }
    _impl.deInitDisplay();
    
    setUnReady();    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::displayChangeNotification( DisplayNotification notification )
{
    
    if ( _delegate )
        _delegate->displayDidChange( notification );
    
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

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

    
    GXPath::clearRect( getBounds(), _clearColor );
    
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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::saveCurrentConfiguration( const std::string &file)
{
    return getDisplayInformationsAsDatabase( getCurrentMode() ).saveToFile( file, '=');
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::loadConfigurationFile( const std::string &file)
{
    Database<std::string> data;
    
    if ( data.parseFile( file, '=') )
        return setVideoModeTo( getDisplayInformationsFromDatabase( data ));

    else
        return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ Database<std::string> DisplayController::getDisplayInformationsAsDatabase( const DisplayInformations &info)
{


    return {
            std::make_pair( "WIDTH"     , std::to_string ( info.size.width )  ),
            std::make_pair( "HEIGHT"    , std::to_string ( info.size.height ) ),
            std::make_pair( "FRAMERATE" , std::to_string ( info.framerate )   ),
            std::make_pair( "TYPE"      , std::to_string ( info.type )        ),
            std::make_pair( "NATIVE"    , std::to_string ( info.native )      ),
            std::make_pair( "RATIO"    , std::to_string  ( info.aspectRatio ) ),
           };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ DisplayInformations DisplayController::getDisplayInformationsFromDatabase( const Database<std::string> &data)
{
    return  DisplayInformations {  /* native*/               data.getValueForItemNameAsBool("NATIVE") ,
                                   /* Type*/  (DisplayType)  data.getValueForItemNameAsInt ("TYPE"),
        
                                   /* size*/ makeSize( data.getValueForItemNameAsInt ("WIDTH") ,
                                                       data.getValueForItemNameAsInt ("HEIGHT")
                                                      ),
        
                                   /* frate*/     data.getValueForItemNameAsInt ("FRAMERATE") ,
                                   /* aspect ratio*/  data.getValueForItemNameAsFloat ("RATIO")
                                };
}

