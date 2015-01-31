//
//  DisplayImpl.h
//  Broadway_core
//
//  Created by Manuel Deneu on 27/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__DisplayImpl__
#define __Broadway_core__DisplayImpl__

#include <iostream>
#include <vector>

#include "../../Internal/Object.h"
#include "../../GXDataType/GXGeometry.h"

/*
    Actual implementation of Display operations.

 On raspberry Pi, performed with bcm_host.
 
 
 */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

typedef enum
{
    Display_Invalid = -1,
    Display_SD      = 0,
    Display_HD      = 1
    
} DisplayType ;


/* Notification flag passed to displayChangeNotification()  */
typedef enum
{
    /* 
        For now, those are the same than defined for the raspberry PI in bcm_host
        ( vc_sdtv.h and vc_hdmi.h )
     */
    HDMI_UNPLUGGED           = (1 << 0), // <HDMI cable is detached
    HDMI_ATTACHED            = (1 << 1), // <HDMI cable is attached but not powered on
    HDMI_DVI                 = (1 << 2), // <HDMI is on but in DVI mode (no audio)
    HDMI_HDMI                = (1 << 3), // <HDMI is on and HDMI mode is active
    HDMI_HDCP_UNAUTH         = (1 << 4), // <HDCP authentication is broken (e.g. Ri mismatched) or not active
    HDMI_HDCP_AUTH           = (1 << 5), // <HDCP is active
    HDMI_HDCP_KEY_DOWNLOAD   = (1 << 6), // <HDCP key download successful/fail
    HDMI_HDCP_SRM_DOWNLOAD   = (1 << 7), // <HDCP revocation list download successful/fail
    HDMI_CHANGING_MODE       = (1 << 8), // <HDMI is starting to change mode, clock has not yet been set
    
    SDTV_UNPLUGGED           = 1 << 16, //  <SDTV cable unplugged, subject to platform support
    SDTV_ATTACHED            = 1 << 17, //  <SDTV cable is plugged in
    SDTV_NTSC                = 1 << 18, //  <SDTV is in NTSC mode
    SDTV_PAL                 = 1 << 19, //  <SDTV is in PAL mode
    SDTV_CP_INACTIVE         = 1 << 20, //  <Copy protection disabled
    SDTV_CP_ACTIVE           = 1 << 21, //   <Copy protection enabled
    
    DISPLAY_ACTIVE           = 1 << 30
    
} DisplayNotification;


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* small POD containing display infos */
struct DisplayInformations
{

    bool        native;
    
    DisplayType type;
    GXSize      size;
    
    int         framerate;
    int         aspectRatio;
    
    bool isValid() const
    {
        return type != Display_Invalid;
    }

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class DisplayController; // forward


class DisplayImpl : public Object
{
    friend class DisplayController;
    

    
    DisplayImpl( DisplayController *controller);
    ~DisplayImpl();
    
    static bool initPlateform();
    static bool deInitPlateform();
    
    bool initDisplay();
    
    float getAspectRatio();
    
    /* Query informations about current display */
    const DisplayInformations getCurrentDisplayInformations();
    
    const std::vector< DisplayInformations > getAvailableVideoMode() const ;
    
private:  /* attributes */
    
    static void initializeEGL();
    
    
    
    
    
    bool _displayInitOk;
    DisplayController *_controller;
    
    static bool s_EGLInitialized;
    
    // RPI callback from bcm_host 
    static void tvCallback(void *userdata, uint32_t reason, uint32_t param1, uint32_t param2);
};

#endif /* defined(__Broadway_core__DisplayImpl__) */