//
//  FontParser.h
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__FontParser__
#define __Broadway_test__FontParser__

#include "../Internal/Object.h"



#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H


class GXFont;

class FontParser : public Object
{
public:
    FontParser();
    ~FontParser();
    
    enum { HorizontalDeviceResolution = 300 , VerticalDeviceResolution= 300 };
    
    
    bool parseFontFile(GXFont *font,  const std::string &filename );
    std::string getFontName( const std::string &filename );
    
    
private:
    FT_Outline_Funcs m_functions;
    FT_Library m_library;
    

    
    static int FT_Outline_MoveToFunc( const FT_Vector*  to, void *user );
    static int FT_Outline_LineToFunc( const FT_Vector*  to, void *user );
    static int FT_Outline_ConicToFunc( const FT_Vector*  control, const FT_Vector*  to, void* user );
    static int FT_Outline_CubicToFunc( const FT_Vector*  control1, const FT_Vector*  control2, const FT_Vector*  to, void* user );
};

#endif /* defined(__Broadway_test__FontParser__) */
