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

GXUI::GXUI() :
_delegate( nullptr )
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
    
    const GXColor bColor = makeColor( ( uint8_t ) std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "r") ),
                                      ( uint8_t ) std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "g") ),
                                      ( uint8_t ) std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "b") ),
                                      ( uint8_t ) std::stoi( p.getAttributeForTag( BACKGROUND_COLOR_NODE_NAME, "a") )
                                     );
    

    printf("\n title = '%s'" , ui_name.c_str() );
    printf("\n bounds = %i %i %i %i" , x , y , w , h);
    printf("\n background color = %i %i %i %i" , bColor.r , bColor.g , bColor.b , bColor.a);


    setElementName( ui_name );
    setBounds( x, y, w, h);
    setBackgroundColor( bColor );
    
    const XMLParser::XMLElement *uiChildNode = XMLParser::getChildElementNamed( p.getRootElement(), ELEMENTS_ROOT_NODE_NAME );
    
    if ( uiChildNode )
    {
        for (const auto element : XMLParser::getChilds( uiChildNode ) )
        {
//            printf("\n name '%s'", XMLParser::getName( element).c_str() );
            
            addUIActionList( getActionsFromElement(element ) );
            
            if ( XMLParser::getName(element) == GXIMAGE_NODE_NAME )
            {
//                printf("\n got an image");
                
                if ( !addGXImage( element ) )
                    printf("\n erreur Image ");

                
            }
            
            else if ( XMLParser::getName(element) == GXTEXT_NODE_NAME )
            {
//                printf("\n got a text label");
                
                if ( !addGXText( element ) )
                    printf("\n erreur Text ");
                
            }
            
            else if ( XMLParser::getName(element) == GXPATH_NODE_NAME )
            {
                if ( !addGXPath( element ) )
                    printf("\n erreur path");
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

/*static*/ ActionList GXUI::getActionsFromElement( const XMLParser::XMLElement *element )
{
    ActionList ret;
    
    if (element )
    {
        const std::string emitter = XMLParser::getAttribute( element, ATTR_NAME );
        
        const auto *actionClicNode =XMLParser::getChildElementNamed( element, ACTIONCLIC_NODE_NAME );
        
        if( actionClicNode )
        {
            const std::string selector = XMLParser::getAttribute(actionClicNode, ATTR_ACTION_SELECTOR );
            const std::string target = XMLParser::getAttribute(actionClicNode, ATTR_ACTION_TARGET );
            

            
            ret.push_back({ getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME )) ,
                            UIAction_clic ,
                            selector ,
                            target ,
                            emitter
                            });
            
        }
        
        
    }
    
    return ret;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXImage( const XMLParser::XMLElement *element )
{
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );

    
    const std::string filePath = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, SOURCE_NODE_NAME ) , ATTR_FILEPATH );
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
//    printf("\n filepath : '%s'" , filePath.c_str() );
  
    /*
    printf("\n bounds = %i %i %i %i" , geometry.bounds.origin.x ,
           geometry.bounds.origin.y ,
           geometry.bounds.size.width ,
           geometry.bounds.size.height
           );
    
    printf("\n layer = %i" , geometry.layer);
    
    */
    
    GXImage *img = new GXImage( filePath );
    img->setLayer ( geometry.layer  );
    img->setBounds( geometry.bounds );
    
    img->setElementName( name );
    
    return addUIElement( img );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXText( const XMLParser::XMLElement *element )
{

    
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );

    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    const std::string text     = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_VALUE );
    
    const GXPoint carret = makePoint( std::stoi(XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_X )),
                                      std::stoi(XMLParser::getAttribute( XMLParser::getChildElementNamed( element, TEXT_NODE_NAME ) , ATTR_Y ) )
                                     );
    
    
    const std::string fontFile = XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_PATH );
    
    const float size           = std::stof( XMLParser::getAttribute( XMLParser::getChildElementNamed( element, FONT_NODE_NAME ) , ATTR_SIZE ));
    
    /*
    printf("\n text = '%s' " , text.c_str() );
    printf("\n font = '%s' size = %f " , fontFile.c_str() ,size );
    */
    GXFont *font = GXFont::loadFont( fontFile );
    

        

    GXTextRenderer *label = new GXTextRenderer();
    
    if (font)
        label->setFont( font );
    
    label->setLayer ( geometry.layer );
    label->setBounds( geometry.bounds );
    label->setDisplayedText( text );
    
    label->setCarretPosition( carret );
    label->setSizeInPoints( size );
    
    label->setElementName( name );
    
    return addUIElement( label );
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXUI::addGXPath ( const XMLParser::XMLElement *element )
{
    const std::string name = XMLParser::getAttribute( element, ATTR_NAME );
//    printf("\n Element name = '%s'" , name.c_str() );
    
    const GXGeometry geometry = getGeometryFromElement( XMLParser::getChildElementNamed( element, GEOMETRY_NODE_NAME ));
    
    
    GXAsyncPainter *painter = new GXAsyncPainter();
    
    painter->setElementName( name );
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

bool GXUI::addUIActionList( const ActionList &list )
{
    _actionList.insert( _actionList.end() , list.begin() , list.end() );
    
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */


const UIAction& GXUI::performClicAtPoint( const GXPoint &pt)
{
    
    for ( const auto &action : _actionList )
    {
        /*
        printf("\n Action : ");
        printf("\n\tBounds %i %i %i %i" , action.zone.bounds.origin.x ,
                                                  action.zone.bounds.origin.y ,
                                                  action.zone.bounds.size.width ,
                                                  action.zone.bounds.size.height
               );
        printf("\n\tSelector = '%s'" , action.selector.c_str() );
        printf("\n\tTarget = '%s'" ,   action.target.c_str() );
        printf("\n\tEmitter = '%s'" ,  action.emitter.c_str() );
        printf("\n\tType %i" ,         action.type);
        */
        
        if ( rectContainsPoint(action.zone.bounds, pt))
        {
            if (_delegate)
                _delegate->uiActionReceived( action);
            
            return action;
        }
    }
    
    return action_invalid;
    
}



