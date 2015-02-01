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

#include "../Internal/FileSystem.h"

#ifdef TARGET_RASPBERRY_PI
    #include "../OMX/MainPlayer.h"

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

class GXVideo : public GXElement
{
public:
    GXVideo();
    ~GXVideo();
    
    /* **** **** **** **** **** **** **** **** */
    // config
    
    void setDisplayController(DisplayController* controllerToUse)
    {
        m_player.setDisplayController( controllerToUse );
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
    
protected:
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    void changed();
    
private:
    AUDIO_OUTPUT m_audioOutput;
    MainPlayer   m_player;
    bool         m_isPrepared;
    
    Timecode     m_length;
    
    OMXClock     m_clock;
    
    
};


#endif /* defined(____GXVideo__) */
