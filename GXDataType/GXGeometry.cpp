//
//  GXGeometry.cpp
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#include <stdio.h>
#include "GXGeometry.h"

GXSize makeSizeNULL()
{
    return GXSize { 0 , 0 };
}

GXSize makeSize( int width , int height )
{
    return GXSize { width , height };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makePoint( int x , int y )
 \brief Creates a point with {x,y} coordinates
 \param x x coordinate
 \param y y coordinate
 \return A point at {x,y}.
 */
GXPoint makePoint( int x , int y )
{
    return GXPoint {  x ,  y};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makePointNULL()
 \brief Creates a point with {0,0} coordinates
 \return A point at {0,0}.
 */
GXPoint makePointNULL()
{
    return GXPoint {  0 ,  0};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeLine( GXPoint p0 , GXLine p1)
 \brief Creates a line crossing p0 and p1.
 \param p0 A point.
 \param p1 An other point.
 \return A Line.
 */
GXLine makeLine( const GXPoint &p0 , const GXPoint &p1 )
{
    return GXLine { p0 ,  p1 };
}

GXLine makeLine( int p0x , int p0y , int p1x , int p1y)
{
    return GXLine { makePoint(p0x, p0y) ,  makePoint(p1x, p1y) };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRect( GXPoint origin , GXPoint size )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param origin Top-left Corner
 \param size   GXPoint structure containing the width and height as coordinates
 \return A Rectangle.
 */
GXRect makeRect( const GXPoint &origin , const GXSize &size )
{
    return GXRect { origin ,  size};
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRect( int x , int y , int width , int height )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param x Top-left Corner's x coordinate
 \param y Top-left Corner's x coordinate
 \param width rectangle's width
 \param height rectangle's height
 \return A Rectangle.
 */
GXRect makeRect( int x , int y , int width , int height )
{
    return GXRect {  makePoint( x , y) , makeSize(width , height ) };
}

/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRectNULL()
 \brief Creates a rectangle with {0,0} coordinates and size
 \return A null rectangle.
 */
GXRect makeRectNULL()
{
    return GXRect {  makePointNULL() , makeSizeNULL() };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Geometry tests
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool rectContainsPoint( const GXRect &rect ,const GXPoint &pt )
{
    return (    ( pt.x >= rect.origin.x  ) && ( pt.x <= rect.origin.x + rect.size.width  )
             && ( pt.y >= rect.origin.y  ) && ( pt.y <= rect.origin.y + rect.size.height  )
           );
}

bool rectIntersectsRect(const GXRect &rect1 ,const GXRect &rect2 )
{
    GXPoint l1 = rect1.origin;
    GXPoint r1 = rect1.origin+ rect1.size;
    
    GXPoint l2 = rect2.origin;
    GXPoint r2 = rect2.origin+ rect2.size;
    
    // If one rectangle is on left side of other
    if (l1.x > r2.x || l2.x > r1.x)
        return false;
    
    // If one rectangle is above other
    if (l1.y < r2.y || l2.y < r1.y)
        return false;
    
    return true;
    
    /*
    return (    rect1.origin.x < (rect2.origin.x + rect2.size.x)
            && (rect2.origin.x + rect2.size.x) > rect1.origin.x
            &&  rect1.origin.y < (rect2.origin.y + rect2.size.y)
            && (rect2.origin.y + rect2.size.y) > rect1.origin.y
            );
  */
    /*
    return (    rectContainsPoint(rect1, rect2.origin )
             || rectContainsPoint(rect1, rect2.origin += rect2.size )
           );
     */
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
 Operators
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXPoint& lhs, const GXPoint& rhs)
{
    return (    ( lhs.x == rhs.x )
            &&  ( lhs.y == rhs.y )
            );
}

bool operator==(const GXSize& lhs, const GXSize& rhs)
{
    return (    ( lhs.width == rhs.width )
            &&  ( lhs.height == rhs.height )
            );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXPoint& lhs, const GXSize& rhs)
{
    return (    ( lhs.x == rhs.width )
            &&  ( lhs.y == rhs.height )
            );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXPoint& lhs, const GXPoint& rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator+(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x + rhs.x ,  lhs.y + rhs.y };
}

GXPoint operator+(const GXPoint& lhs, const GXSize& rhs)
{
    return GXPoint {  lhs.x + rhs.width ,  lhs.y + rhs.height };
}

GXPoint operator+(const GXSize& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.width + rhs.x ,  lhs.height + rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator-(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x - rhs.x ,  lhs.y - rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator+=(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x + rhs.x ,  lhs.y + rhs.y };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator-=(const GXPoint& lhs, const GXPoint& rhs)
{
    return GXPoint {  lhs.x - rhs.x ,  lhs.y - rhs.y };    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXRect& lhs, const GXRect& rhs)
{
    return (    ( lhs.origin == rhs.origin )
            &&  ( lhs.size   == rhs.size )
            );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXRect& lhs, const GXRect& rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXRect operator+=(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    + rhs.x,
                     lhs.origin.y    + rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );// GXRect { lhs.origin + rhs , lhs.size + rhs };
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXRect operator-=(const GXRect& lhs, const GXPoint& rhs)
{
//    return GXRect { lhs.origin - rhs , lhs.size - rhs };
    return  makeRect(lhs.origin.x    - rhs.x,
                     lhs.origin.y    - rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );
}

GXRect operator+(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    + rhs.x,
                     lhs.origin.y    + rhs.y,
                     lhs.size.width  ,
                     lhs.size.height );// GXRect { lhs.origin + rhs , lhs.size + rhs };
}
GXRect operator-(const GXRect& lhs, const GXPoint& rhs)
{
    return  makeRect(lhs.origin.x    - rhs.x,
                     lhs.origin.y    - rhs.y,
                     lhs.size.width ,
                     lhs.size.height);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXLine operator+=(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 + rhs, lhs.p1 + rhs);
}

GXLine operator-=(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 - rhs, lhs.p1 - rhs);
}

GXLine operator+(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 + rhs, lhs.p1 + rhs);
}

GXLine operator-(const GXLine& lhs, const GXPoint& rhs)
{
    return makeLine(lhs.p0 - rhs, lhs.p1 - rhs);
}



