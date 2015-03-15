//
//  GXFont.h
//  Broadway_test
//
//  Created by Manuel Deneu on 10/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXFont__
#define __Broadway_test__GXFont__

#include <unordered_set>
#include <vector>
#include <algorithm>

#include "../Internal/Object.h"
#include "../GXDataType/Curve.h"

#include "../Log/Log.h"



class FontParser;

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

class GXGlyph : public Object
{
    friend class FontParser;    
public:
    GXGlyph();
    ~GXGlyph();
    
    void addSegment( Curve_float *curve);
    
    unsigned long getNumSegments() const
    {
        return m_segments.size();
    }
    
    const Curve_float* getCurveAt( int index ) const
    {
        return m_segments[ index ];
    }
    
    void dump();
    
    float getAdvanceX() const
    {
        return m_advanceX;
    }
    
    float getAdvanceY() const
    {
        return m_advanceY;
    }
    
private:
    float m_multiplier;
    
    float m_advanceX;
    float m_advanceY;
    
    std::vector< Curve_float*> m_segments;
    

};

/* **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** */

class GXFont : public Object
{
    friend class FontParser;
    
public:
    
    // screen resolution in DPI : size in pix = size in points * resolution in DPI / 72
    enum { DPI_Res = 72 };

    static GXFont* loadFont( const std::string &fileName);
    
    static GXFont* getFontByName    ( const std::string &fontName);
    static GXFont* getFontByFileName( const std::string &fileName);
    
    static GXFont* getDefault()
    {
        return getFontByName("Source Sans Pro Regular");
    }
    
    static unsigned long getNumFontsLoaded()
    {
        return s_fontsAtlas.size();
    }

    static const std::unordered_set<GXFont*> &getFontAtlas()
    {
        return s_fontsAtlas;
    }
    
    static void deleteLoadedFonts();
    
    /* **** **** **** **** **** **** */
    
    int getLineSpace() const
    {
        return m_lineSpace;
    }
    
    std::string getName() const
    {
        return m_fontName;
    }
    
    std::string getFileName() const
    {
        return m_fileName;
    }
    
    unsigned long getNumGlyphes() const
    {
        return m_glyphes.size();
    }
    
    void addChar( char c , GXGlyph *glyphe );
    
    const GXGlyph* getCharacter(const char c) const
    {
        auto vectorIt = std::find_if(m_glyphes.begin(),m_glyphes.end(), CompareFirst( c ) );
        
        if ( vectorIt != m_glyphes.end() )
            return vectorIt->second;
        
//        Log::log("\n char '%c' not found in font '%s'" , c , m_fontName.c_str() );
        
        return nullptr;

    }
    
    void dump();

protected:
    GXFont();
    ~GXFont();
    
private:
    
    void changeGlyphesSize();
    
    // font settings
    
    std::string m_fontName;
    int         m_lineSpace;
    std::string m_fileName;

    std::vector< std::pair<char , GXGlyph* > > m_glyphes;
    
    
    /* *** *** */
    
    // predicate for finding glyph by char
    struct CompareFirst
    {
        CompareFirst(int c) : m_char(c) {}
        
        bool operator()(const std::pair<char,GXGlyph* >& elem) const
        {
            return m_char == elem.first;
        }
    private:
        char m_char;
    };
    
    // font atlas
    static std::unordered_set<GXFont*> s_fontsAtlas;
};

#endif /* defined(__Broadway_test__GXFont__) */
