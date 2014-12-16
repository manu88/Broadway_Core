//
//  GXPaintJS.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 08/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <assert.h>

#include "GXPaintJS.h"
#include "GXPath.h"

#include "../Config.h"
#include "../Internal/ThreadLock.h"
#include "DisplayController.h"





GXPaintJS::GXPaintJS() 
{
    className = "GXPaintJS";
}

GXPaintJS::~GXPaintJS()
{
    clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::setFillColor( const GXColor &fillColor)
{
    addPainter( new PainterFillColor( fillColor) );
}

void GXPaintJS::setStrokeColor( const GXColor &strokeColor)
{
    addPainter( new PainterStrokeColor( strokeColor) );
}

void GXPaintJS::setStrokeWidth( float width )
{
    addPainter( new PainterStrokeWidth( width ) );
}

void GXPaintJS::setCapStyle ( GXCapStyle style )
{
    addPainter( new PainterCapStyle( style ) );
}

void GXPaintJS::setJoinStyle( GXJoinStyle style )
{
    addPainter( new PainterJoinStyle( style ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addRect( const GXRect &rect )
{
    addPainter( new PainterRect( rect ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addRoundedRect( const GXRect &rect, float arcW , float arcH )
{
    addPainter( new PainterRoundedRect( rect , arcW , arcH ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addEllipse( const GXRect &rect )
{
    addPainter( new PainterEllipse( rect ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addArc( const GXPoint &center , const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type )
{
    addPainter( new PainterArc( center , size , startAngle , angleExtent , type ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addLine( const GXLine &line  )
{
    addPainter( new PainterLine( line ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */



void GXPaintJS::moveToPoint( const GXPoint &point)
{
    addPainter( new PainterMoveTo( point ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addLineToPoint( const GXPoint &point)
{
    addPainter( new PainterLineTo( point ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addCubicCurveToPoint( const GXPoint &controlPoint0 , const GXPoint &controlPoint1 , const GXPoint &endPoint )
{
    addPainter( new PainterCubicCurveTo( controlPoint0 , controlPoint1 , endPoint  ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addQuadCurveToPoint( const GXPoint &controlPoint , const GXPoint &endPoint  )
{
    addPainter( new PainterQuadCurveTo( controlPoint , endPoint ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::close()
{
    addPainter( new PainterClose() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::stroke()
{
    addPainter( new PainterStroke() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::fill()
{
    addPainter( new PainterFill() );    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::addPainter( Painter *painter  )
{
        m_paintOperations.push_back( painter );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::clear()
{

    for( auto i : m_paintOperations )
    {
        if ( i )
        {
            delete ( i );
        }
    }
    m_paintOperations.clear();
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    The following methods _MUST_ be called on GUI thread
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPaintJS::changed()
{
    
}

void GXPaintJS::prepareRessources()
{
    
}

void GXPaintJS::deleteRessources()
{
    
}



void GXPaintJS::paint( const GXRect &rect )
{
    
    GXPath path( getBounds() );
    
    path.shouldClearAfterRenderOps( true );

    for (auto i : m_paintOperations )
    {
        PainterType type = i->getType();
        
        switch ( type )
        {
            
            case PainterType_Rect:
            {
                const PainterRect* p = dynamic_cast<PainterRect*>( i );
                path.addRect( p->getRect()  );
                
            }
                break;
            
            case PainterType_RoundedRect:
            {
                const PainterRoundedRect* p = dynamic_cast<PainterRoundedRect*>( i );
                path.addRoundedRect( p->getRect()  , p->getArcW() , p->getArcH() );
                
            }
                break;
                
            case PainterType_Line:
            {
                const PainterLine* p = dynamic_cast<PainterLine*>( i );
                path.addLine( p->getLine() );
                

            }
                break;
                
            case PainterType_Ellipse:
            {
                const PainterEllipse* p = dynamic_cast<PainterEllipse*>( i );
                path.addEllipse( p->getRect() );
                

            }
                break;
                
            case PainterType_Arc:
            {
                const PainterArc* p = dynamic_cast<PainterArc*>( i );
                path.addArc( p->getCenter() , p->getSize() , p->getStartAngle(), p->getAngleExtent(), p->getType() );
                
                
            }
                break;
                
            case PainterType_MoveTo:
            {
                const PainterMoveTo* p = dynamic_cast<PainterMoveTo*>( i );
                path.moveToPoint( p->getPoint()  );
                

            }
                break;
                
            case PainterType_LineTo:
            {
                const PainterLineTo* p = dynamic_cast<PainterLineTo*>( i );
                path.addLineToPoint( p->getPoint()  );
                

            }
                break;
                
            case PainterType_CubicCurveTo:
            {
                const PainterCubicCurveTo* p = dynamic_cast<PainterCubicCurveTo*>( i );
                path.addCubicCurveToPoint( p->getCp0() ,
                                           p->getCp1() ,
                                           p->getEndPoint()
                                          );
                
                
            }
                break;
                
            case PainterType_QuadCurveTo:
            {
                const PainterQuadCurveTo* p = dynamic_cast<PainterQuadCurveTo*>( i );
                
                path.addQuadCurveToPoint( p->getCp() ,
                                          p->getEndPoint()
                                         );
                
                break;
            }
                
            case PainterType_Close:
            {
                path.closePath();

            }
                break;
                
            case PainterType_ColorFill:
            {
                PainterFillColor* p = dynamic_cast<PainterFillColor*>( i );
                path.setFillColor( p->m_color);


            }
                break;
                
            case PainterType_ColorStroke:
            {
                PainterStrokeColor* p = dynamic_cast<PainterStrokeColor*>( i );
                path.setStrokeColor( p->m_color);


            }
                break;
                
            case PainterType_StrokeWidth:
            {
                PainterStrokeWidth* p = dynamic_cast< PainterStrokeWidth* >( i );
                path.setStrokeWidth( p->getWidth() );
                
                
            }
                break;
                
            case PainterType_CapStyle:
            {
                PainterCapStyle* p = dynamic_cast< PainterCapStyle* >( i );
                path.setCapStyle( p->style );
                
                
            }
                break;
                
            case PainterType_JoinStyle:
            {
                PainterJoinStyle* p = dynamic_cast< PainterJoinStyle* >( i );
                path.setJoinStyle( p->style );
                
                
            }
                break;
                
            case PainterType_Fill:

                path.fill();


                break;
                
            case PainterType_Stroke:

                path.stroke();

                break;

            default:
                /* Each case should have been covered ... */
                DEBUG_ASSERT( false );
                break;
        }

    }
    

    
}

