//
//  Painter.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 08/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <assert.h>

#include "Painter.h"




Painter::Painter( PainterType type ) :
m_type   ( type  )
{
    className = "Painter";
}

Painter::~Painter()
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterRect::PainterRect( const GXRect &rect):
Painter( PainterType_Rect ),
m_rect( rect )
{

}

PainterRect::~PainterRect()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterRoundedRect::PainterRoundedRect( const GXRect &rect , float arcW , float arcH):
Painter( PainterType_RoundedRect ),
m_rect( rect ),
m_arcW( arcW ),
m_arcH( arcH )
{
    
}

PainterRoundedRect::~PainterRoundedRect()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterEllipse::PainterEllipse( const GXRect &rect):
Painter( PainterType_Ellipse ),
m_rect( rect )
{
    
}

PainterEllipse::~PainterEllipse()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterArc::PainterArc( const GXPoint &center ,   const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type ):
Painter ( PainterType_Arc ),

m_center      ( center ),
m_size        ( size),
m_startAngle  ( startAngle ),
m_angleExtent ( angleExtent),
m_type        ( type)
{
    
}

PainterArc::~PainterArc()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterLine::PainterLine( const GXLine &line) :
Painter( PainterType_Line ),
m_line( line )
{
    
}

PainterLine::~PainterLine()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterMoveTo::PainterMoveTo( const GXPoint &point) :
Painter( PainterType_MoveTo ),
m_point( point )
{
    
}

PainterMoveTo::~PainterMoveTo()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterLineTo::PainterLineTo( const GXPoint &point) :
Painter( PainterType_LineTo ),
m_point( point )
{
    
}

PainterLineTo::~PainterLineTo()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterCubicCurveTo::PainterCubicCurveTo( const GXPoint &controlPoint0 , const GXPoint &controlPoint1 , const GXPoint &endPoint ):
Painter( PainterType_CubicCurveTo ),
m_cp0       ( controlPoint0 ),
m_cp1       ( controlPoint1 ),
m_endPoint  ( endPoint     )
{
    
}

PainterCubicCurveTo::~PainterCubicCurveTo()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterQuadCurveTo::    PainterQuadCurveTo( const GXPoint &controlPoint , const GXPoint &endPoint):
Painter( PainterType_QuadCurveTo ),
m_cp       ( controlPoint ),
m_endPoint ( endPoint     )
{
    
}

PainterQuadCurveTo::~PainterQuadCurveTo()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterClose::PainterClose() :
Painter( PainterType_Close )
{
    
}

PainterClose::~PainterClose()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterStrokeColor::PainterStrokeColor( const GXColor &color ) :
Painter( PainterType_ColorStroke ),
m_color( color )
{
    
}

PainterStrokeColor::~PainterStrokeColor()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterFillColor::PainterFillColor(const GXColor &color ) :
Painter( PainterType_ColorFill ),
m_color( color )
{
    
}

PainterFillColor::~PainterFillColor()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterFill::PainterFill() :
Painter( PainterType_Fill )
{
    
}

PainterFill::~PainterFill()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterStroke::PainterStroke() :
Painter( PainterType_Stroke )
{
    
}

PainterStroke::~PainterStroke()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

PainterStrokeWidth::PainterStrokeWidth( float width  ) :
Painter( PainterType_StrokeWidth ),
m_width( width )
{
    
}

PainterStrokeWidth::~PainterStrokeWidth()
{
    
}










