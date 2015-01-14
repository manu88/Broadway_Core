//
//  FontParser.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "FontParser.h"

#include <assert.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <float.h>

#include "../GXDataType/Curve.h"
#include "../GX/GXFont.h"
#include "../Log/Log.h"

#include "../Internal/FileSystem.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

FontParser::FontParser() 
{
    className = "FontParser";
    
    if( FT_Init_FreeType( &_library ) )
    {
        Log::log("couldn't initialize freetype\n");
    }
    
    // set callback functions from Freetype to our code
    _functions.move_to  = FT_Outline_MoveToFunc;
    _functions.line_to  = FT_Outline_LineToFunc;
    _functions.conic_to = FT_Outline_ConicToFunc;
    _functions.cubic_to = FT_Outline_CubicToFunc;
    
}

FontParser::~FontParser()
{
    FT_Done_FreeType( _library );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
std::string FontParser::getFontName( const std::string &filename )
{
    FT_Face face;
    
    int faceIndex = 0;
    
    if( FT_New_Face( _library, filename.c_str(), faceIndex, &face ) != 0 )
    {
        Log::log("font not found \n");
        return "Undefined";
    }
    
    std::string name;
    if (face->family_name != NULL)
        name = face->family_name;// + face->style_name;
    else
        name = "No Name";

    name += " ";
    
    if  (face->style_name != NULL)
        name += face->style_name;

    else
        name+= " No Style";
    
    FT_Done_Face( face );
    
    return name;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool FontParser::parseFontFile(GXFont *font ,  const std::string &filename )
{
    if ( font == nullptr)
        return false;
    
    if (!FileSystem::fileExists( filename ))
        return false;

    
    FT_Face face;
    
    int faceIndex = 0;
    
    if( FT_New_Face( _library, filename.c_str(), faceIndex, &face ) != 0 )
    {
        Log::log("couldn't load new face\n");
        return false;
    }
    
    font->m_fileName = filename;
    
    if (face->family_name != NULL)
        font->m_fontName = face->family_name;
    else
        font->m_fontName = "No Name";

    font->m_fontName += " ";
    
    if  (face->style_name != NULL)
        font->m_fontName += face->style_name;
    else
        font->m_fontName += " No Style";
    
    
    FT_Set_Char_Size(
                     face,     
                     0 ,                              /* char_width in 1/64th of points  */
                     64*64,                           /* char_height in 1/64th of points */
                     HorizontalDeviceResolution ,     /* horizontal device resolution    */
                     VerticalDeviceResolution         /* vertical device resolution      */
                     );
    
   
    int num_glyphs = 0;
    
    int descent = face->descender;
    
    if (descent <0)
        descent *= -1;
    
    font->m_lineSpace = face->ascender + face->descender +  face->height;

    for(int cc = 0 ; cc<256 ; cc++ )
    {
        
        if( cc < 32 )
            continue;   //discard the first 32 characters
        
        int glyphIndex = FT_Get_Char_Index( face, cc );
        
        if( !FT_Load_Glyph( face, glyphIndex, FT_LOAD_NO_BITMAP | FT_LOAD_NO_HINTING | FT_LOAD_IGNORE_TRANSFORM ) )
        {
            GXGlyph *glyph = new GXGlyph(); // empty
            
            glyph->m_advanceX = (float) ( face->glyph->advance.x );
            glyph->m_advanceY = (float) ( face->glyph->advance.y );
            
            if( cc == ' ' )
            {

            }
            
            else
            {
                _functions.delta = 0;
                _functions.shift = 0;
                
                FT_Outline_Decompose( &face->glyph->outline, &_functions , glyph );

            }
            
            font->addChar(cc, glyph);
            
            num_glyphs++;
        }
    }
    if(! num_glyphs )
        Log::log("warning: no glyphs found\n");
    


    
    FT_Done_Face( face );
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Callback from FT_Outline_Decompose
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ int FontParser::FT_Outline_MoveToFunc( const FT_Vector*  to, void *user )
{
    GXGlyph *gl = reinterpret_cast<GXGlyph*>( user );
    
    gl->addSegment( Curve_float::makeMoveTo(to->x, to->y));
    
    return 0;
}

/*static*/ int FontParser::FT_Outline_LineToFunc( const FT_Vector*  to, void *user )
{
    GXGlyph *gl = reinterpret_cast<GXGlyph*>( user );
    
    gl->addSegment( Curve_float::makeLineTo( to->x, to->y ) );
    
    return 0;
}

/*static*/ int FontParser::FT_Outline_ConicToFunc( const FT_Vector*  control, const FT_Vector*  to, void* user )
{
    GXGlyph *gl = reinterpret_cast<GXGlyph*>( user );
    
    gl->addSegment( Curve_float::makeQuadTo( control->x, control->y, to->x, to->y ) );

    return 0;
}

/*static*/ int FontParser::FT_Outline_CubicToFunc( const FT_Vector*  control1, const FT_Vector*  control2, const FT_Vector*  to, void* user )
{
    GXGlyph *gl = reinterpret_cast<GXGlyph*>( user );

    gl->addSegment( Curve_float::makeCubicTo( control1->x, control1->y,control2->x, control2->y, to->x, to->y ) );

    return 0;    
}

