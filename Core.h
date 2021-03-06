#ifndef Broadway_Core_h
#define Broadway_Core_h

#include "Config.h"

#ifndef CORE_VERSION
#define CORE_VERSION "0.3.0"
#endif

/*
 The following modules are cross-plateform and dont need any plateform-specific defs:
 
 - JSMachine
 - WebServer
 - Network
 - XML_Parsers
 
 For others modules, available plateforms are :
 
 Cubieboard2 : define TARGET_CUBIE2
 RaspberryPi : define TARGET_RASPBERRY_PI
 MacOs       : no define. will auto detect __APPLE__
 
 
 */


#include "Plateforms/Plateform.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/**/

/*
#ifndef ENABLE_ELEMENT_SELECTOR
#define ENABLE_ELEMENT_SELECTOR
#endif
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
 -  If you're using Broadway's Makefile, simply edit and use the one in Core folder and set
    User's flag ( Webserver , JavaMachine, etc.) to 1 in order to build the module, or 0.
 
 - If Not, you can simply define the following symbols in order to activate Broadway's modules
 
 USE_WEB_SERVER       
 USE_JAVA_INTERPRETER
 USE_NETWORK
 USE_GPIO
 USE_GRAPHICS
 USE_GRAPHICS_HELPERS
 USE_GRAPHICS_EXTRA 
 USE_XML_PARSER
 
 
 
 HAVE_PNG_LIB
 HAVE_JPEG_LIB

 */



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */



/* JSON Parser*/
/*
    This module uses cJSON code
    http://cjson.sourceforge.net/
 */
#ifdef USE_JSON_PARSER
#include "Parsers/JSONParser.h"
#endif

/* XML parser*/
/*
    This module needs xerces lib to be installed in standard include + lib paths
    http://xerces.apache.org/xerces-c/
 */
#ifdef USE_XML_PARSER
#include "Parsers/XMLParser.h"
#endif

/* HTML parser*/

#ifdef USE_HTML_PARSER
#include "Parsers/HTMLParser.h"
#endif

/* Web Server */

#ifdef USE_WEB_SERVER
#include "Web/WebServer.h"
#endif

/* JS Machine */

#ifdef USE_JAVA_INTERPRETER
#include "JSMachine/JSMachine.h"
#include"JSMachine/JSMachineDelegate.h"
#endif

/* Network & Osc management */
/*
 This module needs oscpack lib to be installed in standard include + lib paths
 http://www.rossbencina.com/code/oscpack
 */
#ifdef USE_NETWORK
#include "Network/NetworkController.h"
#include "Network/IPC.h"
#endif

/* Gpio management */

#ifdef USE_GPIO
#include "Interfaces/InterfaceController.h"
#endif

/* Graphics context */

#ifdef USE_GRAPHICS_EXTRA
    #ifndef USE_GRAPHICS
        #define USE_GRAPHICS
        #warning USE_GRAPHICS_EXTRA needs USE_GRAPHICS! Symbol is now defined.
    #endif

    #include "GXExtra/GXVideo.h"
    #include "GXExtra/GraphicsComponents.h"
#endif

#ifdef USE_GRAPHICS_HELPERS
    #include "Parsers/SVGParser.h"
    #include "Parsers/FontParser.h"
#endif

#ifdef USE_GRAPHICS
    #include "GX/GXDefs.h"
    #include "GX/DisplayController.h"
    #include "GX/GXElement.h"
    #include "GX/GXPath.h"
    #include "GX/GXImage.h"
    #include "GX/GXScene.h"
    #include "GX/GXAsyncPainter.h"
    #include "GX/GXTextRenderer.h"
    #include "GX/GXFont.h"
#endif


#ifdef USE_GRAPHIC_UI

    #ifndef USE_XML_PARSER
    #define USE_XML_PARSER
    #endif

    #ifndef USE_GRAPHICS
    #define USE_GRAPHICS
    #endif

    #include "GXExtra/UI/GXUI.h"
#endif

/* Core Headers */

#include "Data/Variant.h"

#include "Data/Database.h"

#include "Internal/FileSystem.h"
#include "Internal/Utils.h"

#include "Scheduler/Scheduler.h"

#include "Log/Log.h"

#include "Application/ApplicationBase.h"






/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Small class to store modules configurations
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

typedef enum
{
    EMPTY               =   0,
    WEB_SERVER          = ( 1 << 0 ),
    JAVA_INTERPRETER    = ( 1 << 1 ),
    NETWORK             = ( 1 << 2 ),
    GPIO                = ( 1 << 3 ),
    GRAPHICS            = ( 1 << 4 ),
    GRAPHICS_HELPERS    = ( 1 << 5 ),
    GRAPHICS_EXTRA      = ( 1 << 6 ),
    XML_PARSING         = ( 1 << 7 ),
    
    ALL                 = -1
} CoreModulesFlags;

class CoreModules
{
public:
    
    CoreModules() : _mdlList( EMPTY )
    {}
    
    ~CoreModules()
    {}
    
    void addModule( const CoreModulesFlags module)
    {
        _mdlList = ( CoreModulesFlags ) ( _mdlList | module );
    }
    
    void addAllModules()
    {
        _mdlList = ALL;
    }
    
    
    void removeModule( const CoreModulesFlags module)
    {
        _mdlList = ( CoreModulesFlags ) ( _mdlList & ~module );
    }
    
    void removeAllModules()
    {
        _mdlList = EMPTY;
    }
    
    void setModule( const CoreModulesFlags module , bool add)
    {
        if ( add )
            addModule( module );
        else
            removeModule( module );
    }
    
    bool checkModule( const CoreModulesFlags module)
    {
        return _mdlList & module;
    }
    

    
private:
    CoreModulesFlags _mdlList;

};

#endif /* Broadway_Core_h */
