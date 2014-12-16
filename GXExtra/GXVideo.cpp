//
//  GXVideo.cpp
//  
//
//  Created by Manuel Deneu on 13/08/14.
//
//

#include "GXVideo.h"
#include "../Log/Log.h"

GXVideo::GXVideo() :
    m_isPrepared ( false ),
    m_length     ( Timecode() )
{
    className = "GXVideo";
    
    // no need to access vars on GUI thread
    performChangedSignalOnGUIThread( false );
    
    m_player.setClock( &m_clock );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXVideo::~GXVideo()
{
    m_player.stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::prepareRessources()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::deleteRessources()
{
    if ( isStarted() )
        stop();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::changed()
{
    m_player.setBounds(getBounds().origin.x, getBounds().origin.y,  getBounds().size.width, getBounds().size.height);
    
    m_player.setLayerNum( getLayer() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::paint( const GXRect &rect )
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::setAudioOutput( const AUDIO_OUTPUT &output)
{
    m_audioOutput = output;

    
    if (m_audioOutput == AUDIO_BOTH)
        m_player.setAudioDeviceName("omx:both");
    
    else if (m_audioOutput == AUDIO_HDMI)
        m_player.setAudioDeviceName("omx:hdmi");
    
    else if (m_audioOutput == AUDIO_LOCAL)
        m_player.setAudioDeviceName("omx:local");
    
    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

Timecode GXVideo::getVideoLength()
{
    const int dur = m_player.getStreamLength();

    m_length.h     = 0;
    m_length.min   = (dur/60)%60 ; // dur/3600;
    m_length.sec   = dur%60;
    m_length.milli = 0;

    return m_length;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::seekToTC( Timecode tc )
{
    printTime( tc );

    printTime( m_length);
    if ( tc > m_length )
    {
        Log::log("\n seek excess video lenght");
        return;
    }
    double tcInS = tc.getInMs() / 1000.0f;
    
    m_player.seekTo( tcInS );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Geometry related
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
void GXVideo::setBounds( int x, int y , int width , int height)
{

    m_bounds = makeRect(x, y, width ,  height);
    
    m_player.setBounds(m_bounds.origin.x, m_bounds.origin.y,  m_bounds.size.x, m_bounds.size.y);
    
}



void GXVideo::setBounds( GXRect bounds )
{

    m_bounds = bounds;
    m_player.setBounds(m_bounds.origin.x, m_bounds.origin.y,  m_bounds.size.x, m_bounds.size.y);    
}



void GXVideo::moveOf( int dX , int dY)
{

    m_bounds += makePoint(dX, dY);

    m_player.setBounds(m_bounds.origin.x, m_bounds.origin.y,  m_bounds.size.x, m_bounds.size.y);
    
}



void GXVideo::moveTo( int x  , int y)
{

    m_bounds.origin = makePoint(x, y);
    
    m_player.setBounds(m_bounds.origin.x, m_bounds.origin.y,  m_bounds.size.x, m_bounds.size.y);
}



void GXVideo::setSize( int width , int height)
{

    m_bounds.size = makePoint(width, height);
    
    m_player.setBounds(m_bounds.origin.x, m_bounds.origin.y,  m_bounds.size.x, m_bounds.size.y);    
}

*/

