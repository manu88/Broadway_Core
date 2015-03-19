/*!
 *  \brief     GXGeometry
 *  \details   API geometric's description
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef _GXGeometry_h
#define _GXGeometry_h

/*! \file GXGeometry.h
 \brief Simple geometric elements
 */




typedef struct
{
    int x;
    int y;
} GXPoint;

/* **** **** **** **** **** **** **** **** **** **** **** */

typedef struct
{
    int width;
    int height;
    
} GXSize;

/* **** **** **** **** **** **** **** **** **** **** **** */

typedef struct
{
    GXPoint p0;
    GXPoint p1;
    
} GXLine;

/* **** **** **** **** **** **** **** **** **** **** **** */

typedef struct
{
    GXPoint origin;
    GXSize size;
    
} GXRect;


GXSize makeSizeNULL();
GXSize makeSize( int width , int height );

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makePoint( int x , int y )
    \brief Creates a point with {x,y} coordinates
    \param x x coordinate
    \param y y coordinate 
    \return A point at {x,y}.
 */
GXPoint makePoint( int x , int y );

/*! \fn makePointNULL()
 \brief Creates a point with {0,0} coordinates
 \return A point at {0,0}.
 */
GXPoint makePointNULL();

/*! \fn makeLine( GXPoint p0 , GXLine p1)
 \brief Creates a line crossing p0 and p1.
 \param p0 A point.
 \param p1 An other point.
 \return A Line.
 */
GXLine makeLine( const GXPoint &p0 , const GXPoint &p1 );
GXLine makeLine( int p0x , int p0y , int p1x , int p1y);

/*! \fn makeRect( GXPoint origin , GXPoint size )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param origin Top-left Corner
 \param size   GXPoint structure containing the width and height as coordinates
 \return A Rectangle. 
 */
GXRect makeRect( const GXPoint &origin , const GXSize &size );


/*! \fn makeRect( int x , int y , int width , int height )
 \brief Creates a rectangle with an origin (top-left corner) and a size.
 \param x Top-left Corner's x coordinate
 \param y Top-left Corner's x coordinate
 \param width rectangle's width
 \param height rectangle's height
 \return A Rectangle.
 */
GXRect makeRect( int x , int y , int width , int height );


/* **** **** **** **** **** **** **** **** **** **** **** */
/*! \fn makeRectNULL()
 \brief Creates a rectangle with {0,0} coordinates and size
 \return A null rectangle.
 */
GXRect makeRectNULL();


bool rectContainsPoint( const GXRect &rect ,const GXPoint &pt );

bool rectIntersectsRect(const GXRect &rect1 ,const GXRect &rect2 );

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Operators
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXPoint& lhs, const GXPoint& rhs);
bool operator==(const GXSize& lhs, const GXSize& rhs);
bool operator==(const GXPoint& lhs, const GXSize& rhs);

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXPoint operator+(const GXPoint& lhs, const GXPoint& rhs);
GXPoint operator-(const GXPoint& lhs, const GXPoint& rhs);

GXPoint operator+=(const GXPoint& lhs, const GXPoint& rhs);
GXPoint operator-=(const GXPoint& lhs, const GXPoint& rhs);

GXPoint operator+(const GXPoint& lhs, const GXSize& rhs);
GXPoint operator+(const GXSize& lhs, const GXPoint& rhs);
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXPoint& lhs, const GXPoint& rhs);
bool operator!=(const GXSize&  lhs, const GXSize&  rhs);

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXRect& lhs, const GXRect& rhs);

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXRect& lhs, const GXRect& rhs);

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXRect operator+=(const GXRect& lhs, const GXPoint& rhs);
GXRect operator-=(const GXRect& lhs, const GXPoint& rhs);

GXRect operator+(const GXRect& lhs, const GXPoint& rhs);
GXRect operator-(const GXRect& lhs, const GXPoint& rhs);

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXLine operator+=(const GXLine& lhs, const GXPoint& rhs);
GXLine operator-=(const GXLine& lhs, const GXPoint& rhs);

GXLine operator+(const GXLine& lhs, const GXPoint& rhs);
GXLine operator-(const GXLine& lhs, const GXPoint& rhs);

#endif
