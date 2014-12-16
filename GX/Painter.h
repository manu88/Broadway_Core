/*!
 *  \brief     Painter + derived classes
 *  \details   Holds a simple paint element to be used in GXPaintJS 
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __Broadway_test__Painter__
#define __Broadway_test__Painter__

#include "../Internal/Object.h"

#include "../GXDataType/GXGeometry.h"
#include "../GXDataType/GXColors.h"

#include "GXPath.h"

typedef enum
{
    // basic shapes
    PainterType_Rect        = 1,
    PainterType_RoundedRect = 2,
    PainterType_Ellipse     = 3,
    PainterType_Line        = 4,
    PainterType_Arc         = 5,
    
    // path related
    PainterType_MoveTo       = 10,
    PainterType_LineTo       = 11,
    PainterType_CubicCurveTo = 12,
    PainterType_QuadCurveTo  = 13,
    PainterType_Close        = 14,
    
    // actions
    PainterType_ColorStroke = 20,
    PainterType_ColorFill   = 21,
    PainterType_StrokeWidth = 22,
    PainterType_CapStyle    = 23,
    PainterType_JoinStyle   = 24,
    PainterType_Fill        = 30,
    PainterType_Stroke      = 31,
    
} PainterType;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class Painter : public Object
{
    friend class GXPaintJS;    
public:

    virtual ~Painter();
    
    PainterType getType() const
    {
        return m_type;
    }
    
    
protected:
    Painter( PainterType type );
    PainterType m_type;
    
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// basic shapes

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterRect : public Painter
{
public:
    PainterRect( const GXRect &rect);
    ~PainterRect();
    
    GXRect getRect() const
    {
        return m_rect;
    }

    
private:
    GXRect m_rect;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterRoundedRect : public Painter
{
public:
    PainterRoundedRect( const GXRect &rect , float arcW , float arcH);
    ~PainterRoundedRect();
    
    GXRect getRect() const
    {
        return m_rect;
    }
    
    float getArcW() const
    {
        return m_arcW;
    }
    
    float getArcH() const
    {
        return m_arcH;
    }
    
    
private:
    GXRect m_rect;
    float m_arcW;
    float m_arcH;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterEllipse : public Painter
{
public:
    PainterEllipse( const GXRect &rect);
    ~PainterEllipse();
    
    GXRect getRect() const
    {
        return m_rect;
    }

    
private:
    GXRect m_rect;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterArc : public Painter
{
public:
    PainterArc( const GXPoint &center ,  const GXSize &size , float startAngle /* rads*/ , float angleExtent /* rads*/ ,GXArcType type );
    ~PainterArc();
    

    GXPoint getCenter() const
    {
        return m_center;
    }
    
    GXSize getSize() const
    {
        return m_size;
    }
    
    float getStartAngle() const
    {
        return m_startAngle;
    }
    
    float getAngleExtent() const
    {
        return m_angleExtent;
    }
    
    GXArcType getType() const
    {
        return m_type;
    }
    
    
private:

    GXPoint   m_center;
    GXSize    m_size;
    float     m_startAngle;
    float     m_angleExtent;
    GXArcType m_type;
};



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterLine : public Painter
{
public:
    PainterLine( const GXLine &line);
    ~PainterLine();
    
    GXLine getLine() const
    {
        return m_line;
    }
    
private:
    GXLine m_line;

    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// path manipulations

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
class PainterMoveTo : public Painter
{
public:
    PainterMoveTo( const GXPoint &point);
    ~PainterMoveTo();
    
    GXPoint getPoint() const
    {
        return m_point;
    }
    
    
private:
    GXPoint m_point;
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterLineTo : public Painter
{
public:
    PainterLineTo( const GXPoint &point );
    ~PainterLineTo();
    
    GXPoint getPoint() const
    {
        return m_point;
    }
    
    
private:
    GXPoint m_point;
    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterCubicCurveTo : public Painter
{
public:
    PainterCubicCurveTo( const GXPoint &controlPoint0 ,const GXPoint &controlPoint1 , const GXPoint &endPoint );
    ~PainterCubicCurveTo();
    
    GXPoint getCp0() const
    {
        return m_cp0;
    }
    
    GXPoint getCp1() const
    {
        return m_cp1;
    }
    
    GXPoint getEndPoint() const
    {
        return m_endPoint;
    }
    
private:
    GXPoint m_cp0;
    GXPoint m_cp1;
    GXPoint m_endPoint;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterQuadCurveTo : public Painter
{
public:
    PainterQuadCurveTo( const GXPoint &controlPoint , const GXPoint &endPoint);
    ~PainterQuadCurveTo();
    
    GXPoint getCp() const
    {
        return m_cp;
    }
    
    GXPoint getEndPoint() const
    {
        return m_endPoint;
    }
    
private:
    GXPoint m_cp;
    GXPoint m_endPoint;    
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterClose : public Painter
{
public:
    PainterClose();
    ~PainterClose();
    
    
    
private:
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

// Path actions

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
class PainterStrokeColor : public Painter
{
    friend class GXPaintJS;    
public:
    PainterStrokeColor( const GXColor &color);
    ~PainterStrokeColor();
        
private:
    GXColor m_color;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterFillColor : public Painter
{
    friend class GXPaintJS;    
public:
    PainterFillColor( const GXColor &color);
    ~PainterFillColor();
    

    
private:
    GXColor m_color;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterFill : public Painter
{
public:
    PainterFill();
    ~PainterFill();
    

    
private:
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterStroke : public Painter
{
public:
    PainterStroke();
    ~PainterStroke();

    
private:
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterStrokeWidth : public Painter
{
public:
    PainterStrokeWidth( float width );
    ~PainterStrokeWidth();
    
    float getWidth() const 
    {
        return m_width;
    }
    
private:
    float m_width;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterCapStyle : public Painter
{
public:
    PainterCapStyle( GXCapStyle capStyle ):
        Painter( PainterType_CapStyle ),
        style( capStyle )
    {}
    
    ~PainterCapStyle()
    {}
    
    GXCapStyle style;
    

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class PainterJoinStyle : public Painter
{
public:
    PainterJoinStyle( GXJoinStyle joinStyle ):
    Painter( PainterType_JoinStyle ),
    style( joinStyle )
    {}
    
    ~PainterJoinStyle()
    {}
    
    GXJoinStyle style;
    
    
};






#endif /* defined(__Broadway_test__Painter__) */
