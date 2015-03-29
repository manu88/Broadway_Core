//
//  GXFont.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "GXFont.h"
#include "../Parsers/FontParser.h"



std::unordered_set<GXFont*> GXFont::s_fontsAtlas;

GXGlyph::GXGlyph()
{
    className = "GXGlyph";
}

GXGlyph::~GXGlyph()
{
    for (auto i : m_segments )
    {
        delete i;
    }
    m_segments.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXGlyph::addSegment( Curve_float *curve)
{
    m_segments.push_back( curve );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXFont::GXFont() :
m_fontName      ( "" ),
m_lineSpace     ( 0  ),
m_fileName      ( "" )
{
    className = "GXFont";
    
}

GXFont::~GXFont()
{
    for (auto i : m_glyphes)
        delete i.second;
    
    m_glyphes.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXFont* GXFont::loadFont( const std::string &fileName)
{
    FontParser p;
    
    GXFont* font = getFontByName( p.getFontName( fileName ) );
    
    if ( font == nullptr)
    {
        font = new GXFont();
        
        if ( p.parseFontFile( font , fileName ) )
            s_fontsAtlas.insert( font );
        else
        {
            delete font;
            return nullptr;
        }
    }
    else
        Log::log("\n font already loaded");
    
    return font;
    

}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ GXFont* GXFont::getFontByName( const std::string &fontName)
{
    if ( fontName == "Undefined")
        return nullptr;
    
    for ( auto i = s_fontsAtlas.begin() ; i != s_fontsAtlas.end() ; i++ )
    {
        if ( (*i)->m_fontName == fontName )
            return (*i);
    }
    return nullptr;
}
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ GXFont* GXFont::getFontByFileName( const std::string &fileName)
{
    for ( auto i = s_fontsAtlas.begin() ; i != s_fontsAtlas.end() ; i++ )
    {
        if ( (*i)->m_fileName == fileName )
            return (*i);
    }
    return nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void GXFont::deleteLoadedFonts()
{
    for ( auto i : s_fontsAtlas )
    {

        delete i;
    }
    
    s_fontsAtlas.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXFont::addChar(char c , GXGlyph *glyphe)
{
    m_glyphes.push_back( std::make_pair( c , glyphe ) );
    
}

void GXFont::changeGlyphesSize()
{
    
}

