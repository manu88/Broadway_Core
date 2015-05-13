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

#include "../DisplayController.h"
#include "DisplayImpl.h"
#include "../GXDefs.h"




bool DisplayImpl::s_EGLInitialized = false;

#ifdef USE_OPENMAXIL

CRBP     DisplayImpl::s_RBP = CRBP();
COMXCore DisplayImpl::s_OMX = COMXCore();

#endif

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
#ifdef TARGET_RASPBERRY_PI
    
    
    
    
    uint32_t screen_width  = 0;
	uint32_t screen_height = 0;
    
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;
    
    
    //	VC_RECT_T dst_rect; // GX_EL
    //	VC_RECT_T src_rect;
    
    VC_RECT_T  dst_rect; // rpi
    VC_RECT_T  src_rect; // rpi
    
    DISPMANX_ELEMENT_HANDLE_T   element; // rpi
    
	const EGLint attribute_list[] =
    {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};
    
    
    
	// get an EGL display connection
	_EGLdisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	assert( _EGLdisplay != EGL_NO_DISPLAY );
    
	// initialize the EGL display connection
	result = eglInitialize( _EGLdisplay, NULL, NULL);
	assert(EGL_FALSE != result);
    
	EGLConfig config;
    
	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig( _EGLdisplay, attribute_list, &config, 1, &num_config);
	assert( EGL_FALSE != result );
    
	// create an EGL rendering context
	_context = eglCreateContext( _EGLdisplay, config, EGL_NO_CONTEXT, NULL);
	assert( _context != EGL_NO_CONTEXT );
    
	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */ , &screen_width,&screen_height);
	assert( success >= 0 );
    /* 
     On raspberry PI, this assertion usualy means bcm_host_init() was not called priori to this :
     Don't forget to create an instance of ScopedPlateformConfig at the app startup! 
     */
    
    
    
    int oriX = 0;
    int oriY = 0;
    
    
	dst_rect.x = oriX;
	dst_rect.y = oriY;
	dst_rect.width = screen_width;
	dst_rect.height = screen_height;
    

    
	src_rect.x = oriX;
	src_rect.y = oriY;
	src_rect.width = screen_width << 16;
	src_rect.height =screen_height << 16;
    
    
    /* Partie déjà dans GXScene*/
	DISPMANX_DISPLAY_HANDLE_T dispman_display; // GXScene
	DISPMANX_UPDATE_HANDLE_T dispman_update;   // GXScene
    
	dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
	dispman_update = vc_dispmanx_update_start(0);
    
    
    //VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 0xff, 0 };
	element = vc_dispmanx_element_add( dispman_update,
                                      dispman_display,
                                      0 ,
                                      &dst_rect,
                                      0 /*src */ ,
                                      &src_rect,
                                      DISPMANX_PROTECTION_NONE,
                                      0 /*alpha */ ,
                                      0 /*clamp */ ,
                                      (DISPMANX_TRANSFORM_T) 0 /*transform */
                                      );
    
	_nativewindow.element = element;
	_nativewindow.width = screen_width;
	_nativewindow.height = screen_height;
    
	vc_dispmanx_update_submit_sync( dispman_update );
    /**/
    
    
	_surface = eglCreateWindowSurface( _EGLdisplay, config, &_nativewindow, NULL);
	assert( _surface != EGL_NO_SURFACE);
    
	// preserve the buffers on swap
	result = eglSurfaceAttrib( _EGLdisplay, _surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED );
	assert(EGL_FALSE != result);
    
	// connect the context to the surface
	result = eglMakeCurrent( _EGLdisplay, _surface, _surface, _context );
	assert(EGL_FALSE != result);
    
	// set up screen ratio
	glViewport(0, 0, (GLsizei) screen_width, (GLsizei) screen_height);
    
    
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    
	float ratio = (float) screen_width / (float) screen_height;
	glFrustumf(-ratio, ratio, -1.0f, 1.0f, 1.0f, 10.0f);
    
