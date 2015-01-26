
//
//  GXScene.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//


#include <stdio.h>
#include <algorithm>    
#include <assert.h>
#include <limits.h>

#include "../Config.h"

#include "DisplayController.h"
#include "../Internal/ThreadLock.h" 

#include "GXScene.h"
#include "GXPath.h"

/* **** **** **** **** **** **** **** **** **** **** **** */

GXScene::GXScene()
{
    className = "GXScene";
    
    setBackgroundColor( makeColor(255, 255, 255 ) );
    
}

GXScene::~GXScene()
{
    

}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::addElement( GXElement* element )
{
    DEBUG_ASSERT(element);
    
    ThreadLock l( DisplayController::getController() );
    
    
    if ( isAlreadyInScene( element ) )
        return false;
    

    element->setParentElement( this );
    element->setBounds( m_bounds );
    
    m_prepared = false;
    m_elements.push_back( element );
    
    


 

    
    reorderElements();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::removeElement( GXElement* element )
{

    ThreadLock l( DisplayController::getController() );
    
    if (element == nullptr)
        return false;
    
    auto got = std::find (m_elements.begin() , m_elements.end() , element );

    if ( got == m_elements.end() )
        return false;

    element->m_shouldBeRemoved = true;
    

    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::removeAllElements()
{
    ThreadLock l( DisplayController::getController() );
    
    for ( auto i : m_elements )
    {
        //delete i;
        i->m_shouldBeRemoved = true;
    }
    
//    m_elements.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement* GXScene::getElementByID( int _id)
{
    ThreadLock l( DisplayController::getController() ); 
    for ( auto i : m_elements ) 
    {
        if ( i->getElementId() == _id )
            return i;
    }
    
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXScene::isAlreadyInScene( GXElement* elementTofind ) const
{
    auto iter = std::find(m_elements.begin() , m_elements.end() ,  elementTofind );
    
    return iter != m_elements.end();
}



/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::forceUpdate()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::reorderElements()
{
    std::sort(m_elements.begin() , m_elements.end(), layer_comparor());
}

/* **** **** **** **** **** **** **** **** **** **** **** */

int GXScene::findDeepestLayer() const
{
    int min = INT_MAX;
    
    for ( auto i = m_elements.begin(); i != m_elements.end(); ++i )
    {
        if ( (*i)->m_layer <= min)
            min = (*i)->m_layer;
        
    }
    
    return min;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// ALWAYS called on GUI Thread !

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void GXScene::paint( const GXRect &rect )
{
    reorderElements();
    
//    GXPath::clearRect( getBounds(), m_backgroundColor);
    
    for ( auto &i : m_elements )
    {
        updateElementInRect( i , rect );
        
    } // end for

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXScene::updateElementInRect( GXElement *element , const GXRect &rect )
{
    if (DisplayController::signalStop() )
        return;
    
    if ( !element->isPrepared() )
        element->prepare();
    
    
    
    if ( element->m_changed_flag && element->m_callChangedOnGUIThread)
    {
        element->changed();
        element->m_changed_flag = false;
        
    }
    
    
    if ( element->shouldBeRemoved() )
    {
        element->cleanUp();
        deleteElementFromGUIThread( element );
    }
    
    else if ( element->needsDisplay() )
    {
        if ( element->isOpaque() )
        {
            GXPath::clearRect( element->getBounds(), element->getBackgroundColor() );

        }
        
        // gestion alpha
        else // transparent -> we need to update each GXelement in the back
        {
            /*
            printf("\n element %i '%s' with layer %i is transparent" , element->getElementId() ,
                                                                       element->className.c_str() ,
                                                                       element->getLayer() );
            
            printf("\n needs to update in %i %i %i %i" , element->m_updateRect.origin.x ,
                                                         element->m_updateRect.origin.y,
                                                         element->m_updateRect.size.width,
                                                         element->m_updateRect.size.height
                   );
            */
            for (auto prev : m_elements)
            {
                if ( prev == element)
                    break;
                /*
                printf("\n\t so element %i '%s' with layer %i Needs display" ,
                                                prev->getElementId() ,
                                                prev->className.c_str() , prev->getLayer() );
                 */
                prev->setNeedsDisplayInRect( element->m_updateRect );
                updateElementInRect( prev, element->m_updateRect );

            }
        }
        
        element->paint( element->m_updateRect );
        element->setUpdated();
        
//        GXPath::scissorRect( element->getBounds() );

    }
    
    
}

void GXScene::prepareRessources()
{

    for ( auto i  : m_elements )
    {
        if ( !(i->isPrepared() ) )
            i->prepare();
        
    }
}

void GXScene::deleteRessources()
{
    for ( auto i  : m_elements )
    {
        i->cleanUp();
        delete i;
    }
    
    m_elements.clear();
}

void GXScene::changed()
{
    reorderElements();

}

void GXScene::deleteElementFromGUIThread(GXElement *element)
{
    if (element == nullptr)
        return;
    
    auto got = std::find (m_elements.begin() , m_elements.end() , element );
    
    if ( got == m_elements.end() )
        return;
    
    delete element;
    
    m_elements.erase( got );

}

void GXScene::childRemoved( GXElement *element )
{
    if (element == nullptr)
        return;
    
    auto got = std::find (m_elements.begin() , m_elements.end() , element );
    
    if ( got == m_elements.end() )
        return;
    
    
    m_elements.erase( got );
}

