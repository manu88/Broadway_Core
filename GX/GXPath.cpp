//
//  GXPath.cpp
//  
//
//  Created by Manuel Deneu on 07/08/14.
//
//




#include <assert.h>
#include "GXPath.h"
#include "../Config.h"

#include <math.h>



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXPath::GXPath( const GXRect &bounds ) :
    m_shouldClearAfterRender ( false ),
    m_rendercount  ( 0 ),
    m_currentPoint ( makePointNULL() ),
    m_boundingRect ( bounds )
{
    className = "GXPath";
    
#ifdef TARGET_RASPBERRY_PI
    m_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    
#endif
    
    // reset to defaults ?
    setStrokeWidth ( 1. );
    setJoinStyle   ( Join_Miter );
    setCapStyle    ( Cap_Butt );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXPath::~GXPath()
{
#ifdef TARGET_RASPBERRY_PI
    vgDestroyPath( m_path);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::clear()
{
#ifdef TARGET_RASPBERRY_PI
    vgClearPath( m_path , VG_PATH_CAPABILITY_ALL);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */


/*static*/ void GXPath::setStrokeColor( const GXColor &color )
{

    VGPaint painter = vgCreatePaint();
    
    GXColorFLOAT clr = GXColorToGXColorFLoat( color );
    
    const VGfloat _color[4] { clr.r , clr.g , clr.b , clr.a};
    
    vgSetParameteri( painter, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv( painter, VG_PAINT_COLOR, 4, _color);
    
    vgSetPaint( painter, VG_STROKE_PATH);
    
    vgDestroyPaint(painter);
    

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setFillColor  (const GXColor &color )
{
#ifdef TARGET_RASPBERRY_PI
    VGPaint painter = vgCreatePaint();
    
    GXColorFLOAT clr = GXColorToGXColorFLoat( color );
    
    VGfloat _color[4];
    _color[0] = clr.r ; // r
    _color[1] = clr.g ; // g
    _color[2] = clr.b ; // b
    _color[3] = clr.a ;
    
    vgSetParameteri( painter, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetParameterfv( painter, VG_PAINT_COLOR, 4, _color);
    
    vgSetPaint( painter, VG_FILL_PATH);
    
    vgDestroyPaint(painter);
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setFillGradient (const GXColorGradient &color )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setStrokeWidth( float width )
{
#ifdef TARGET_RASPBERRY_PI   
    
	vgSetf( VG_STROKE_LINE_WIDTH, (VGfloat) width);


    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setCapStyle ( GXCapStyle style )
{
    vgSeti( VG_STROKE_CAP_STYLE,  GXCapStyleToVG( style) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setJoinStyle( GXJoinStyle style )
{
    vgSeti( VG_STROKE_JOIN_STYLE, GXJoinStyleToVG( style ) );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setFillRule( GXFillRule rule )
{
#ifdef TARGET_RASPBERRY_PI
    
    if ( rule == Fill_EvenOdd)
        vgSeti( VG_FILL_RULE,   VG_EVEN_ODD );
    
    else if ( rule == Fill_NonZero)
        vgSeti( VG_FILL_RULE,   VG_NON_ZERO );
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::setFillPainterType( GXPaintType type )
{
#ifdef TARGET_RASPBERRY_PI
    if (type == Paint_color)
    {
        
    }
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::scissorRect( const GXRect &rect)
{
#ifdef TARGET_RASPBERRY_PI        
    vgSeti(VG_SCISSORING, VG_TRUE );
    VGint coords[4] = { rect.origin.x , rect.origin.y , rect.size.width , rect.size.height };

    vgSetiv( VG_SCISSOR_RECTS, 4, coords );
    
#endif    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXPath::clearRect( const GXRect &rect , const GXColor &color )
{
#ifdef TARGET_RASPBERRY_PI
    const GXColorFLOAT c = GXColorToGXColorFLoat( color);
    const VGfloat col[4] = { c.r, c.g , c.b, c.a };
    
    vgSeti( VG_SCISSORING, VG_FALSE );
    vgSetfv( VG_CLEAR_COLOR, 4, col );
    
    vgClear( rect.origin.x , rect.origin.y , rect.size.width , rect.size.height );
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::appendPath( const GXPath &otherPath)
{
    changed();
    
#ifdef TARGET_RASPBERRY_PI
    vgAppendPath(m_path /* dest*/ , otherPath.m_path /* scr*/);
#endif
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Basic shapes
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
void GXPath::addRect( const GXRect &rect)
{
    changed();
#ifdef TARGET_RASPBERRY_PI
    
    /*VGUErrorCode ret =*/ vguRect(
                                   m_path,
                                   rect.origin.x + m_boundingRect.origin.x ,
                                   rect.origin.y + m_boundingRect.origin.y ,
                                   rect.size.width,
                                   rect.size.height
                                   );
 
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addRoundedRect( const GXRect &rect , float arcW , float arcH)
{
    changed();
#ifdef TARGET_RASPBERRY_PI
    VGUErrorCode ret = vguRoundRect( m_path,
                                     rect.origin.x + m_boundingRect.origin.x,
                                     rect.origin.y + m_boundingRect.origin.y,
                                     rect.size.width,
                                     rect.size.height ,
                                     arcW ,
                                     arcH);
    
    DEBUG_ASSERT( ret == VGU_NO_ERROR);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addEllipse( const GXRect &boundingRect)
{
    changed();
    
    GXPoint center = makePoint( boundingRect.origin.x + m_boundingRect.origin.x + (boundingRect.size.width/2),
                                boundingRect.origin.y + m_boundingRect.origin.y + (boundingRect.size.height/2)
                               );
    
#ifdef TARGET_RASPBERRY_PI
    VGUErrorCode ret = vguEllipse ( m_path , center.x , center.y , boundingRect.size.width , boundingRect.size.height );
    
    DEBUG_ASSERT( ret == VGU_NO_ERROR);
#endif    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addArc( const GXPoint &center ,  const GXSize &size , float startAngle , float angleExtent ,GXArcType type )
{
    changed();
    
    const float a1 = startAngle*360.f / (M_PI*2.f);
    const float a2 = angleExtent*360.f / (M_PI*2.f);
#ifdef TARGET_RASPBERRY_PI
    
    VGUArcType arcType = VGU_ARC_OPEN;
    
    if ( type == Arc_Chord )
        arcType = VGU_ARC_CHORD;
    
    else if ( type == Arc_Pie )
        arcType = VGU_ARC_PIE;
        
    
    VGUErrorCode ret = vguArc ( m_path ,
                                center.x + m_boundingRect.origin.x ,
                                center.y + m_boundingRect.origin.y,
                                size.width ,
                                size.height ,
                                a1 ,
                                a2 ,
                                arcType
                               );
    
    DEBUG_ASSERT( ret == VGU_NO_ERROR);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addLine( const GXPoint &p0 , const GXPoint &p1 )
{
    changed();
    
#ifdef TARGET_RASPBERRY_PI
    VGUErrorCode ret = vguLine( m_path ,
                                p0.x + m_boundingRect.origin.x ,
                                p0.y + m_boundingRect.origin.y ,
                                p1.x + m_boundingRect.origin.x ,
                                p1.y + m_boundingRect.origin.y
                               );
    
    DEBUG_ASSERT( ret == VGU_NO_ERROR);
#endif
}

void GXPath::addLine( GXLine line )
{    
    addLine(line.p0, line.p1);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Curves
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::moveToPoint( const GXPoint &point)
{
    changed();
    
#ifdef TARGET_RASPBERRY_PI
    VGubyte segments[] = {  VG_MOVE_TO };
    
	VGfloat coords[] = { (VGfloat) point.x + m_boundingRect.origin.x , (VGfloat) point.y+ m_boundingRect.origin.y };
    
    vgAppendPathData( m_path, 1, segments, coords);
#endif
    
    m_currentPoint = point;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addLineToPoint     ( const GXPoint &point)
{
    changed();
    
#ifdef TARGET_RASPBERRY_PI    
    VGubyte segments[] = {  VG_LINE_TO };
    
	VGfloat coords[] = { (VGfloat) point.x + m_boundingRect.origin.x , (VGfloat) point.y + m_boundingRect.origin.y };
    
    vgAppendPathData( m_path, 1, segments, coords);
#endif
    m_currentPoint = point;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addCubicCurveToPoint( const GXPoint &controlPoint0, const GXPoint &controlPoint1 , const GXPoint &endPoint )
{
    changed();
    // s beginning of current subpath;
    // o last point
    // p control point
    
#ifdef TARGET_RASPBERRY_PI
    VGubyte segments[] = {  VG_CUBIC_TO };
	VGfloat coords[] = { (VGfloat) controlPoint0.x + m_boundingRect.origin.x , (VGfloat) controlPoint0.y  + m_boundingRect.origin.y,
                         (VGfloat) controlPoint1.x + m_boundingRect.origin.x , (VGfloat) controlPoint1.y  + m_boundingRect.origin.y ,
                         (VGfloat) endPoint.x + m_boundingRect.origin.x      , (VGfloat) endPoint.y + m_boundingRect.origin.y ,
                        };
    
    vgAppendPathData(m_path, 1, segments, coords);

#endif
    
    m_currentPoint = endPoint + m_boundingRect.origin;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addQuadCurveToPoint( const GXPoint &controlPoint , const GXPoint &endPoint)
{
    changed();
    
#ifdef TARGET_RASPBERRY_PI
    VGubyte segments[] = {  VG_QUAD_TO };
    
	VGfloat coords[] = { (VGfloat) controlPoint.x + m_boundingRect.origin.x , (VGfloat) controlPoint.y + m_boundingRect.origin.y,
                         (VGfloat) endPoint.x  + m_boundingRect.origin.x    , (VGfloat) endPoint.y  + m_boundingRect.origin.y,
                       };
    
    vgAppendPathData(m_path, 1, segments, coords);
    
#endif
    
    m_currentPoint = endPoint + m_boundingRect.origin;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::addCurve(const Curve_float &curve)
{
    changed();
    
    const int size = (int) curve.getNumPoints();
    
#ifdef TARGET_RASPBERRY_PI
    
    VGPath path = vgCreatePath( VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_S_32,1.0f / 65536.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
    VGubyte segments[size];
    
	VGfloat coords[size*2];/* = { (VGfloat) controlPoint.x , (VGfloat) controlPoint.y ,
        (VGfloat) endPoint.x      , (VGfloat) endPoint.y ,
    };*/
    
    for (int i = 0 ; i<size ; i++ )
    {
        Curve_float::CurvePoint pt = curve.getPointAt(i);

        segments[i] = VG_LINE_TO;

        coords[i*2] =  (VGfloat) pt.getX()/10.f     + m_boundingRect.origin.x;
        coords[(i*2)+1] =(VGfloat)  pt.getY() /10.f + m_boundingRect.origin.y;

    }
    
    vgAppendPathData(path, size, segments, coords);
    
    vgAppendPath( m_path , path);
    vgDestroyPath( path );
    
#endif
    
//    m_currentPoint = endPoint;
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::closePath()
{
#ifdef TARGET_RASPBERRY_PI
    VGubyte segments[] = {  VG_CLOSE_PATH };
    
	VGfloat coords[] = {};
    
    vgAppendPathData(m_path, 1, segments, coords);
    
#endif

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Draw & render
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::stroke()
{
    m_rendercount++;
    
#ifdef TARGET_RASPBERRY_PI
    vgDrawPath( m_path, VG_STROKE_PATH);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::fill()
{
    m_rendercount++;
    
#ifdef TARGET_RASPBERRY_PI
    vgDrawPath( m_path,  VG_FILL_PATH );
#endif
    

}

void GXPath::clearRect( const GXColor &color)
{
#ifdef TARGET_RASPBERRY_PI
    const GXColorFLOAT c = GXColorToGXColorFLoat( color);
    const VGfloat col[4] = { c.r, c.g , c.b, c.a };

    vgSeti( VG_SCISSORING, VG_FALSE );
    vgSetfv( VG_CLEAR_COLOR, 4, col );
    
    vgClear( m_boundingRect.origin.x , m_boundingRect.origin.y , m_boundingRect.size.width , m_boundingRect.size.height );
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXRect GXPath::getBoundingRect() const
{
//    vgPathBounds( VGPath path, VGfloat *minx , VGfloat *miny , VGfloat *width , VGfloat *heigh);
//    vgPathTransformedBounds
    
    return m_boundingRect;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXPath::applyTransformation( GXTransformation const transformation)
{
    //  { sx, shy, w0,
    //    shx, sy, w1,
    //    tx, ty, w2 }
 const float mat[9] = { transformation.sx  , transformation.shy , transformation.w0 ,
                        transformation.shx , transformation.sy  , transformation.w1 ,
                        transformation.tx , transformation.ty , transformation.w2
                      };
    
#ifdef TARGET_RASPBERRY_PI
    vgLoadMatrix( mat );
#endif
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXTransformation GXPath::getCurrentTransformation() const 
{
    float mat[9] = { 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , 0.0f , };
    
#ifdef TARGET_RASPBERRY_PI
    vgGetMatrix( mat );
#endif
    
    return makeTransformation(mat, GXTransformation_Unknown);
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

inline GXPoint GXPath::applyOriginTransformation( const GXPoint &point)
{
    if ( m_origin == TopLeft_Origin )
        return makePoint( m_boundingRect.size.height - point.x, point.y);
    
    return point;
}

void GXPath::changed()
{
    if ( m_shouldClearAfterRender && m_rendercount)
    {
        clear();
        m_rendercount = 0;
    }
}



