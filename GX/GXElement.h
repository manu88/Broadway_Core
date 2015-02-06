/*!
 *  \brief     GXElement
 *  \details   Base class for all Graphic Elements
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */


#ifndef ____GXElement__
#define ____GXElement__



#include "../Internal/Element.h"

#include "../GXDataType/GXGeometry.h"
#include "../GXDataType/GXColors.h"

class GXAnimation; // forward

class GXElement : public virtual Element
{
public:
    
    friend class DisplayController;
    friend class GXScene;
    
    // default is set To true
    void performChangedSignalOnGUIThread( bool callOnGUIThread );

    void prepare();
    void cleanUp();
    
    // signal that this GXElement has to be redrawn
    void setNeedsDisplay();
    void setNeedsDisplayFor( int numFrames);
    void setNeedsDisplayInRect( const GXRect &rect);
    
    void setUpdated();
    
    bool startContinuousRendering();
    bool stopContinuousRendering();
    
    bool attachAnimation( GXAnimation *anim);
    bool detachAnimation();
    
    // 
    
    void removeFromParent() noexcept
    {
        _shouldBeRemoved = true;
    }
    
    bool shouldBeRemoved() const noexcept
    {
        return _shouldBeRemoved;
    }
    
    bool isPrepared() const noexcept
    {
        return _prepared;
    }
    
    void setUnprepared();
    
    bool needsDisplay() const;
    
    // 1 atop 0
    void setLayer(int layer);
    
    int getLayer() const;
    
    /* **** **** **** **** **** **** **** **** **** */

    /* These setters will call changed() later*/
    void setBounds( int x, int y , int width , int height);
    void setBounds( const GXRect &bounds );
    
    void moveTo( const GXPoint &point );
    void moveOf( int dX , int dY);
    void moveTo( int x  , int y);
    
    void setSize( int width , int height);
    void setSize( const GXSize &size);

    GXRect getBounds() const noexcept
    {
        return _bounds;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    
    void setBackgroundColor( const GXColor &color);
    
    GXColor getBackgroundColor() const noexcept
    {
        return _backgroundColor;
    }

    /* **** **** **** **** **** **** **** **** **** */

    // will call changed()
    void setTransparency( bool transparent );
    void setOpacity( bool opaque );
    
    bool isTransparent() const noexcept
    {
        return _isTransparent;
    }

    bool isOpaque() const noexcept
    {
        return !isTransparent();
    }
    
    // will call changed()
    void setHidden(  bool hidden);
    void setVisible( bool visible);
    bool flipVisibility();
    
    bool isVisible() const noexcept
    {
        return !_hidden;
    }
    

    
    /* **** **** **** **** **** **** **** **** **** */
    
    void setParentElement( GXElement *parentElement)
    {
        _parentElement = parentElement;
        
        _added = true;
    }
    
    GXElement* getParentElement() const
    {
        return _parentElement;
    }
    
    
protected:
    
    // derived only.
    GXElement();
    virtual ~GXElement();
    
    // called _ONLY_ from GUI thread
    virtual void paint( const GXRect &rect, GXAnimation* anim ) = 0;
    virtual void prepareRessources() = 0;
    virtual void deleteRessources() = 0;
    
    // can be called synchronusly on caller's thread,
    // or on GUI thread if performChangedSignalOnGUIThread( true ) is called.
    virtual void changed() = 0;
    
    virtual void childRemoved(GXElement *element);
    
    void setStrokeColor( GXColor color );
    void setFillColor  ( GXColor color );
    
    

    
    

    


private:
    
    void elementChanged();
    
    int             _layer;
    
    bool            _prepared;
    bool            _shouldBeRemoved;
    bool            _added;
    
    int             _needsDisplay;
    GXRect          _updateRect;
    
    bool            _hidden;
    bool            _layerChanged;
    
    bool            m_changed_flag;
    bool            m_callChangedOnGUIThread;
    
    GXElement       *_parentElement;
    
    GXRect          _bounds;
    
    GXColor         _backgroundColor;

    // default to false             
    bool            _isTransparent;
    
    bool            _autoRendering;
    
    GXAnimation    *_anim;

};


#endif /* defined(____GXElement__) */
