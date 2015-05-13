//
//  SVGParser.h
//  Broadway_test
//
//  Created by Manuel Deneu on 09/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__SVGParser__
#define __Broadway_test__SVGParser__

#include <iostream>


#include "../Internal/Object.h"
#include "../GXDataType/GXColors.h"


// forward
struct NSVGimage;
struct NSVGpaint;
class GXAsyncPainter;

class SVGParser : public Object
{
public:
    SVGParser();
    ~SVGParser();
    
    bool parseSvgFile( const std::string &filename );
    
    bool populateGXPaintJS( GXAsyncPainter* element , bool shouldCleanBefore);
    
    void dump();
    
    static GXColor getColorFromNSVGpaint( const NSVGpaint &color);
    
private:
    
    NSVGimage *_image;
    float      _dpiRes;
};

#endif /* defined(__Broadway_test__SVGParser__) */
