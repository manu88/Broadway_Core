//
//  GXTextRenderer.h
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXTextRenderer__
#define __Broadway_test__GXTextRenderer__

#include "GXElement.h"


class GXFont;
class GXGlyph;
class GXPath;

class GXTextRenderer : public GXElement 
{
public:
    GXTextRenderer();
    ~GXTextRenderer();
    
    /* *** */    
    
    void setFont( GXFont* const font);
    
    const GXFont* getFont() const
    {
        return m_font;
    }
    
    /* *** */
    
    void setSizeInPoints( float size);
    
    float getSizeInPoints() const
    {
        return m_size;
    }
    
    /* *** */
    
    void setTextColor( const GXColor &color);
    
    GXColor getTextColor() const
    {
        return m_textColor;
    }
    
    /* *** */    
    
    void setDisplayedText( const std::string &text);
    
    std::string getDisplayedText() const
    {
        return m_text;
    }
    
    /* *** */    
    
    void setCarretPosition( const GXPoint &point);

    GXPoint getCarretPosition() const
    {
        return m_carret;
    }

    
protected:
    float m_size;
    float m_sizeCoef;
    
    virtual void paint( const GXRect &rect );    
    virtual void prepareRessources();
    virtual void deleteRessources();
    virtual void changed();
    
    void drawCharAtPositionWithSize( const GXGlyph *glyph , const GXPoint &pos , float size );
    
    std::string    m_text;
    const GXFont*  m_font;
    GXPoint        m_carret;
    GXColor        m_textColor;
    
    GXPath         *m_textPath;
    
};
#endif /* defined(__Broadway_test__GXTextRenderer__) */
