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

#ifdef TARGET_RASPBERRY_PI
    #include "OMX/MainPlayer.h"

#else
    #include "../Plateforms/MacOS/MacDummies/MainPlayer.h"
#endif

typedef enum
{
    AUDIO_NONE    = -1,
    AUDIO_HDMI    = 0,
    AUDIO_LOCAL   = 1,
    AUDIO_BOTH    = 2
    
} AUDIO_OUTPUT;


typedef enum
{
    VideoLoaded     = 0,
    VideoWillStart  = 1,
    VideoDidStart   = 2,
    VideoPaused     = 3,
    VideoDidResume  = 4,
    VideoWillStop   = 5, // sent when video demuxer is end-of-stream
    VideoDidStop    = 6,
    VideoDidReachTC = 7, // notif fired when a registered TC is reached
} GXVideoNotification;

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
    void setSchedulerDelegate( SchedulerDelegate *delegate)
    {
        _schedulerDelegate = delegate;
    }
    
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
        if (! m_isPrepared )
        {
            m_player.prepare();
            m_isPrepared = true;
        }
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
    
    void start()
    {
        if ( prepare() )
            m_player.start();
    }
    
    void pause()
    {
        m_player.pauseStream();
    }
    
    void stop()
    {
        m_player.stop();
//        releasePlayer();
    }
    
    void seekToTC( Timecode tc );
    
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
    
protected:
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
    /* Called by mainPlayer */
    
    void willStart();
    void didStart();
    
    void didPause();
    void didResume();
    
    void willEnd();
    void didEnd();
    
    void didReachTC( unsigned long millis);
    
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
