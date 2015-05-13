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

#endif /* TARGET_RASPBERRY_PI */

#include <semaphore.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>



#include "../Internal/AbstractController.h"
#include "../Internal/AbstractDelegate.h"
#include "../Internal/Thread.h"
#include "../Scheduler/Scheduler.h"
#include "../Plateforms/Plateform.h"

#include "../Data/Database.h"

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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class DisplayControllerDelegate : public virtual AbstractDelegate
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
    
    bool start();
    bool stop();
    
    /* Modes & infos */
    
    inline const std::vector< DisplayInformations > getAvailableVideoMode() const
    {
        return _impl.getAvailableVideoMode();
    }
    
    inline const DisplayInformations getCurrentMode() const
    {
        return _impl.getCurrentDisplayInformations();
    }
    
    inline bool setVideoModeTo( const DisplayInformations &mode)
    {
        return _impl.setVideoModeTo( mode );
    }
    
    inline int getAudioVideoLatency() const
    {
        return _impl.getAudioVideoLatency();
    }
    
    /* Load/save config */

    void setSaveOnFile( bool save)
    {
        _shouldSaveConfigOnFile = save;
    }
    
    bool saveActivated() const
    {
        return _shouldSaveConfigOnFile;
    }
    
    void setSaveFile( const std::string &file)
    {
        _fileConfig = file;
    }
    
    const std::string &getSaveFile() const
    {
        return _fileConfig;
    }
    
    
    /* Commands to send to the display */
    
    inline bool showInfosOnDisplay( bool show ) const
    {
        return _impl.showInfosOnDisplay( show );
    }
    
    bool powerOn()
    {
        if (_impl.sendTvPowerOn() )
            _displayIsOn = true;
        
        return _displayIsOn;
    }
    
    bool powerOff()
    {
        if ( _impl.sendTvPowerOff() )
            _displayIsOn = false;
        
        return _displayIsOn;
    }
    
    bool isDisplayOn() const
    {
        return _displayIsOn;
    }
    
    /* Displayed Element's management */
    
    void setDisplayedElement( GXElement* element);
    
    void removeElement( GXElement* element);
    
    void setCleanColor( GXColor color);
    void clearScreen();
    void update();
    
    /* adds for OMX */
    
    
    bool isNativeDeinterlace() const
    {
        return true;
    }
    
    /* Utils */
    
    bool saveCurrentConfiguration( const std::string &file) const;
    bool loadConfigurationFile( const std::string &file);
    
    static Database getDisplayInformationsAsDatabase( const DisplayInformations &info);
    static DisplayInformations   getDisplayInformationsFromDatabase( Database &data);
    
    
    //! Will write on logging system infos regarding the currentlt implemented GXGraphics functions
    static void dumpImplementation();

    
private:
    void displayChangeNotification( DisplayNotification notification );
    
    bool saveConfigFile() const;
    bool loadConfigFile();
    

    DisplayImpl _impl;
    
    DisplayInformations _currentMode;
    
    bool _displayIsOn;
    DisplayControllerDelegate *_delegate;

    GXColor _clearColor; /* later use GXElements' backgroundColor*/
    
    GXElement   *m_currentElement;
    
/* **** **** **** **** **** **** */
/* **** **** **** **** **** **** */
    
public:

    /*
     Todo : Seulement utilisé par GXelement -> peut être remplacé par un remontée de getParent() 
     jusqu'au DisplayController, qui est l'élément au sommet de l'arborescence
     */
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

    /* File config load/save */
    
    bool        _shouldSaveConfigOnFile;
    std::string _fileConfig;
};

#endif /* defined(__Broadway_test__DisplayController__) */
