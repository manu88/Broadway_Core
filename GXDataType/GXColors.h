/*!
 *  \brief     GXColor
 *  \details   API + internal colors' representation
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef COLORS_H
#define COLORS_H

#include <stdint.h>
#include <vector>
#include "GXGeometry.h"

/*! \file GXColors.h
 \brief public and internal color's description.
 Note : Regarding color's component, the API uses integer values 
 from 0 to 255 instead of [0 , 1.] floating points values.
 
 
 */
//! API's color representation.
typedef struct
{
    //! red component
    uint8_t r;
    
    //! green component
    uint8_t g;
    
    //! blue component
    uint8_t b;
    
    //! alpha component        
    uint8_t a;
    
} GXColor;

typedef struct
{
public:
    
    GXPoint p1;
    GXPoint p2;
    
    float par;
    
    // linear gradient:
    // p1 = start
    // p2 = end
    // par unused
    
    // radial gradient:
    // p1  = center
    // p2  = focus
    // par = radius
    
    
    void addStop(GXColor color)
    {
        m_stops.push_back( color);
    }
    
    int getNumStops() const
    {
        return ( int ) m_stops.size();
    }
    
    GXColor getStopAt (int index)
    {
        return m_stops.at( index );
    }

private:
    std::vector<GXColor> m_stops;
    
} GXColorGradient;


//! Secondary representation using floating point values.
/*!
 Since the API is written to use GXColor (with [0 , 255] integer values), you should _not_
 use this representation, provided for compatibility and internal use only.
*/
typedef struct
{
    //! red component
    float r;
    
    //! green component    
    float g;
    
    //! blue component
    float b;
    
    //! alpha component       
    float a;
    
} GXColorFLOAT;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
//! Create a new GXColor, with RGB(A) components.
/*!
 */
GXColor makeColor(uint8_t r , uint8_t g , uint8_t b , uint8_t a = 255 );



bool operator==(const GXColor& lhs, const GXColor& rhs);
bool operator!=(const GXColor& lhs, const GXColor& rhs);


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Converters
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

//! Convert a color description from RGB24 to RGB565 colorspace.
uint16_t colorsRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b);

//! Convert a color description from RGBA24 to ARGB8888 colorspace.
uint32_t colorsRGBA24toARGB8888(uint8_t r, uint8_t g, uint8_t b , uint8_t  a );

//! Convert a GXcolor description from API representation to ARGB8888 colorspace.
uint32_t GXColorToARGB8888(GXColor color );


//! Convert a GXcolor description from API representation to float representation.
GXColorFLOAT GXColorToGXColorFLoat(const GXColor &color );




#endif /* COLORS_H */
