//
//  GXUI.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 20/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "../GX/GXImage.h"
#include "../GX/GXTextRenderer.h"

#include "GXUI.h"
#include "GXUIXML.h"

#include "../Log/Log.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** */

GXUI::GXUI()
{
    
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
    
    return addUIElement( img );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXText( const XMLParser::XMLElement *element )
{
    GXTextRenderer *label = new GXTextRenderer();
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    const std::string text     = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_VALUE );
    const std::string fontFile = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_PATH );
    
    const float size           = std::stof( XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_SIZE ));
    
    printf("\n text = '%s' " , text.c_str() );
    printf("\n font = '%s' size = %f " , fontFile.c_str() ,size );
    
    
    label->setLayer ( geometry.layer );
    label->setBounds( geometry.bounds );
    
    return addUIElement( label );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addUIElement( GXElement *element )
{
    return true;// addElement( element );
}







