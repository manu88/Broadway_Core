//
//  DisplayImpl.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 27/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include <assert.h>

#ifdef TARGET_RASPBERRY_PI
    #include <bcm_host.h>
#endif

#include "DisplayController.h"
#include "DisplayImpl.h"
#include "../GXDefs.h"




bool DisplayImpl::s_EGLInitialized = false;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

DisplayImpl::DisplayImpl( DisplayController *controller ) :

_displayInitOk ( false ),
_controller    ( controller ),
_currentMode   ( DisplayInformations::makeInvalid() )
{
#ifdef TARGET_RASPBERRY_PI
    vc_tv_register_callback( tvCallback, this);
#endif
    
    
}

DisplayImpl::~DisplayImpl()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::initDisplay()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

float DisplayImpl::getAspectRatio()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

const DisplayInformations DisplayImpl::getCurrentDisplayInformations()
{
#ifdef TARGET_RASPBERRY_PI
    
    TV_DISPLAY_STATE_T currentState;
    
    if (vc_tv_get_display_state( &currentState) == 0)
    {
        DisplayType type = Display_Invalid;
        
        GXSize size = makeSizeNULL();
        
        int frate;
        
        // HD or DVI
        if (currentState.state  & ( VC_HDMI_HDMI | VC_HDMI_DVI ) )
        {
            type = Display_HD;
            
            HDMI_DISPLAY_STATE_T desc= ( HDMI_DISPLAY_STATE_T ) currentState.display.hdmi;
            
            size.width  = desc.width;
            size.height = desc.height;
            
            frate = desc.frame_rate;
        }
        
        // SD
        else
        {
            type = Display_SD;
            
            SDTV_DISPLAY_STATE_T desc= ( SDTV_DISPLAY_STATE_T ) currentState.display.sdtv;
            
            size.width  = desc.width;
            size.height = desc.height;
            
            frate = desc.frame_rate;            
        }
        
        
        return DisplayInformations{ false, type , size , frate , 0  };
    }
    
#endif
    
    return DisplayInformations{ false, Display_Invalid , makeSize( 0,0) , 0 , 0  };
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

const std::vector< DisplayInformations > DisplayImpl::getAvailableVideoMode() const
{
    std::vector<DisplayInformations> ret;
    
#ifdef TARGET_RASPBERRY_PI
    
    /*
     HDMI_RES_GROUP_T ->
     HDMI_RES_GROUP_INVALID = 0, //< Initialised value
    HDMI_RES_GROUP_CEA = 1, //< CEA - HDMI device
    HDMI_RES_GROUP_DMT = 2, //< DMT - computer monitors
     */
    
    const int maxSize = 20;
    
    TV_SUPPORTED_MODE_NEW_T list[maxSize];
    
    int count = vc_tv_hdmi_get_supported_modes_new(/*HDMI_RES_GROUP_T*/ HDMI_RES_GROUP_CEA , list , maxSize , NULL , NULL  );
    

    
    /*
     TV_SUPPORTED_MODE_NEW_T:
     uint32_t scan_mode : 1; //<1 is interlaced, 0 for progressive
    uint32_t native : 1; /: <1 means native mode, 0 otherwise
    uint32_t group : 3; // <group
    uint32_t code : 7; // <mode code
    uint32_t pixel_rep : 3; // <pixel repetition (zero means no repetition)
    uint32_t aspect_ratio : 5; // <aspect ratio of the format
    uint16_t frame_rate; // <frame rate
    uint16_t width; // <frame width
    uint16_t height; // <frame height
    uint32_t pixel_freq; // <pixel clock in Hz
    uint32_t struct_3d_mask;// <3D structure supported for this mode, only valid if group == CEA. This is a bitmask of HDMI_3D_STRUCT_T

     */
    
    for (int i = 0 ; i<count ; i++)
    {
        ret.push_back( DisplayInformations { (bool) list[i].native ,
                                             Display_HD,
                                             makeSize( list[i].width, list[i].height),
                                             list[i].frame_rate,
                                             ( int ) list[i].aspect_ratio
                                           }
                      );
    }
    
    assert( count == ret.size() );
    
#elif defined __APPLE__
    
    ret.push_back( DisplayInformations { (bool) true ,
                                          Display_SD,
                                          makeSize( 640 , 800),
                                          30,
                                          1
                                       }
                  );
    
    ret.push_back( DisplayInformations {    (bool) true ,
                                            Display_HD,
                                            makeSize( 1920 , 1080),
                                            60,
                                            3
                                        }
                  );
    
#endif

    return ret;
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void DisplayImpl::tvCallback(void *userdata, uint32_t reason, uint32_t param1, uint32_t param2)
{
    DisplayImpl *self = static_cast< DisplayImpl*>( userdata );
    
    assert( self );
    
    /*
     Reason HDMI :
    VC_HDMI_UNPLUGGED = (1 << 0), //<HDMI cable is detached
    VC_HDMI_ATTACHED = (1 << 1), // <HDMI cable is attached but not powered on
    VC_HDMI_DVI = (1 << 2), // <HDMI is on but in DVI mode (no audio)
    VC_HDMI_HDMI = (1 << 3), // <HDMI is on and HDMI mode is active
    VC_HDMI_HDCP_UNAUTH = (1 << 4), //<HDCP authentication is broken (e.g. Ri mismatched) or not active
    VC_HDMI_HDCP_AUTH = (1 << 5), // <HDCP is active
    VC_HDMI_HDCP_KEY_DOWNLOAD = (1 << 6), // <HDCP key download successful/fail
    VC_HDMI_HDCP_SRM_DOWNLOAD = (1 << 7), //<HDCP revocation list download successful/fail
    VC_HDMI_CHANGING_MODE  = (1 << 8), //<HDMI is starting to change mode, clock has not yet been set 
     
     SD:
     VC_SDTV_UNPLUGGED = 1 << 16, //<SDTV cable unplugged, subject to platform support
    VC_SDTV_ATTACHED = 1 << 17, //<SDTV cable is plugged in
    VC_SDTV_NTSC = 1 << 18, //<SDTV is in NTSC mode
    VC_SDTV_PAL = 1 << 19, //<SDTV is in PAL mode
    VC_SDTV_CP_INACTIVE = 1 << 20, //<Copy protection disabled
    VC_SDTV_CP_ACTIVE = 1 << 21 //<Copy protection enabled 

     */

    self->_controller->displayChangeNotification( ( DisplayNotification ) reason );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    
    Init Part
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::initPlateform()
{
    initializeEGL();
    
    
    return true;
}

bool DisplayImpl::deInitPlateform()
{
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void DisplayImpl::initializeEGL()
{
    if ( !s_EGLInitialized )
    {
#ifdef HAVE_EGL
        
        // bind OpenVG API
        eglBindAPI(EGL_OPENVG_API);
#endif
        s_EGLInitialized = true;
    }

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

