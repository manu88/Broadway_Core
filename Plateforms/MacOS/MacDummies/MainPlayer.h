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

class GXVideo;

class MainPlayer
{
public:
    MainPlayer( GXVideo *parent) :
    _parent(parent),
    m_volumeAudio(1.f)
    {
        
    }
    ~MainPlayer()
    {
        
    }
    
    bool prepare()
    {
        return true;
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
    
    void flipVisibilityTo( bool visible)
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
    
    unsigned long getCurrentTC()
    {
        return 100;
    }
    
    void seekTo(double timeInS)
    {
        
    }
    
    void registerTC( unsigned long tc)
    {
        
    }
    
    void resetAllRegisteredTC()
    {
        
    }
    
    void showInfosOnScreen( bool show )
    {
        _showInfosOnScreen = show;
    }
    
    bool getInfoOnScreen() const // Todo : better name :-)
    {
        return _showInfosOnScreen;
    }
    
    void signalSourceChange()
    {

    }
    
    void signalSourceChanged()
    {
        
    }
    
private:
    GXVideo *_parent;
    float m_volumeAudio;
    bool _showInfosOnScreen;
    
    
};

#endif /* defined(__Broadway_test__MainPlayer__) */
