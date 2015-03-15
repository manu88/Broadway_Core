//
//  SVGParser.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 09/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "SVGParser.h"

#include "../Log/Log.h"

#include "../GXDataType/GXGeometry.h"

#include "../GX/GXPaintJS.h"

#include "../Internal/FileSystem.h"

#define NANOSVG_IMPLEMENTATION
#include "NanoSVG/nanosvg.h"

SVGParser::SVGParser() :
_image  ( nullptr ),
_dpiRes ( 96.0f   )
{
    className = "SVGParser";
}
SVGParser::~SVGParser()
{
    if ( _image != nullptr)
        nsvgDelete( _image );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool SVGParser::parseSvgFile( const std::string &filename )
{
    if ( !FileSystem::fileExists( filename ) )
        return false;
    
//    'px', 'pt', 'pc' 'mm', 'cm', or 'in'
    _image = nsvgParseFromFile(filename.c_str() , "px", _dpiRes );
    
	if (_image == NULL)
    {
//        Log::log("Could not open SVG image '%s' " , filename.c_str() );
		return false;
	}
    
    return true;
}

bool SVGParser::populateGXPaintJS(GXPaintJS* element , bool shouldCleanBefore)
{
    if ( ! (_image && element ) )
        return false;
    
    NSVGshape* shape = nullptr;
	NSVGpath* path = nullptr;
    
    if ( shouldCleanBefore )
        element->clear();
    

    for (shape = _image->shapes; shape != NULL; shape = shape->next)
    {
        element->setFillColor( getColorFromNSVGpaint( shape->fill ) );
        element->setStrokeColor( getColorFromNSVGpaint( shape->stroke ) );
        
		for (path = shape->paths; path != NULL; path = path->next)
        {
            printf("\n got a path %i points" , path->npts );
            
            for ( int i = 0; i < path->npts-1; i += 3)
            {
                float* p = &path->pts[i*2];
                
                GXPoint x0 = makePoint( (int) p[0], (int) p[1] );
                GXPoint p0 = makePoint( (int) p[2], (int) p[3] );
                GXPoint p1 = makePoint( (int) p[4], (int) p[5] );
                GXPoint x1 = makePoint( (int) p[6], (int) p[7] );
                
                element->moveToPoint(x0);
                element->addCubicCurveToPoint(p0, p1, x1);
                

            }
            if (path->closed)
                element->close();

		}
	}

    
    return true;
}

void SVGParser::dump()
{
    NSVGshape* shape = nullptr;
	NSVGpath* path = nullptr;
    
    for (shape = _image->shapes; shape != NULL; shape = shape->next)
    {
        printf("\n got a shape");
		for (path = shape->paths; path != NULL; path = path->next)
        {
            printf("\n got a path %i points" , path->npts );
            
            for ( int i = 0; i < path->npts-1; i += 3)
            {
                float* p = &path->pts[i*2];
                printf("\n Cubic : %f %f %f %f %f %f %f %f" ,p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7] );
                
//                cubicBez(p[0],p[1], p[2],p[3], p[4],p[5], p[6],p[7], );
            }
            //drawPath(path->pts, path->npts, path->closed, px * 1.5f);
			//drawControlPts(path->pts, path->npts);
		}
	}
}

/*static*/ GXColor SVGParser::getColorFromNSVGpaint(const NSVGpaint &color)
{
    const unsigned int r = (color.color & 0xff000000) >> 24;
    const unsigned int g = (color.color & 0x00ff0000) >> 16;
    const unsigned int b = (color.color & 0x0000ff00) >> 8;
    const unsigned int a = (color.color & 0x000000ff);
    
    return makeColor(r, g, b , a);
}

