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
    
    bool startContinuousRendering();
    bool stopContinuousRendering();
    
    // 
    
    void removeFromParent()
    {
        m_shouldBeRemoved = true;
    }
    
    bool shouldBeRemoved() const
    {
        return m_shouldBeRemoved;
    }
    
    bool isPrepared() const
    {
        return m_prepared;
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

    GXRect getBounds() const
    {
        return m_bounds;
    }
    
    /* **** **** **** **** **** **** **** **** **** */
    
    void setBackgroundColor( const GXColor &color);
    
    GXColor getBackgroundColor() const noexcept
    {
        return m_backgroundColor;
    }

    /* **** **** **** **** **** **** **** **** **** */

    // will call changed()
    void setTransparency( bool transparent );
    void setOpacity( bool opaque );
    
    bool isTransparent() const noexcept
    {
        return m_isTransparent;
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
        return !m_hidden;
    }
    

    
    /* **** **** **** **** **** **** **** **** **** */
    
    void setParentElement( GXElement *parentElement)
    {
        m_parentElement = parentElement;
        m_added = true;
    }
    
    GXElement* getParentElement() const
    {
        return m_parentElement;
    }
    
    
protected:
    
    // derived only.
    GXElement();
    virtual ~GXElement();
    
    // called _ONLY_ from GUI thread
    virtual void paint( const GXRect &rect ) = 0;
    virtual void prepareRessources() = 0;
    virtual void deleteRessources() = 0;
    
    // can be called synchronusly on caller's thread,
    // or on GUI thread if performChangedSignalOnGUIThread( true ) is called.
    virtual void changed() = 0;
    
    virtual void childRemoved(GXElement *element);
    
    void setStrokeColor( GXColor color );
    void setFillColor  ( GXColor color );
    
    
    void setUpdated();
    
    

    


private:
    
    void elementChanged();
    
    int             m_layer;
    
    bool            m_prepared;
    bool            m_shouldBeRemoved;
    bool            m_added;
    
    int             m_needsDisplay;
    GXRect          m_updateRect;
    
    bool            m_hidden;
    bool            m_layerChanged;
    
    bool            m_changed_flag;
    bool            m_callChangedOnGUIThread;
    
    GXElement       *m_parentElement;
    
    GXRect          m_bounds;
    
    GXColor         m_backgroundColor;

    // default to false             
    bool            m_isTransparent;
    
    bool  m_autoRendering;
    




};

//typedef GXElement* create_element();

#endif /* defined(____GXElement__) */
