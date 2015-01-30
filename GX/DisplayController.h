/*!
 *  \brief     DisplayController
 *  \details   Handles screen operations + holds the displayed element(s).
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __Broadway_test__DisplayController__
#define __Broadway_test__DisplayController__



#ifdef TARGET_RASPBERRY_PI
    #include <bcm_host.h>



/*  FOR OMX ONLY
 
    #include "OMXStreamInfo.h"

    #include "utils/log.h"
    #include "DllAvUtil.h"
    #include "DllAvFormat.h"
    #include "DllAvFilter.h"
    #include "DllAvCodec.h"
    #include "linux/RBP.h"
    #include "OMXVideo.h"
    #include "utils/PCMRemap.h"
    #include "DllOMX.h"
    #include "Srt.h"
    #include "utils/Strprintf.h"
 */

#endif /* TARGET_RASPBERRY_PI */

#include <semaphore.h>

#include <iostream>
#include <string>
#include <utility>



#include "../Internal/AbstractController.h"
#include "../Internal/Thread.h"
#include "../Scheduler/Scheduler.h"
#include "../Plateforms/Plateform.h"

#include "GXDefs.h"

#include "Impl/DisplayImpl.h"

#include "../GXDataType/GXColors.h"
#include "GXElement.h"
#include "GXPath.h"
#include "GXImage.h"



// a virer
typedef enum
{
    CONF_FLAGS_FORMAT_NONE,
    CONF_FLAGS_FORMAT_SBS,
    CONF_FLAGS_FORMAT_TB
    
} FORMAT_3D_T;

class DisplayControllerDelegate
{
    friend class DisplayController;
    
public:
    virtual ~DisplayControllerDelegate()
    {}
    
protected:
    DisplayControllerDelegate()
    {}
    
    virtual void displayDidChange( DisplayNotification notification ) = 0;
    
};


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class DisplayController : public AbstractController,
                          public  Thread,
                          private GXElement
{

    
/* **** **** **** **** **** **** */
// new methods, with DisplayImpl
    
    friend class DisplayImpl;
    
public:
    
    void setDelegate( DisplayControllerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    const std::list< DisplayInformations > getAvailableVideoMode() const
    {
        return _impl.getAvailableVideoMode();
    }
    
    
private:
    void displayChangeNotification( DisplayNotification notification );
    
    DisplayControllerDelegate *_delegate;
    
/* **** **** **** **** **** **** */
public:
    static DisplayController* getController()
    {
        return s_instance;
    }
    
    DisplayController();
    ~DisplayController();

    static bool signalStop();


    
    bool start();
    bool stop();
    
    float getFrameRate() const
    {
        return 1000.f / (float) m_frameRate;
    }
    
    void setDisplayedElement( GXElement* element);
    
    void removeElement( GXElement* element);
    
    void clearScreen();
    void update();
    
    
    bool isBusy() const
    {
        return needsDisplay();
    }
    
    
    bool isNativeDeinterlace() const
    {
        return m_NativeDeinterlace;
    }
    
    float getDisplayRatio(); // -> move to DisplayImpl
    
#ifdef TARGET_RASPBERRY_PI
    
    bool isHDMIorDVI() // -> move to DisplayImpl
    {
        return m_current_tv_state.state & ( VC_HDMI_HDMI | VC_HDMI_DVI );
    }
    
    bool isComposite() // -> move to DisplayImpl
    {
        return !isHDMIorDVI();
    }
    
    // -> move to DisplayImpl
    bool canSupportAudioFormat( EDID_AudioFormat audio_format,
                                uint32_t num_channels,
                                EDID_AudioSampleRate fs,
                                uint32_t bitrate);
    
    void displayDidChange(); // move to private / protected asap
    
    // -> move to DisplayImpl
    static float get_display_aspect_ratio(HDMI_ASPECT_T aspect);
    static float get_display_aspect_ratio(SDTV_ASPECT_T aspect);
    static void CallbackTvServiceCallback(void *userdata, uint32_t reason, uint32_t param1, uint32_t param2);
    
#endif /*TARGET_RASPBERRY_PI*/
    
protected:
    virtual void run();
    void init();

    
    // called on GUI thread!!
    void clearContext();  // will go private
    void updateContext(); // will go private

    
    
    // reimp from GXElement, useless
    virtual void paint( const GXRect &rect );
    virtual void prepareRessources();
    virtual void deleteRessources();
    virtual void changed();
    
    
private:
    
    static DisplayController* s_instance;
    
    DisplayImpl _impl;
    
    // moved in impl
    static void initializeEGL();
    static void deinitializeEGL();
    static bool s_EGLInitialized;
    

    
    GXElement   *m_currentElement;
    
    bool     m_shouldClearContext;
    bool     m_shouldForceUpdate;

    uint32_t m_screen_width;
	uint32_t m_screen_height;
    
    bool    m_NativeDeinterlace;
    sem_t   m_tv_synced;

    
    Chrono    m_frameRateClock;
    long long m_frameRate;

#ifdef HAVE_EGL
    // OpenGL|ES objects
	EGLDisplay m_EGLdisplay; // egl.h
    
	EGLSurface m_surface; // egl.h
	
    // unique
    EGLContext m_context; // egl.h
#endif
    
#ifdef TARGET_RASPBERRY_PI
    EGL_DISPMANX_WINDOW_T m_nativewindow; // rpi


    
    VC_RECT_T  m_dst_rect; // rpi
    VC_RECT_T  m_src_rect; // rpi
    
    DISPMANX_ELEMENT_HANDLE_T   m_element; // rpi
    
    // rpi
    TV_DISPLAY_STATE_T   m_old_tv_state; // store params before init. params are restaured at exit
    TV_DISPLAY_STATE_T   m_current_tv_state; // store params before init. params are restaured at exit
    
    // add for omxplayer
    /*
    DllBcmHost        m_BcmHost; removed -> use directly bcm_host 
    CRBP              m_RBP;
    COMXCore          m_OMX;
    */
    

    
#endif
    
    float             m_display_aspect;

};

#endif /* defined(__Broadway_test__DisplayController__) */
