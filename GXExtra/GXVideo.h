//
//  GXVideo.h
//  
//
//  Created by Manuel Deneu on 13/08/14.
//
//

#ifndef ____GXVideo__
#define ____GXVideo__

#include <iostream>
#include "../GX/GXElement.h"

#include "../GX/DisplayController.h"

#include "../Scheduler/Timecode.h"
#include "../Scheduler/Event.h"

#include "../Internal/FileSystem.h"

/* **** **** **** **** **** **** **** **** **** **** **** */
// Actual implementation.

// On raspberry Pi, uses OMXPlayer
// see https://github.com/popcornmix/omxplayer

#ifdef TARGET_RASPBERRY_PI
    #include "OMX/MainPlayer.h"


// Dummy impl on Mac OS !
#else
    #include "../Plateforms/MacOS/MacDummies/MainPlayer.h"


#endif

/* **** **** **** **** */

//! Audio output representation
/*
    Todo : create audioGraph processor, add alsa support for external sound cards, ...
 */
typedef enum
{
    AUDIO_NONE    = -1,
    AUDIO_HDMI    = 0,
    AUDIO_LOCAL   = 1,
    AUDIO_BOTH    = 2
    
} AUDIO_OUTPUT;

/* **** **** **** **** */

//! Notifications sent to SchedulerDelegate instance ( optionnal ).
/*
    These notifications are async and sent when possible :
    Its garranted that they will be fired on time, or possibly after a short delay, but _NEVER_ before the event! 
 */
typedef enum
{
    VideoLoaded     = 0, // Sent after the player has been loaded and ready to roll.
    VideoWillStart  = 1, // Sent when player enters playing loop.
    VideoDidStart   = 2, // Sent when demuxer sent first frames.
    VideoPaused     = 3, // Sent when pause command performed.
    VideoDidResume  = 4, // Sent when resume command performed.
    VideoWillEnd   = 5, // Sent when video demuxer is end-of-stream.
    VideoDidEnd    = 6, // Sent when player did stop, i.e after flushing last frames.
    VideoDidReachTC = 7, // Notif fired when a registered TC is reached.
} GXVideoNotification;

/* **** **** **** **** */

class SchedulerDelegate;

class GXVideo : public GXElement, public Event
{
public:
    
    friend class MainPlayer;
    GXVideo();
    ~GXVideo();
    
    /* **** **** **** **** **** **** **** **** */
    // config
    
    void setDisplayController(DisplayController* controllerToUse)
    {
        m_player.setDisplayController( controllerToUse );
    }
    
    //! register a SchedulerDelegate for GXVideoNotification. Optionnal.
    void setSchedulerDelegate( SchedulerDelegate *delegate)
    {
        _schedulerDelegate = delegate;
    }
    
    /* Player options */
    
    bool setVideoFileSource( const std::string &filePath )
    {
        if (!FileSystem::fileExists( filePath ))
            return false;
        
        m_player.setVideoFileSource( filePath );
        
        return true;
        
    }
    

    
    void setLooped(bool loop )
    {
        m_player.setLooped( loop );
    }
    

    bool prepare()
    {

        m_isPrepared = m_player.prepare();
        getVideoLength();
        
        return m_isPrepared;
    }
    
    void releasePlayer()
    {
        m_player.releasePlayer();
        m_isPrepared = false;
        
        
    }
    
    /* **** **** **** **** **** **** **** **** */
    // telecommand
    
    bool start()
    {
//        if ( m_isPrepared )
            
        m_player.start();
        return m_isPrepared;
    }
    
    void pause()
    {
        m_player.pauseStream();
    }
    
    void stop()
    {
        m_player.stop();
//        releasePlayer();
        
        m_isPrepared = false;
    }
    
    void seekToTC( Timecode tc );
    
    void setSpeed( float speed );
    float getSpeed() const;
    
    /* **** **** **** **** **** **** **** **** */
    // accessors
    
    bool isStarted() const
    {
        return m_player.isRunning();
    }
    
    bool isPaused() const
    {
        return m_player.isPaused();
    }

    Timecode getVideoLength();
    
    long getMediaTime()// const
    {
        return m_clock.OMXMediaTime();
    }
    
    Timecode getCurrentTC()
    {
        return Timecode(0,0,0, (unsigned int) m_player.getCurrentTC() );
    }
    
    

    void registerTCNotification( const Timecode &tc);
    void resetAllTCNotification();
    
    void showInfosOnScreen( bool show )
    {
        m_player.showInfosOnScreen( show );
    }

    
    // audio
    void setAudioOutput( const AUDIO_OUTPUT &output);

    void setVolume( float vol)
    {
        m_player.setVolume( vol );
    }
    
    float getVolume() const
    {
        return m_player.getVolume();
    }
    
    /**/
    
    GXVideoNotification getNotification() const
    {
        return _notif;
    }

    
    /**/
    
    //! Tell the demux. to stop running so changes can be performed
    /*
        streams will be flushed.
     */
    void willChange()
    {
        m_player.signalSourceChange();
    }

    //! Tell the demux. to start running again after a reinitialization.
    void didChange()
    {
        m_player.signalSourceChanged();
    }
    
    
    /* Revisited interface */
    
    bool changeVideoFileTo( const std::string &file );
    
#ifdef ENABLE_ELEMENT_SELECTOR
    const Variant performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall = nullptr) ;
#endif
    
protected:
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
    /* Called by mainPlayer */
    
    void sig_ready();
    void sig_willStart();
    void sig_didStart();
    
    void sig_didPause();
    void sig_didResume();
    
    void sig_willEnd();
    void sig_didEnd();
    
    void sig_didReachTC( unsigned long millis);
    
    GXVideoNotification _notif;
    
private:
    SchedulerDelegate *_schedulerDelegate;
    
    AUDIO_OUTPUT m_audioOutput;
    MainPlayer   m_player;
    bool         m_isPrepared;
    
    Timecode     m_length;
    
    OMXClock     m_clock;
    
    
};


#endif /* defined(____GXVideo__) */