#endif /* TARGET_RASPBERRY_PI */
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::deInitDisplay()
{
#ifdef TARGET_RASPBERRY_PI
    
    eglDestroyContext(_EGLdisplay , _context );
    
#endif /*TARGET_RASPBERRY_PI*/
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

float DisplayImpl::getAspectRatio()
{
#ifdef TARGET_RASPBERRY_PI
    
#else
    return 1.0f;
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

const DisplayInformations DisplayImpl::getCurrentDisplayInformations() const
{
#ifdef TARGET_RASPBERRY_PI
    
    TV_DISPLAY_STATE_T currentState = {0,0};
    
    if (vc_tv_get_display_state( &currentState) == 0)
    {
        DisplayType type = Display_Invalid;
        
        GXSize size = makeSizeNULL();
        
        int frate;
        float aspectRatio;
        
        // HD or DVI
        if (currentState.state  & ( VC_HDMI_HDMI | VC_HDMI_DVI ) )
        {
            type = Display_HD;
            
            HDMI_DISPLAY_STATE_T desc= ( HDMI_DISPLAY_STATE_T ) currentState.display.hdmi;
            
            size.width  = desc.width;
            size.height = desc.height;
            
            frate = desc.frame_rate;
            
            aspectRatio = get_display_aspect_ratio( (HDMI_ASPECT_T) currentState.display.hdmi.aspect_ratio);
        }
        
        // SD
        else
        {
            type = Display_SD;
            
            SDTV_DISPLAY_STATE_T desc= ( SDTV_DISPLAY_STATE_T ) currentState.display.sdtv;
            
            size.width  = desc.width;
            size.height = desc.height;
            
            frate = desc.frame_rate;
            
            aspectRatio = get_display_aspect_ratio( (SDTV_ASPECT_T) currentState.display.sdtv.display_options.aspect);
        }
        
        aspectRatio *= (float) currentState.display.hdmi.height / (float) currentState.display.hdmi.width;
        
        return DisplayInformations{ false, type , size , frate , aspectRatio  };
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
                                             get_display_aspect_ratio( (HDMI_ASPECT_T) list[i].aspect_ratio )
                                           }
                      );
    }
    
    assert( count == (int) ret.size() );
    
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

int DisplayImpl::getAudioVideoLatency() const
{
    int ret = 0;
    
#ifdef TARGET_RASPBERRY_PI
    ret = vc_tv_hdmi_get_av_latency();
#endif
    
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayImpl::update()
{
    /*
     typedef enum {
     VG_NO_ERROR                                 = 0,
     VG_BAD_HANDLE_ERROR                         = 0x1000,
     VG_ILLEGAL_ARGUMENT_ERROR                   = 0x1001,
     VG_OUT_OF_MEMORY_ERROR                      = 0x1002,
     VG_PATH_CAPABILITY_ERROR                    = 0x1003,
     VG_UNSUPPORTED_IMAGE_FORMAT_ERROR           = 0x1004,
     VG_UNSUPPORTED_PATH_FORMAT_ERROR            = 0x1005,
     VG_IMAGE_IN_USE_ERROR                       = 0x1006,
     VG_NO_CONTEXT_ERROR                         = 0x1007
     } VGErrorCode;*/

#ifdef HAVE_EGL
    VGErrorCode ret = vgGetError();
    
    /*
    if (ret !=  VG_NO_ERROR)
    {
        if (ret == VG_BAD_HANDLE_ERROR )
            printf("VG_BAD_HANDLE_ERROR");
        
        else if (ret == VG_ILLEGAL_ARGUMENT_ERROR )
            printf("VG_ILLEGAL_ARGUMENT_ERROR");
        
        else if (ret == VG_OUT_OF_MEMORY_ERROR )
            printf("VG_OUT_OF_MEMORY_ERROR");
        
        else if (ret ==VG_PATH_CAPABILITY_ERROR  )
            printf("VG_PATH_CAPABILITY_ERROR");
        
        else if (ret == VG_UNSUPPORTED_IMAGE_FORMAT_ERROR  )
            printf("VG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
        
        else if (ret == VG_UNSUPPORTED_PATH_FORMAT_ERROR  )
            printf("VG_UNSUPPORTED_PATH_FORMAT_ERROR");
        
        else if (ret == VG_IMAGE_IN_USE_ERROR  )
            printf("VG_IMAGE_IN_USE_ERROR");
        
        else if (ret == VG_NO_CONTEXT_ERROR  )
            printf("VG_NO_CONTEXT_ERROR");

        
    }
     */
//    assert( ret == VG_NO_ERROR );
    
    // fix for raspberry : rendering freezes after a short time ...
//    glFinish();

//    vgFinish();
//    vgFlush();
    
	eglSwapBuffers( _EGLdisplay, _surface);

//	assert( eglGetError() == EGL_SUCCESS);
#endif
}

void DisplayImpl::checkErrors()
{
#ifdef TARGET_RASPBERRY_PI
    VGErrorCode ret = vgGetError();
    
/*
        switch (ret) {
            case VG_BAD_HANDLE_ERROR:
                printf("\nVG_BAD_HANDLE_ERROR");
                break;
            case VG_ILLEGAL_ARGUMENT_ERROR:
                printf("\nVG_ILLEGAL_ARGUMENT_ERROR");
                break;
            case VG_OUT_OF_MEMORY_ERROR:
                printf("\nVG_OUT_OF_MEMORY_ERROR");
                break;
            case VG_PATH_CAPABILITY_ERROR:
                printf("\nVG_PATH_CAPABILITY_ERROR");
                break;
            case VG_UNSUPPORTED_IMAGE_FORMAT_ERROR:
                printf("\nVG_UNSUPPORTED_IMAGE_FORMAT_ERROR");
                break;
            case VG_UNSUPPORTED_PATH_FORMAT_ERROR:
                printf("\nVG_UNSUPPORTED_PATH_FORMAT_ERROR");
                break;
            case VG_IMAGE_IN_USE_ERROR:
                printf("\nVG_IMAGE_IN_USE_ERROR");
                break;
            case VG_NO_CONTEXT_ERROR:
                printf("\nVG_NO_CONTEXT_ERROR");
                break;
            default:
                printf("\nUNKNOWN_ERROR");
        }

*/
    
    
   // assert( ret == VG_NO_ERROR );
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::setVideoModeTo( const DisplayInformations &mode)
{
    bool ret = false;
    
#ifdef TARGET_RASPBERRY_PI
    
    /* HDMI_INTERLACED_T =  HDMI_NONINTERLACED / HDMI_INTERLACED */
    
    /* EDID_MODE_MATCH_FLAG_T  = HDMI_MODE_MATCH_FRAMERATE HDMI_MODE_MATCH_RESOLUTION HDMI_MODE_MATCH_SCANMODE*/
    
    ret =  vc_tv_hdmi_power_on_best( /*uint32_t width*/  mode.size.width  ,
                                     /*uint32_t height*/ mode.size.height ,
                                     /*uint32_t frame_rate*/ mode.framerate ,
                                     /*HDMI_INTERLACED_T scan_mode*/ HDMI_INTERLACED ,
                                     /* EDID_MODE_MATCH_FLAG_T match_flags*/ HDMI_MODE_MATCH_RESOLUTION
                                    ) == 0 ;
#endif
    
    return ret;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::showInfosOnDisplay( bool show ) const
{
    bool ret = false;
  
#ifdef TARGET_RASPBERRY_PI
    ret =  vc_tv_show_info( (uint32_t) show) == 0;
#endif
    
    return ret;
    
}

bool DisplayImpl::sendTvPowerOff() const
{
    bool ret = false;
    
#ifdef TARGET_RASPBERRY_PI
    ret = (bool) vc_tv_power_off();
#endif
    
    return ret;
}

bool DisplayImpl::sendTvPowerOn() const
{
    bool ret = false;

#ifdef TARGET_RASPBERRY_PI
    ret = vc_tv_hdmi_power_on_preferred() == 0;
    
#endif
    return ret;
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    
    Init Part
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayImpl::initPlateform()
{
    
#ifdef USE_OPENMAXIL
    s_RBP.Initialize();
    s_OMX.Initialize();
#endif
    
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
        EGLBoolean ret = eglBindAPI(EGL_OPENVG_API);
        assert(ret != EGL_FALSE);
#endif
        s_EGLInitialized = true;
    }

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    RaspberryPI ( bcmhost ) methods
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef TARGET_RASPBERRY_PI
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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

/*static*/ float DisplayImpl::get_display_aspect_ratio(HDMI_ASPECT_T aspect)
{
    float display_aspect;
    
    switch (aspect)
    {
        case HDMI_ASPECT_4_3:   display_aspect = 4.0/3.0;   break;
        case HDMI_ASPECT_14_9:  display_aspect = 14.0/9.0;  break;
        case HDMI_ASPECT_16_9:  display_aspect = 16.0/9.0;  break;
        case HDMI_ASPECT_5_4:   display_aspect = 5.0/4.0;   break;
        case HDMI_ASPECT_16_10: display_aspect = 16.0/10.0; break;
        case HDMI_ASPECT_15_9:  display_aspect = 15.0/9.0;  break;
        case HDMI_ASPECT_64_27: display_aspect = 64.0/27.0; break;
        default:                display_aspect = 16.0/9.0;  break;
    }
    
    return display_aspect;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*static*/ float DisplayImpl::get_display_aspect_ratio(SDTV_ASPECT_T aspect)
{
    float display_aspect;
    
    switch (aspect)
    {
        case SDTV_ASPECT_4_3:  display_aspect = 4.0/3.0;  break;
        case SDTV_ASPECT_14_9: display_aspect = 14.0/9.0; break;
        case SDTV_ASPECT_16_9: display_aspect = 16.0/9.0; break;
        default:               display_aspect = 4.0/3.0;  break;
    }
    
    
    return display_aspect;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool DisplayImpl::canSupportAudioFormat(EDID_AudioFormat audio_format,
                                              uint32_t num_channels,
                                              EDID_AudioSampleRate fs,
                                              uint32_t bitrate)
{
    return vc_tv_hdmi_audio_supported(audio_format, num_channels, fs, bitrate ) == 0;
}
#endif /*TARGET_RASPBERRY_PI*/



