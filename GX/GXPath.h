/*!
 *  \brief     GXPath
 *  \details   Basic drawing operations
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       addEllipse : revoir coordonnées. Ok normalement?
 *  \warning   .
 *  \copyright GNU Public License.
 */


/*
 
 changement de coords système openVG ( origine en bas à gauche) vers origine en haut à gauche :

 translation +y de la hauteur de l'écran
 scale   sur y de -1
 
 */

#ifndef ____GXPath__
#define ____GXPath__
/*
#ifdef TARGET_RASPBERRY_PI
    #include <EGL/egl.h>
    #include <GLES/gl.h>
    #include "VG/openvg.h"
    #include "VG/vgu.h"

#elif __APPLE__
    #include "../Plateforms/MacOS/MacDummies/vg_mac.h"
#endif
*/
#include <iostream>
#include <stdio.h>

#include "../GXDataType/GXTransformations.h"
#include "../GXDataType/GXColors.h"
#include "../GXDataType/GXGeometry.h"
#include "../GXDataType/Curve.h"


#include "../Internal/Object.h"

#include "GXDefs.h"

/* **** **** **** **** **** */

typedef enum
{
    Arc_Open  = 0,
    Arc_Chord = 1,
    Arc_Pie   =2
    
} GXArcType;

/* **** **** **** **** **** */

typedef enum
{
    Cap_Butt   = 0, // def in openVG
    Cap_Round  = 1,
    Cap_Square = 2
    
} GXCapStyle ;

/* **** **** **** **** **** */ 

typedef enum
{
    Join_Miter = 0, // def in openVG
    Join_Round = 1,
    Join_Bevel = 2
    
} GXJoinStyle;

/* **** **** **** **** **** */

typedef enum
{
    Fill_EvenOdd = 0, // def in openVG
    Fill_NonZero = 1
    
} GXFillRule;

/* **** **** **** **** **** */

typedef enum
{
    Paint_color           = 0,
    Paint_Gradient_Linear = 1,
    Paint_Gradient_Radial = 2,
    Paint_Pattern         = 3,
    
    
} GXPaintType;


// openvg specifies origin to be at bottom Left of the screen.
// set m_origin to To TopLeft_Origin to match general specs.

typedef enum
{
    BottomLeft_Origin = 0,
    TopLeft_Origin    = 1
} Origin;


//! A set of tool to draw vectorized elements on screen
/********************************************//**
*
*   
*
*
*
***********************************************/
class GXPath : public Object
{
public:
    
    GXPath( const GXRect &bounds);
    ~GXPath();
    
    void clear();
    
    // painter operations
    static void setStrokeColor  ( const GXColor &color );

    static void setFillColor    ( const GXColor &color );
    static void setFillGradient ( const GXColorGradient &color );
    
    static void setStrokeWidth( float width );
    static void setCapStyle ( GXCapStyle style );
    static void setJoinStyle( GXJoinStyle style );
    
    static void setFillRule( GXFillRule rule );
    static void setFillPainterType( GXPaintType  type);
    
    static void scissorRect( const GXRect &rect);
    static void clearRect( const GXRect &rect , const GXColor &color);

    
    void appendPath( const GXPath &otherPath);
    
    /* basic shapes*/
    
    //! add a rectangle.
    /*!
     \param rect the rect to draw.
     */
    void addRect       ( const GXRect &rect);

    //! add a rectangle, with rounded (elliptical) corners.
    /*!
     \param rect the rect to draw.
     \param arcW the ellipse's base width.
     \param arcH the ellipse's base height.
     */
    void addRoundedRect( const GXRect &rect , float arcW , float arcH);

    //! add a line, given to points.
    /*!
     \param p0 a point.
     \param p1 another point.
     */
    void addLine       ( const GXPoint &p0 , const GXPoint &p1 );
    void addLine       ( GXLine line );
    
    //! add an ellipse.
    /*!
     \param rect the rectangle bouding the ellipse to draw
     */
    void addEllipse ( const GXRect &boundingRect); // bug coordonnées
    void addArc     ( const GXPoint &center ,  const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type );

    /* curves */
    
    //! Move the current point
    /*!
     \param point the destination.
     */
    void moveToPoint        ( const GXPoint &point);
    
    //! Draw a line from the current point to another point
    /*!
     \param point the end of the segment to draw.
     */
    void addLineToPoint     ( const GXPoint &point);
    
    //
    void addCubicCurveToPoint( const GXPoint &controlPoint0, const GXPoint &controlPoint1 , const GXPoint &endPoint );
    void addQuadCurveToPoint( const GXPoint &controlPoint , const GXPoint &endPoint);
    
    
    void addCurve(const Curve_float &curve);
    //
    void closePath();
    
    //! Render the path's outline.
    void stroke();
    
    //! Fill the path's interior.
    /*!
     If the path as not been closed with GXPath::closePath(),
     it will be closed by a simple segment, whitch will not be displayed (no stroke on this segment).
     */
    void fill();
    
    //! decide if the path has to been cleaned after fill/stroke operation.
    /*!
     When set to true, will allows to do drawing routines like this:
     @code
     setStrokeColor(...);
     addLine( ... );
     addRect(...);
     stroke();
     setFillColor(...);
     addEllipse( ... );
     fill();
    @endcode
     With shouldClearAfterRenderOps set to true, the line and rect will be only stroked, then the ellipse
     will be filled. But with shouldClearAfterRenderOps set to false, the line and rect will be stroked _AND_ filled, 
     then the ellipse will be filled.
     */
    void shouldClearAfterRenderOps( bool shouldI)
    {
        m_shouldClearAfterRender = shouldI;
    }
    
    //! Get the global geometry of the path.
    /*!
        \return A rectangle bounding the drawed shape.
     */
    
    void clearRect( const GXColor &color);
    GXRect getBoundingRect() const;
    
    
    // transformations
    void applyTransformation( GXTransformation const transformation);
    GXTransformation getCurrentTransformation() const ;
    
private:
    inline GXPoint applyOriginTransformation( const GXPoint &point);
    
    void changed();
    
    bool m_shouldClearAfterRender;
    int  m_rendercount;

    VGPath  m_path;

    
    GXPoint m_currentPoint;
    GXRect  m_boundingRect;
    
    Origin  m_origin;
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    /* static converters  */
    
    static inline VGCapStyle GXCapStyleToVG( const GXCapStyle & style)
    {
        if (style == Cap_Round)
            return VG_CAP_ROUND;
        
        else if (style == Cap_Butt)
            return VG_CAP_BUTT;
        
        return VG_CAP_SQUARE;
    }
    
    static inline VGJoinStyle GXJoinStyleToVG( const GXJoinStyle & style)
    {
        if (style == Join_Round)
            return VG_JOIN_ROUND;
        
        else if (style == Join_Miter)
            return VG_JOIN_MITER;
        
        return VG_JOIN_BEVEL;
    }
};



#endif /* defined(____GXPath__) */
