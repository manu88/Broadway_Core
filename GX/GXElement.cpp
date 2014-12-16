//
//  GXElement.cpp
//  
//
//  Created by Manuel Deneu on 31/07/14.
//
//

#include <assert.h>


#include "GXElement.h"
#include "GXPath.h"




/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement::GXElement() : 
    m_layer                    ( 0       ),
    m_prepared                 ( false   ),
    m_shouldBeRemoved          ( false   ),
    m_added                    ( false   ),
    m_needsDisplay             ( false   ),
    m_updateRect               ( makeRectNULL() ),
    m_hidden                   ( false   ),
    m_layerChanged             ( false   ),
    m_changed_flag             ( false   ),
    m_callChangedOnGUIThread   ( true    ),
    m_parentElement            ( nullptr ),
    m_bounds                   ( makeRectNULL()       ),
    m_backgroundColor          ( makeColor( 0, 0, 0 ) ),
    m_isTransparent            ( false   ),
    m_autoRendering            ( false   )

{

}

/* **** **** **** **** **** **** **** **** **** **** **** */

GXElement::~GXElement()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::performChangedSignalOnGUIThread( bool callOnGUIThread )
{
    m_callChangedOnGUIThread  = callOnGUIThread;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::prepare()
{

    prepareRessources();
    
    m_updateRect = m_bounds;
    m_prepared = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::cleanUp()
{

    deleteRessources();

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setLayer(int layer)
{
    if ( m_layer != layer )
    {
        m_layer = layer;
        m_layerChanged = true;
        
        elementChanged();
    }
}

int GXElement::getLayer() const
{
    return m_layer;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setBounds(int x, int y , int width , int height)
{
    setBounds( makeRect( x , y , width , height ) ); // call ElementChanged!
}

void GXElement::setBounds( const GXRect &bounds )
{
    m_bounds = bounds;

    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::moveTo( const GXPoint &point )
{
    m_bounds.origin = point;
    
    elementChanged();
}

void GXElement::moveOf( int dX , int dY)
{
    moveTo( makePoint( m_bounds.origin.x + dX, m_bounds.origin.y + dY)); // call ElementChanged!
}

void GXElement::moveTo( int x , int y)
{
    moveTo( makePoint(x, y)); // call ElementChanged!
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setSize( int width , int height)
{
    setSize( makeSize( width, height ) ); // call ElementChanged!
}

void GXElement::setSize( const GXSize &size)
{
    m_bounds.size = size;
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setTransparency( bool transparent)
{
    if (m_isTransparent != transparent )
    {
        m_isTransparent = transparent;
        
        elementChanged();
    }
}

void GXElement::setOpacity( bool opaque )
{
    setTransparency( !opaque ); // call ElementChanged!
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setHidden(bool hidden)
{
    if ( hidden != m_hidden )
    {
        m_hidden = hidden;
        
        elementChanged();
    }
}

void GXElement::setVisible(bool visible)
{
    setHidden( !visible ); // call ElementChanged!
}

bool GXElement::flipVisibility()
{
    setHidden( !m_hidden ); // call ElementChanged!
    
    return m_hidden;
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::elementChanged()
{
    // direct call
    if ( !m_callChangedOnGUIThread )
    {
        changed();
        m_changed_flag  = false;
    }
    
    // set flag to perform call later on GUI Thread
    else
        m_changed_flag  = true;


    if (m_parentElement)
        m_parentElement->elementChanged();    
}


// base imp. does nothing. subclasses should override this
// to get a notification of removed element
void GXElement::childRemoved( GXElement *element )
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUnprepared()
{
    m_prepared = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void  GXElement::setNeedsDisplay()
{
    setNeedsDisplayInRect( m_bounds );
}

void GXElement::setNeedsDisplayFor( int numFrames)
{
    m_updateRect = m_bounds;
    m_autoRendering = false;
    
    m_needsDisplay = numFrames;
    
    if (m_parentElement)
        m_parentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setNeedsDisplayInRect( const GXRect &rect)
{
    m_updateRect = rect;
    m_needsDisplay++;
    
    if (m_parentElement)
        m_parentElement->setNeedsDisplay();
    
}
/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUpdated()
{
    if (!m_autoRendering)
    {
        m_needsDisplay--;
        
        if (m_needsDisplay<0)
        {
            m_needsDisplay = 0;
        }
    }
}

bool GXElement::needsDisplay() const
{
    if ( m_autoRendering )
        return true;
    
    return m_needsDisplay > 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXElement::startContinuousRendering()
{
    if ( m_autoRendering )
        stopContinuousRendering();
    
    m_autoRendering = true;


    m_parentElement->setNeedsDisplay();
    
    return m_autoRendering;
}

bool GXElement::stopContinuousRendering()
{
    m_autoRendering = false;
    m_needsDisplay = 0;
    
    return m_autoRendering;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setStrokeColor( GXColor color )
{
    GXPath::setStrokeColor( color );
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setFillColor  ( GXColor color )
{
    GXPath::setFillColor( color );
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setBackgroundColor( const GXColor &color)
{
    m_backgroundColor = color;
    
    GXPath::clearRect( m_bounds , m_backgroundColor);
}







