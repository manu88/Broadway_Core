//
//  GXColors.cpp
//  
//
//  Created by Manuel Deneu on 28/08/14.
//
//

#include <stdio.h>
#include "GXColors.h"



GXColor makeColor(uint8_t r , uint8_t g , uint8_t b , uint8_t a     )
{
    return GXColor { r ,  g ,  b ,  a };
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator==(const GXColor& lhs, const GXColor& rhs)
{
    return (    ( lhs.r == rhs.r )
            && ( lhs.g == rhs.g )
            && ( lhs.b == rhs.b )
            && ( lhs.a == rhs.a )
            );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool operator!=(const GXColor& lhs, const GXColor& rhs)
{
    return !( lhs == rhs);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
 Converters
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

uint16_t colorsRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

uint32_t colorsRGBA24toARGB8888(uint8_t r, uint8_t g, uint8_t b , uint8_t  a )
{
    return    ((a & 0xff) << 24)
    | ((r & 0xff) << 16)
    | ((g & 0xff) << 8)
    | ((b & 0xff)) ;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

uint32_t GXColorToARGB8888(GXColor color )
{
    return colorsRGBA24toARGB8888( color.r , color.g , color.b , color.a );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

GXColorFLOAT GXColorToGXColorFLoat(const GXColor &color )
{
    return GXColorFLOAT { (float) color.r /255.0f ,
                          (float) color.g /255.0f ,
                          (float) color.b /255.0f ,
                          (float) color.a /255.0f
                        };
}

