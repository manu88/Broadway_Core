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
#include <vector>


#include "../../GX/GXScene.h"
#include "../../GX/GXAsyncPainter.h"

#include "../../Parsers/XMLParser.h"

#include "../../GXDataType/GXGeometry.h"
#include "../../Scheduler/Event.h"

#include "../../Internal/AbstractController.h"
#include "../../Internal/AbstractDelegate.h"


/* **** **** **** **** **** ****  */

typedef struct
{
    GXRect bounds;
    int    layer;
    
}GXGeometry;

/* **** **** **** **** **** ****  */

typedef enum
{
    UIAction_none       = 0,
    UIAction_clic       = 1,
    UIAction_doubleClic = 2
    
} UIActionType;

/* **** **** **** **** **** ****  */

typedef struct
{
    GXGeometry zone;
    
    UIActionType type;
    
    std::string selector;
    std::string target;
    std::string emitter;
    
} UIAction;

static UIAction action_invalid = { { makeRectNULL() , -1} , UIAction_none };


typedef std::vector<UIAction> ActionList;



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
class GXUI; // Forwards

class UIDelegate : public virtual AbstractDelegate
{
    friend class GXUI;
public:
    virtual ~UIDelegate() {}
    
    
protected:
    UIDelegate() {}
    
    // !override this method to get user interactions .
    virtual bool uiActionReceived( const UIAction &action) = 0;
    
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class GXButton; // Forwards


class GXUI : public GXScene
{
public:
    
    GXUI();
    ~GXUI();
    
    void setDelegate( UIDelegate *delegate )
    {
        _delegate = delegate;
    }
    
    bool parseXMLFile( const std::string &file);
    
    /**/
    
    const UIAction &performClicAtPoint( const GXPoint &pt);

    
    
private:
    // parse an Element Node and return its geometry.
    static GXGeometry getGeometryFromElement( const XMLParser::XMLElement *element );
    
    static ActionList getActionsFromElement( const XMLParser::XMLElement *element );
    
    // called by parseXMLFile()
    bool addGXImage( const XMLParser::XMLElement *element );
    bool addGXText ( const XMLParser::XMLElement *element );
    bool addGXPath ( const XMLParser::XMLElement *element );
    
    // called by each add**()
    bool addUIElement( GXElement *element );
    
    bool addUIActionList( const ActionList &list );
    
    GXAsyncPainter _painter;
    
    struct actionComparator
    {
        bool operator() (const UIAction &lhs, const UIAction &rhs) const
        {
            return lhs.zone.layer <= rhs.zone.layer;
        }
    };
    
    ActionList _actionList;
    
    UIDelegate *_delegate;

};




#endif /* defined(__Broadway_core__GXUI__) */
