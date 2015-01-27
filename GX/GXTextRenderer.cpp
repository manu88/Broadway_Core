//
//  GXTextRenderer.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <assert.h>

#include "../Log/Log.h"
#include "../Config.h"
#include "GXTextRenderer.h"
#include "GXFont.h"
#include "GXPath.h"
#include "../GXDataType/Curve.h"

GXTextRenderer::GXTextRenderer():
m_size      ( 10.f ),
m_text      (  ""  ),
m_font      ( nullptr ),
m_carret    ( makePointNULL() ),
m_textPath  ( nullptr ),
m_textColor ( makeColor(255, 255, 255) )
{
    m_sizeCoef = 1.0f/3000.f;
    
    m_font = GXFont::getDefault();
}

GXTextRenderer::~GXTextRenderer()
{
  //  deleteRessources();
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    These setters cause a full redraw of the text, so setUnprepared() is called in order to perform
    the prepareRessources() method before drawing.
 
 */

void GXTextRenderer::setFont( GXFont* const font)
{
    if ( m_font != font )
    {
        m_font = font;
        
        setUnprepared();
    }
}

void GXTextRenderer::setDisplayedText( const std::string &text)
{
    if ( text != m_text)
    {
        m_text = text;
        
        setUnprepared();
    }
    
}

void GXTextRenderer::setSizeInPoints( float size)
{
    if ( m_size != size )
    {
        setUnprepared();
        m_size = size;
    }
}

void GXTextRenderer::setCarretPosition( const GXPoint &point)
{
//    const int lineJump = m_font->getLineSpace()*2 * sizeCoef;
    m_carret.y = point.y ;//m_bounds.size.height - point.y - lineJump;
    m_carret.x = point.x;

    setUnprepared();
}

void GXTextRenderer::setTextColor( const GXColor &color)
{
    m_textColor = color;
    
    setUnprepared();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// accessed by GUI thread only! 

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
void GXTextRenderer::changed()
{

    setUnprepared();
}

void GXTextRenderer::paint( const GXRect &rect )
{
    if (m_font == nullptr)
        return;
    
    GXPath::setFillColor( m_textColor );
    m_textPath->fill();

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::drawCharAtPositionWithSize( const GXGlyph *glyph , const GXPoint &pos , float size )
{
    for (int i = 0 ; i < ( int ) glyph->getNumSegments() ; i++)
    {
        const Curve_float *c = glyph->getCurveAt( i );
        
        if ( c->getCurveType() == CurveMoveTo )
        {
            m_textPath->moveToPoint( pos + makePoint( (int) c->getPointAt(0).getX()*size ,
                                                      (int) c->getPointAt(0).getY()*size)
                             );
        }
        
        else if ( c->getCurveType() == CurveLineTo )
        {
            m_textPath->addLineToPoint( pos + makePoint( (int) c->getPointAt(0).getX()*size ,
                                                         (int) c->getPointAt(0).getY()*size)
                                );
        }
        else if ( c->getCurveType() == CurveCubicTo )
        {
            m_textPath->addCubicCurveToPoint( pos +   makePoint( (int) c->getPointAt(0).getX()*size , (int) c->getPointAt(0).getY()*size),
                                              pos +   makePoint( (int) c->getPointAt(1).getX()*size , (int) c->getPointAt(1).getY()*size),
                                              pos +   makePoint( (int) c->getPointAt(2).getX()*size , (int) c->getPointAt(2).getY()*size)
                                );
        }
        else if ( c->getCurveType() == CurveQuadTo )
        {
            m_textPath->addQuadCurveToPoint( pos +makePoint( (int) c->getPointAt(0).getX()*size , (int) c->getPointAt(0).getY()*size ),
                                             pos +makePoint( (int) c->getPointAt(1).getX()*size , (int) c->getPointAt(1).getY()*size)
                               );
        }
        else
            DEBUG_ASSERT( false );
        
    }
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::prepareRessources()
{
    const float realSize = m_size* m_sizeCoef;
    if ( m_font == nullptr )
    {
        Log::log("\n Warning, no font set to display text ...");
        return ;
    }

    if ( m_textPath == nullptr)
        m_textPath = new GXPath( getBounds() );

    m_textPath->clear();
    
    const int lineJump = m_font->getLineSpace()*2 * realSize ;
    
    
    m_textPath->setStrokeColor( m_textColor );
    m_textPath->setFillColor  ( m_textColor );
    
    GXPoint pt = m_carret;
    
    for (unsigned i=0; i<m_text.length(); ++i)
    {
        const char c =m_text.at(i);
        
        if ( c == '\n')
        {
            pt.x = m_carret.x;
            pt.y -= lineJump;
        }
        
        else
        {
            const GXGlyph *glyph =  m_font->getCharacter( c );
            
            drawCharAtPositionWithSize( glyph, pt , realSize );
            
            pt.x += (int ) glyph->getAdvanceX() * realSize;
            
            if (pt.x >= getBounds().size.width)
            {
                pt.x = m_carret.x;
                pt.y -= lineJump;
            }
        }
    }
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXTextRenderer::deleteRessources()
{
    if (m_textPath != nullptr)
        delete m_textPath;
    
    m_textPath = nullptr;
}



