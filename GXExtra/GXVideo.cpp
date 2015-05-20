//
//  GXVideo.cpp
//  
//
//  Created by Manuel Deneu on 13/08/14.
//
//

#include "GXVideo.h"
#include "../Log/Log.h"

#include "../Internal/Thread.h"

GXVideo::GXVideo() :
    Event   ( Event_Video ),
    m_player     ( this   ),
    m_isPrepared ( false  ),
    m_length     ( Timecode() )
{
    className = "GXVideo";
    
    // no need to access vars on GUI thread
    performChangedSignalOnGUIThread( false );
    
    m_player.setClock( &m_clock );
    
    setVisible( true);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXVideo::~GXVideo()
{
    stop();
    
    if (m_isPrepared )
        releasePlayer();
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
    
    m_player.flipVisibilityTo( isVisible() );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::paint( const GXRect &rect , GXAnimation* anim)
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

    m_length.h     = dur/3600;
    m_length.min   = (dur/60)%60 ; // dur/3600;
    m_length.sec   = dur%60;
    m_length.milli = 0;

    return m_length;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::seekToTC( Timecode tc )
{

    if ( tc > m_length )
    {
        return;
    }
    double tcInS = tc.getInMs() / 1000.0f;
    
    m_player.seekTo( tcInS );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::setSpeed( float speed )
{
    m_player.setSpeedAsync( (int ) speed );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

float GXVideo::getSpeed() const
{
    return 1.0f;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::registerTCNotification( const Timecode &tc )
{
    m_player.registerTC (tc.getInMs() );
}

void GXVideo::resetAllTCNotification()
{
    m_player.resetAllRegisteredTC();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXVideo::changeVideoFileTo( const std::string &file )
{
    willChange();
    
    //Thread::sleepForMs( 1000 );
    
    releasePlayer();
    
    //Thread::sleepForMs( 1000 );
    
    setVideoFileSource( file  );
    

    
    //Thread::sleepForMs( 1000 );
    
    if (!prepare() )
        Log::log("PREPARE VIDEO IN CHANGE FILE ___PAS____ OK \n");

    //Thread::sleepForMs( 1000 );
    
    didChange();
    
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    Callback from MainPlayer to SchedulerDelegate
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXVideo::sig_ready()
{
    _notif = VideoLoaded;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}

void GXVideo::sig_willStart()
{
    _notif = VideoWillStart;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
    
}

void GXVideo::sig_didStart()
{
    _notif = VideoDidStart;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}

void GXVideo::sig_didPause()
{
    _notif = VideoPaused;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}

void GXVideo::sig_didResume()
{
    _notif = VideoDidResume;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}

void GXVideo::sig_willEnd()
{

    _notif = VideoWillEnd;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}
void GXVideo::sig_didEnd()
{
    _notif = VideoDidEnd;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);

}

void GXVideo::sig_didReachTC( unsigned long millis)
{
    _notif = VideoDidReachTC;
    
    if (_schedulerDelegate )
        _schedulerDelegate->scheduledEventReceived( *this);
}

#ifdef ENABLE_ELEMENT_SELECTOR
const Variant GXVideo::performSelectorWithArguments( const std::string &selector , const VariantList  &arguments , bool *validCall /*= nullptr*/)
{
    if ( validCall )
        *validCall = true;
    if( selector == "getCurrentTC")
        return (double) getCurrentTC().getInMs();
    
    else if( selector == "setVideoFileSource")
        return setVideoFileSource( arguments.at(0).getString() );
    
    else if( selector == "setLooped")
        setLooped( arguments.at(0).getBool() );
    
    else if ( selector == "setSpeed")
        setSpeed( arguments.at(0).getFloat() );
    
    else if( selector == "start")
        return start();
    
    else if( selector == "pause")
        pause();
    
    else if( selector == "stop")
        stop();
    
    else if( selector == "isStarted")
        return isStarted();
    
    else if( selector == "isPaused")
        return isPaused();
    
    if ( validCall )
        *validCall = false;
    
    
    return Element::performSelectorWithArguments(selector, arguments , validCall );
}

#endif

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

