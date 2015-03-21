//
//  GXUI.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 20/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "../../GX/GXImage.h"
#include "../../GX/GXTextRenderer.h"

#include "../../GX/GXFont.h"

#include "GXUI.h"
#include "GXUIXML.h"

#include "../../Log/Log.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** */

GXUI::GXUI()
{
    className = "GXUI";
}

GXUI::~GXUI()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::parseXMLFile( const std::string &file)
{
    XMLParser p;
    
    if ( !p.parseFile( file ) )
        return false;
    
    const std::string ui_name = p.getAttributeForTag( UI_TITLE_NODE_NAME, ATTR_VALUE );
    
    const int x = std::stoi( p.getAttributeForTag( GEOMETRY_NODE_NAME, ATTR_X ) );
    const int y = std::stoi( p.getAttributeForTag( GEOMETRY_NODE_NAME, ATTR_Y ) );
    const int w = std::stoi( p.getAttributeForTag( GEOMETRY_NODE_NAME, ATTR_W ) );
    const int h = std::stoi( p.getAttributeForTag( GEOMETRY_NODE_NAME, ATTR_H ) );
    
    const GXColor bColor = makeColor( std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "r") ),
                                      std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "g") ),
                                      std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "b") ),
                                      std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "a") )
                                     );
    
    
    printf("\n title = '%s'" , ui_name.c_str() );
    printf("\n bounds = %i %i %i %i" , x , y , w , h);
    printf("\n background color = %i %i %i %i" , bColor.r , bColor.g , bColor.b , bColor.a);
    

    
    const XMLParser::XMLElement *uiChildNode = XMLParser::getChildElementNamed( p.getRootElement(), ELEMENTS_ROOT_NODE_NAME );
    
    if ( uiChildNode )
    {
        for (const auto element : XMLParser::getChilds( uiChildNode ) )
        {
            if ( XMLParser::getName(element) == GXIMAGE_NODE_NAME )
            {
                printf("\n got an image");
                
                if ( !addGXImage( element ) )
                    printf("\n erreur Image ");

                
            }
            
            else if ( XMLParser::getName(element) == GXTEXT_NODE_NAME )
            {
                printf("\n got a text label");
                
                if ( !addGXText( element ) )
                    printf("\n erreur Text ");
                
            }
            
            else if ( XMLParser::getName(element) == GXPATH_NODE_NAME )
            {
                printf("\n got a path");
            }
            

            


            
        }
    }
    
    

    
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ GXGeometry GXUI::getGeometryFromElement( const XMLParser::XMLElement *element )
{
    if (element)
    {
        const int layer = std::stoi( XMLParser::getAttribute(element , ATTR_LAYER ) );
        
        const int x = std::stoi( XMLParser::getAttribute(element , ATTR_X ) );
        const int y = std::stoi( XMLParser::getAttribute(element , ATTR_Y ) );
        const int w = std::stoi( XMLParser::getAttribute(element , ATTR_W ) );
        const int h = std::stoi( XMLParser::getAttribute(element , ATTR_H ) );
        
        return {makeRect(x, y, w, h) , layer};
    }
    
    return {makeRectNULL() , 0};
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXImage( const XMLParser::XMLElement *element )
{
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );
    printf("\n Element name = '%s'" , name.c_str() );
    
    const std::string filePath = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, SOURCE_NODE_NAME ) , ATTR_FILEPATH );
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    printf("\n filepath : '%s'" , filePath.c_str() );
    
    printf("\n bounds = %i %i %i %i" , geometry.bounds.origin.x ,
           geometry.bounds.origin.y ,
           geometry.bounds.size.width ,
           geometry.bounds.size.height
           );
    
    printf("\n layer = %i" , geometry.layer);
    
    
    
    GXImage *img = new GXImage( filePath );
    img->setLayer ( geometry.layer  );
    img->setBounds( geometry.bounds );
    
    img->setName( name );
    
    return addUIElement( img );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXText( const XMLParser::XMLElement *element )
{

    
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );
    printf("\n Element name = '%s'" , name.c_str() );
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    const std::string text     = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_VALUE );
    
    const GXPoint carret = makePoint( std::stoi(XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_X )),
                                      std::stoi(XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_Y ) )
                                     );
    
    
    const std::string fontFile = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_PATH );
    
    const float size           = std::stof( XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_SIZE ));
    
    printf("\n text = '%s' " , text.c_str() );
    printf("\n font = '%s' size = %f " , fontFile.c_str() ,size );
    
    GXFont *font = GXFont::loadFont( fontFile );
    

        

    GXTextRenderer *label = new GXTextRenderer();
    
    if (font)
        label->setFont( font );
    
    label->setLayer ( geometry.layer );
    label->setBounds( geometry.bounds );
    label->setDisplayedText( text );
    
    label->setCarretPosition( carret );
    label->setSizeInPoints( size );
    
    label->setName( name );
    
    return addUIElement( label );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXPath ( const XMLParser::XMLElement *element )
{
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );
    printf("\n Element name = '%s'" , name.c_str() );
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    
    GXPaintJS *painter = new GXPaintJS();
    
    painter->setName( name );
    painter->setBounds( geometry.bounds );
    painter->setLayer( geometry.layer );
    
    
    return addUIElement( painter );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addUIElement( GXElement *element )
{

    return addElement( element );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */






