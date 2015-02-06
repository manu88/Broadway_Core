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
#include <vector>



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
    
    DisplayController();
    ~DisplayController();
    
    void setDelegate( DisplayControllerDelegate *delegate)
    {
        _delegate = delegate;
    }
    
    inline const std::vector< DisplayInformations > getAvailableVideoMode() const
    {
        return _impl.getAvailableVideoMode();
    }
    
    inline const DisplayInformations getCurrentMode() const
    {
        return _impl.getCurrentDisplayInformations();
    }
    
    bool setVideoModeTo( const DisplayInformations &mode);
    
    bool start();
    bool stop();
    
    /* power */
    
    bool powerOn()
    {
        _delegate->displayDidChange( HDMI_ATTACHED );
        
         _displayIsOn = true;
        
        return true;
    }
    
    bool powerOff()
    {
        _delegate->displayDidChange( HDMI_UNPLUGGED );
        
         _displayIsOn = false;
        
        return true;
    }
    
    bool isDisplayOn() const
    {
        return _displayIsOn;
    }
    
    /* Displayed Element's management */
    
    void setDisplayedElement( GXElement* element);
    
    void removeElement( GXElement* element);
    
    void clearScreen();
    void update();
    
    
private:
    void displayChangeNotification( DisplayNotification notification );
    
    DisplayImpl _impl;
    
    bool _displayIsOn;
    DisplayControllerDelegate *_delegate;
    
    GXElement   *m_currentElement;
    
/* **** **** **** **** **** **** */
/* **** **** **** **** **** **** */
    
public:

    static DisplayController* getController()
    {
        return s_instance;
    }


    
    float getFrameRate() const
    {
        return 1000.f / (float) m_frameRate;
    }
    

    
    bool isBusy() const
    {
        return needsDisplay();
    }
    
private:
    void run();
    void init();

    
    // called on GUI thread!!
    void clearContext();  // will go private
    void updateContext(); // will go private

    
    
    // reimp from GXElement, useless
    void paint( const GXRect &rect, GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
    

    
    static DisplayController* s_instance;
    



    
    bool     m_shouldClearContext;
    bool     m_shouldForceUpdate;
    
    Chrono    m_frameRateClock;
    long long m_frameRate;


    // add for omxplayer
    /*
#ifdef TARGET_RASPBERRY_PI
    
    
    DllBcmHost        m_BcmHost; removed -> use directly bcm_host 
    CRBP              m_RBP;
    COMXCore          m_OMX;

    
#endif
    */    

};

#endif /* defined(__Broadway_test__DisplayController__) */
