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
    _layer                    ( 0       ),
    _prepared                 ( false   ),
    _shouldBeRemoved          ( false   ),
    _added                    ( false   ),
    _needsDisplay             ( false   ),
    _updateRect               ( makeRectNULL() ),
    _hidden                   ( false   ),

    _layerChanged             ( false   ),
    _changed_flag            ( false   ),
    m_callChangedOnGUIThread  ( true    ),

    _parentElement            ( nullptr ),
    _bounds                   ( makeRectNULL()       ),
    _backgroundColor          ( makeColor( 0, 0, 0 ) ),
    _isTransparent            ( false   ),
    _autoRendering            ( false   ),

    _anim                     ( nullptr )

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
    
    _updateRect = _bounds;
    _prepared = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::cleanUp()
{

    deleteRessources();

}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setLayer(int layer)
{
    if ( _layer != layer )
    {
        _layer        = layer;
        _layerChanged = true;
        
        elementChanged();
    }
}

int GXElement::getLayer() const
{
    return _layer;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setBounds(int x, int y , int width , int height)
{
    setBounds( makeRect( x , y , width , height ) ); // call ElementChanged!
}

void GXElement::setBounds( const GXRect &bounds )
{
    _bounds = bounds;

    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::moveTo( const GXPoint &point )
{
    _bounds.origin = point;
    
    elementChanged();
}

void GXElement::moveOf( int dX , int dY)
{
    moveTo( makePoint( _bounds.origin.x + dX, _bounds.origin.y + dY)); // call ElementChanged!
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
    _bounds.size = size;
    
    elementChanged();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setTransparency( bool transparent)
{
    if (_isTransparent != transparent )
    {
        _isTransparent = transparent;
        
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
    if ( hidden != _hidden )
    {
        _hidden = hidden;
        
        elementChanged();
    }
}

void GXElement::setVisible(bool visible)
{
    setHidden( !visible ); // call ElementChanged!
}

bool GXElement::flipVisibility()
{
    setHidden( !_hidden ); // call ElementChanged!
    
    return _hidden;
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::elementChanged()
{
    // direct call
    if ( !m_callChangedOnGUIThread )
    {
        changed();
        _changed_flag  = false;
    }
    
    // set flag to perform call later on GUI Thread
    else
        _changed_flag  = true;


    if (_parentElement)
        _parentElement->elementChanged();
}


// base imp. does nothing. subclasses should override this
// to get a notification of removed element
void GXElement::childRemoved( GXElement *element )
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUnprepared()
{
    _prepared = false;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void  GXElement::setNeedsDisplay()
{
    setNeedsDisplayInRect( _bounds );
}

void GXElement::setNeedsDisplayFor( int numFrames)
{
    _updateRect = _bounds;
    _autoRendering = false;
    
    _needsDisplay = numFrames;
    
    if (_parentElement)
        _parentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setNeedsDisplayInRect( const GXRect &rect)
{
    _updateRect = rect;
    _needsDisplay++;
    
    if (_parentElement)
        _parentElement->setNeedsDisplay();
    
}
/* **** **** **** **** **** **** **** **** **** **** **** */

void GXElement::setUpdated()
{
    if (!_autoRendering)
    {
        _needsDisplay--;
        
        if (_needsDisplay<0)
        {
            _needsDisplay = 0;
        }
    }
}

bool GXElement::needsDisplay() const
{
    if ( _autoRendering )
        return true;
    
    return _needsDisplay > 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXElement::startContinuousRendering()
{
    if ( _autoRendering )
        stopContinuousRendering();
    
    _autoRendering = true;


    _parentElement->setNeedsDisplay();
    
    return _autoRendering;
}

bool GXElement::stopContinuousRendering()
{
    _autoRendering = false;
    
    _needsDisplay = 0;
    
    return _autoRendering;
}

/* **** **** **** **** **** **** **** **** **** **** **** */

bool GXElement::attachAnimation( GXAnimation *anim)
{
    if (_anim)
    {
        printf("\n already has anim, should remove previous one");
    }
    
    _anim = anim;
    
    return true;
}

bool GXElement::detachAnimation()
{
    printf("\n  should remove previous anim");
    
    return true;
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
    _backgroundColor = color;
    
    GXPath::clearRect( _bounds , _backgroundColor);
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXElement::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments ,bool *validCall /*= nullptr*/)
{
    if( validCall )
        *validCall = true;
    /* Setters */
    
    if ( selector == "setNeedsDisplay")
    {
        setNeedsDisplay();
        return Variant();
    }
    
    else if ( selector == "startContinuousRendering")
    {
        return startContinuousRendering();
    }
    
    else if ( selector == "stopContinuousRendering")
    {
        return stopContinuousRendering();
    }
    
    else if ( selector == "setLayer")
    {
        setLayer( arguments.at(0).getInt() );
        return Variant();
    }
    else if ( selector == "setBounds" )
    {
        setBounds( arguments.at(0).getInt(),
                   arguments.at(1).getInt(),
                   arguments.at(2).getInt(),
                   arguments.at(3).getInt()
                  );
        return Variant();
    }
    else if ( selector == "moveOf" )
    {
        moveOf(arguments.at(0).getInt(), arguments.at(1).getInt() );
        return Variant();
    }
    else if ( selector == "moveTo" )
    {
        moveTo(arguments.at(0).getInt(), arguments.at(1).getInt() );
        return Variant();
    }
    
    else if ( selector == "flipVisibility")
        return flipVisibility();
    
    else if ( selector == "setVisible")
    {
        setVisible( arguments.at(0).getBool() );
        return Variant();
    }
    
    else if ( selector == "setBackgroundColor")
    {
        const GXColor col = makeColor( (uint8_t) arguments.at(0).getInt(),
                                       (uint8_t) arguments.at(1).getInt(),
                                       (uint8_t) arguments.at(2).getInt(),
                                       (uint8_t) arguments.at(3).getInt()
                                      );
        setBackgroundColor( col );
        return Variant();
    }

    else if ( selector == "setTransparency")
    {
        setTransparency( arguments.at(0).getBool() );
        return Variant();
    }
    
    /* Getters */
    
    else if ( selector == "isVisible" )
        return isVisible();
    
    else if ( selector == "needsDisplay" )
        return needsDisplay();
    
    
    else if ( selector == "getBounds" )
        return Variant{ getBounds().origin.x ,getBounds().origin.y , getBounds().size.width , getBounds().size.height  };

    else if ( selector == "getBackgroundColor")
        return Variant{ getBackgroundColor().r ,getBackgroundColor().g , getBackgroundColor().b ,getBackgroundColor().a  };
    
    else if ( selector == "isOpaque")
        return isOpaque();
    /*
     void setHidden(  bool hidden);
     void setSize( int width , int height);
     void setOpacity( bool opaque );
     */
    
    if( validCall )
        *validCall = false;
    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}

#endif /*ENABLE_ELEMENT_SELECTOR*/





