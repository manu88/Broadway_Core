//
//  GXUI.h
//  Broadway_core
//
//  Created by Manuel Deneu on 20/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__GXUI__
#define __Broadway_core__GXUI__

#include <iostream>

#include <map>

#include "../GX/GXScene.h"
#include "../GX/GXPaintJS.h"

#include "../Parsers/XMLParser.h"

#include "../GXDataType/GXGeometry.h"

/* **** **** **** **** **** ****  */

typedef struct
{
    GXRect bounds;
    int    layer;
    
}GXGeometry;

/* **** **** **** **** **** ****  */

// Forwards

class GXButton;


class GXUI : public GXScene
{
public:
    
    GXUI();
    ~GXUI();
    
    bool parseXMLFile( const std::string &file);
    
    /**/
    

    
    
private:
    // parse an Element Node and return its gemoetry.
    static GXGeometry getGeometryFromElement( const XMLParser::XMLElement *element );
    
    // called by parseXMLFile()
    bool addGXImage( const XMLParser::XMLElement *element );
    bool addGXText ( const XMLParser::XMLElement *element );
    bool addGXPath ( const XMLParser::XMLElement *element );
    
    // called by each add**()
    bool addUIElement( GXElement *element );
    
    
    
    
    GXPaintJS _painter;
    

    
};




#endif /* defined(__Broadway_core__GXUI__) */
