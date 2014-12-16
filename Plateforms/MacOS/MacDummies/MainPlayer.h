//
//  MainPlayer.h
//  Broadway_test
//
//  Created by Manuel Deneu on 05/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__MainPlayer__
#define __Broadway_test__MainPlayer__

#include <iostream>

#include "../../../GX/DisplayController.h"


class OMXClock
{
public:
    OMXClock()
    {
        
    };
    
    ~OMXClock()
    {
        
    }
    
    long OMXMediaTime() const
    {
        return 0.0;
    }

    

    
};

class MainPlayer
{
public:
    MainPlayer() :
    m_volumeAudio(1.f)
    {
        
    }
    ~MainPlayer()
    {
        
    }
    
    void prepare()
    {
        
    }
    
    void setDisplayController(DisplayController* controllerToUse)
    {
        
    }
    
    void setClock(OMXClock *clockToUse)
    {
        
    }
    
    void setVideoFileSource( const std::string &filePath )
    {
        
    }
    void setLooped(bool loop )
    {
        
    }
    
    
    
    void setBounds(int x , int y , int w , int h)
    {
        
    }
    
    void setLayerNum( int layer )
    {
        
    }
    
    void start()
    {

    }
    
    void pauseStream()
    {

    }
    
    void stop()
    {

    }
    
    void releasePlayer()
    {

    }
    
    bool isRunning() const
    {
        return false;
    }
    
    bool isPaused() const
    {
        return false;
    }
    
    void setAudioDeviceName (const std::string &device)
    {
        
    }
    
    void setVolume( float vol)
    {
        if ( vol <0)
            vol = 0.f;
        m_volumeAudio = vol;
        
        
    }
    
    float getVolume() const
    {
        return m_volumeAudio;
    }
    
    int getStreamLength() // should go const ..
    {
        return 100;
    }
    
    
    void seekTo(double timeInS)
    {
        
    }
    
private:
    float m_volumeAudio;
    
    
};

#endif /* defined(__Broadway_test__MainPlayer__) */
