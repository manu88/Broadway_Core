//
//  DisplayController.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 22/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//


#include <assert.h>
#include "DisplayController.h"
#include "../Internal/ThreadLock.h"
#include "../Log/Log.h"

#include "GXPath.h"


/*static*/ DisplayController* DisplayController::s_instance = nullptr;

/*static*/ bool DisplayController::s_EGLInitialized = false;

DisplayController::DisplayController() :
AbstractController   ( "Display controller" ),
Thread               ( "GUI"   ),

_impl( this ),

_displayIsOn         ( false ),
_delegate            ( nullptr ),
m_currentElement     ( nullptr ),
m_shouldClearContext ( true    ),

// dummy dimensions
m_screen_width  ( 200 ),
m_screen_height ( 100 ),
m_frameRate ( 0 )
{

    className = "Display controller";
    
#ifdef USE_OPENMAXIL
    m_RBP.Initialize();
    m_OMX.Initialize();
#endif
    
#ifdef TARGET_RASPBERRY_PI
   // displayDidChange();
#endif

    s_instance = this;
    
    setLayer(1);

}

DisplayController::~DisplayController()
{
    stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::start()
{
    return startThread();
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::stop()
{
    return stopThread();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::removeElement( GXElement* element)
{
    ThreadLock l(this);
    element->removeFromParent();
    
}

void DisplayController::setDisplayedElement( GXElement* element)
{
    ThreadLock l(this);    
    m_currentElement = element;

    m_currentElement->setParentElement( this );
    
    m_currentElement->setBounds(0 ,0 , m_screen_width , m_screen_height);
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearScreen()
{
    m_shouldClearContext = true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::update()
{
    ThreadLock l(this);
    
    if (m_currentElement)
        m_currentElement->setNeedsDisplay();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::run()
{
    initializeEGL();
    

    

    
    Log::log("screen size is %i %i" , _bounds.size.width , _bounds.size.height );
    
/*
    for( const DisplayInformations &mode : _impl.getAvailableVideoMode() )
    {
        printf("\n ==== MODE  =====" );
        printf("\n\t W = %i H = %i" , mode.size.width , mode.size.height );
        printf("\n\t native %s" , mode.native? "YES" : "NO ");
        printf("\n\t framerate : %i" , mode.framerate);
        printf("\n\t pixelRatio : %i " , mode.aspectRatio);
        
    }
*/
    
    DisplayInformations mode = _impl.getCurrentDisplayInformations();
    
    printf("\n ==== MODE  =====" );
    printf("\n\t W = %i H = %i" , mode.size.width , mode.size.height );
    printf("\n\t native %s" , mode.native? "YES" : "NO ");
    printf("\n\t framerate : %i" , mode.framerate);
    printf("\n\t pixelRatio : %i " , mode.aspectRatio);
    
    init();
    
    
    setReady();
    clearContext();
    
    _delegate->displayDidChange( DISPLAY_ACTIVE );
    
    while ( !threadShouldStop() )
    {

        m_frameRateClock.reset();
        
        if ( needsDisplay() )
        {
            setUpdated();
        }
        
        if (m_shouldClearContext )
        {
            clearContext();
            updateContext();
            m_shouldClearContext = false ;
        }
        
        else if (m_currentElement)
        {
            if ( !m_currentElement->isPrepared() )
            {
                Lock();
                m_currentElement->prepare();
                UnLock();
            }
            
            if ( m_currentElement->shouldBeRemoved() )
            {
                Lock();
                m_currentElement->cleanUp();
                m_currentElement = nullptr;
                UnLock();
                
                break;
            }
            
            if ( m_currentElement->m_changed_flag && m_currentElement->m_callChangedOnGUIThread)
            {
                Lock();
                m_currentElement->changed();
                m_currentElement->m_changed_flag = false;
                UnLock();
                
            }
            
            else //if ( m_currentElement->needsDisplay() )
            {

                if ( threadShouldStop() )
                    break;
                
                Lock();                
                m_currentElement->paint( _updateRect , _anim );
                
                m_currentElement->setUpdated();
                
                UnLock();
                
                updateContext();
                
            }
            
            
        }
        

        m_frameRate = m_frameRateClock.elapsedTimeInMs();



    }
    

    if ( m_currentElement )
    {
        Lock();
        m_currentElement->cleanUp();
        UnLock();
    }
    setUnReady();    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::displayChangeNotification( DisplayNotification notification )
{
    printf("\n Got a notification from display");
    
    if ( notification & HDMI_UNPLUGGED )
        printf("\n HDMI is Unplugged");
    
    if ( notification & HDMI_ATTACHED )
        printf("\n HDMI is Attached");
    
    if ( notification & HDMI_DVI )
        printf("\n HDMI is DVI");
    
    if ( notification & HDMI_HDMI )
        printf("\n HDMI is in HDMI mode and ON");
    
    
    if ( _delegate )
        _delegate->displayDidChange( notification );
    
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool DisplayController::setVideoModeTo( const DisplayInformations &mode)
{

    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*static*/ bool DisplayController::signalStop()
{
    return s_instance->threadShouldStop();
}


void DisplayController::init()
{
    assert( calledFromThisThread() );
    
#ifdef TARGET_RASPBERRY_PI
	int32_t success = 0;
	EGLBoolean result;
	EGLint num_config;

    
    //	VC_RECT_T dst_rect; // GX_EL
    //	VC_RECT_T src_rect;
    
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
	m_EGLdisplay = eglGetDisplay( EGL_DEFAULT_DISPLAY );
	assert( m_EGLdisplay != EGL_NO_DISPLAY );
    
	// initialize the EGL display connection
	result = eglInitialize( m_EGLdisplay, NULL, NULL);
	assert(EGL_FALSE != result);
    
	EGLConfig config;
    
    
    
	// get an appropriate EGL frame buffer configuration
	result = eglChooseConfig( m_EGLdisplay, attribute_list, &config, 1, &num_config);
	assert( EGL_FALSE != result );
    
	// create an EGL rendering context
	m_context = eglCreateContext(m_EGLdisplay, config, EGL_NO_CONTEXT, NULL);
	assert( m_context != EGL_NO_CONTEXT );
    
	// create an EGL window surface
	success = graphics_get_display_size(0 /* LCD */ , &m_screen_width,&m_screen_height);
	assert( success >= 0 );
    

    

    int oriX = 0;
    int oriY = 0;

    
	m_dst_rect.x = oriX;
	m_dst_rect.y = oriY;
	m_dst_rect.width = m_screen_width;
	m_dst_rect.height = m_screen_height;
    
    m_bounds = makeRect(0, 0, m_screen_width, m_screen_height);
    
	m_src_rect.x = oriX;
	m_src_rect.y = oriY;
	m_src_rect.width = m_screen_width << 16;
	m_src_rect.height =m_screen_height << 16;
    
    
    /* Partie déjà dans GXScene*/
	DISPMANX_DISPLAY_HANDLE_T dispman_display; // GXScene
	DISPMANX_UPDATE_HANDLE_T dispman_update;   // GXScene
    
	dispman_display = vc_dispmanx_display_open(0 /* LCD */ );
	dispman_update = vc_dispmanx_update_start(0);
    
    
    //VC_DISPMANX_ALPHA_T alpha = { DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS, 0xff, 0 };
	m_element = vc_dispmanx_element_add( dispman_update,
                                        dispman_display,
                                        getLayer() ,
                                        &m_dst_rect,
                                        0 /*src */ ,
                                        &m_src_rect,
                                        DISPMANX_PROTECTION_NONE,
                                        0 /*alpha */ ,
                                        0 /*clamp */ ,
                                        (DISPMANX_TRANSFORM_T) 0 /*transform */
                                        );
    
	m_nativewindow.element = m_element;
	m_nativewindow.width = m_screen_width;
	m_nativewindow.height = m_screen_height;
    
	vc_dispmanx_update_submit_sync( dispman_update );
    /**/
    
    
	m_surface = eglCreateWindowSurface( m_EGLdisplay, config, &m_nativewindow, NULL);
	assert(m_surface != EGL_NO_SURFACE);
    
	// preserve the buffers on swap
	result = eglSurfaceAttrib( m_EGLdisplay, m_surface, EGL_SWAP_BEHAVIOR, EGL_BUFFER_PRESERVED );
	assert(EGL_FALSE != result);
    
	// connect the context to the surface
	result = eglMakeCurrent( m_EGLdisplay, m_surface, m_surface, m_context );
	assert(EGL_FALSE != result);
    
	// set up screen ratio
	glViewport(0, 0, (GLsizei) m_screen_width, (GLsizei) m_screen_height);

    
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
    
	float ratio = (float) m_screen_width / (float) m_screen_height;
	glFrustumf(-ratio, ratio, -1.0f, 1.0f, 1.0f, 10.0f);
    
#endif /* TARGET_RASPBERRY_PI */
    
}




/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::clearContext()
{
    assert( calledFromThisThread() );
    
#ifdef TARGET_RASPBERRY_PI
	vgClear( 0, 0, m_screen_width, m_screen_height);
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::updateContext()
{
    
    assert( calledFromThisThread() );
    

#ifdef TARGET_RASPBERRY_PI
//    assert( vgGetError() == VG_NO_ERROR );

	eglSwapBuffers(m_EGLdisplay, m_surface);
    
//	assert(eglGetError() == EGL_SUCCESS);
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

float DisplayController::getDisplayRatio()
{
#ifdef TARGET_RASPBERRY_PI
    if ( isHDMIorDVI() )
    {
        m_display_aspect = get_display_aspect_ratio( (HDMI_ASPECT_T) m_current_tv_state.display.hdmi.aspect_ratio);
    }
    
    else
    {
        m_display_aspect = get_display_aspect_ratio( (SDTV_ASPECT_T) m_current_tv_state.display.sdtv.display_options.aspect);
    }
    
    

    return m_display_aspect *= (float) m_current_tv_state.display.hdmi.height / (float) m_current_tv_state.display.hdmi.width;
    
#endif
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
#ifdef TARGET_RASPBERRY_PI
void DisplayController::displayDidChange()
{

    
    memset( &m_current_tv_state, 0, sizeof(TV_DISPLAY_STATE_T));
    vc_tv_get_display_state( &m_current_tv_state );
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool DisplayController::canSupportAudioFormat(EDID_AudioFormat audio_format,
                                              uint32_t num_channels,
                                              EDID_AudioSampleRate fs,
                                              uint32_t bitrate)
{
    return vc_tv_hdmi_audio_supported(audio_format, num_channels, fs, bitrate ) == 0;
}
#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 EGL initialization
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/*static*/ void DisplayController::initializeEGL()
{
    if ( !s_EGLInitialized )
    {
#ifdef TARGET_RASPBERRY_PI

        // bind OpenVG API
        eglBindAPI(EGL_OPENVG_API);
#endif
        s_EGLInitialized = true;
    }
    else
    {
        Log::log("\n EGL already initialized");
    }
    
}


/*static*/ void DisplayController::deinitializeEGL()
{
    if ( s_EGLInitialized )
    {
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
#ifdef TARGET_RASPBERRY_PI
/*static*/ void DisplayController::CallbackTvServiceCallback(void *userdata, uint32_t reason, uint32_t param1, uint32_t param2)
{

    DisplayController *self = reinterpret_cast<DisplayController*>(userdata);
    

    switch(reason)
    {
        case VC_HDMI_UNPLUGGED:
            break;
        case VC_HDMI_STANDBY:
            break;
        case VC_SDTV_NTSC:
        case VC_SDTV_PAL:
        case VC_HDMI_HDMI:
        case VC_HDMI_DVI:
            // Signal we are ready now
            sem_post(&(self->m_tv_synced)/* tv_synced*/);
            //self->displayDidChange();
            
            break;
        default:
            break;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*static*/ float DisplayController::get_display_aspect_ratio(HDMI_ASPECT_T aspect)
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
/*static*/ float DisplayController::get_display_aspect_ratio(SDTV_ASPECT_T aspect)
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
#endif


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void DisplayController::paint( const GXRect &rect, GXAnimation* anim )
{
    
}
void DisplayController::prepareRessources()
{
    
}
void DisplayController::deleteRessources()
{
    
}

void DisplayController::changed()
{
    
}

