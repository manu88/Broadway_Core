//
//  Broadway.cpp
//  
//
//  Created by Manuel Deneu on 26/07/14.
//
//

extern "C"
{
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
};

#include "MainPlayer.h"
#include "utils/Strprintf.h"

#include "../GXVideo.h"

volatile sig_atomic_t MainPlayer::g_abort           = false;

/* **** **** **** **** **** **** **** **** **** **** **** */

MainPlayer::MainPlayer( GXVideo *parent) :
    _parent               ( parent ),
    m_filename            ( ""      ),
    m_displayController   ( nullptr ),
    m_seek_flush          ( false ),
    m_incr                ( 0     ),

    m_new_win_pos         ( false ),

    _showInfosOnScreen ( false ),

    m_video_queue_size    ( 0.0f  ),
    m_video_fifo_size     ( 0.0f  ),
    m_live                ( false ),
    m_latency             ( 0.0f  ),
    m_shouldPause         ( false ),
    m_isRunning           ( false ),
    _shouldFlipVisibility ( false ),
    m_pChannelMap         ( NULL  ),
    m_3d                  ( CONF_FLAGS_FORMAT_NONE ),
    m_passthrough         ( false ),
    m_stats               ( false ),
    m_volumeAudio         ( 1.f   ),
//    m_Volume              ( 0     ),
    m_Amplification       ( 0     ),
    m_Deinterlace         ( false ),
    m_NoDeinterlace       ( false ),
    m_anaglyph            ( OMX_ImageFilterAnaglyphNone ),
    m_HWDecode            ( false ),

    deviceString          ( "omx:both" ),

    m_use_hw_audio        ( false ),
    m_osd                 ( true  ),
    

    m_has_external_subtitles ( false ),
    
    m_font_path           ( "/usr/share/fonts/truetype/freefont/FreeSans.ttf"        ),
    m_italic_font_path    ( "/usr/share/fonts/truetype/freefont/FreeSansOblique.ttf" ),
    m_dbus_name           ( "org.mpris.MediaPlayer2.omxplayer"                       ),
    
    m_asked_for_font      ( false   ),
    m_asked_for_italic_font ( false ),
    m_font_size           ( 0.055f  ),
    m_centered            ( false   ),
    m_ghost_box           ( true    ),
    m_subtitle_lines      ( 3       ),
    m_Pause               ( false   ),
    

    
    m_audio_index_use     ( 0       ),
    m_thread_player       ( true    ),
    m_av_clock            ( nullptr ),

    m_omx_pkt             ( NULL    ),
    
    m_hdmi_clock_sync     ( false   ),
    m_no_hdmi_clock_sync  ( false   ),
    
    m_stop                ( false   ),
    
    m_subtitle_index      ( -1      ),
    
    
    m_tv_show_info        ( 0       ),
    
    m_has_video           ( false   ),
    m_has_audio           ( false   ),
    m_has_subtitle        ( false   ),
    
    
    m_boost_on_downmix    ( true    ),
    m_loop                ( false   ),
    m_layer               ( 0       )
{
    
    signal(SIGSEGV, sig_handler);
    signal(SIGABRT, sig_handler);
    signal(SIGFPE, sig_handler);
    signal(SIGINT, sig_handler);



    

    //Create(); -> moved to start()
}

/* **** **** **** **** **** **** **** **** **** **** **** */

MainPlayer::~MainPlayer()
{
//    StopThread();
    _registeredTCNotif.clear();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::start()
{

    if (!m_isRunning)
    {
        m_isRunning = true;
        Create(); // starts the thread
    }
    else
        printf("\n video is already running");
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::stop()
{
    
    m_stop = true;
    m_isRunning = false;
    m_incr = 0;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::seekTo(double timeinS)
{

//    double newPos = 0;//(int64_t)result.getArg() * 1e-6;
    double oldPos = m_av_clock->OMXMediaTime()*1e-6;
    m_incr = timeinS - oldPos;
    
    resetAllRegisteredTC();
}

void MainPlayer::setBounds(int x , int y , int w , int h)
{
    m_destRect.x1 = x;
    m_destRect.y1 = y;
    
    m_destRect.x2 = x + w;
    m_destRect.y2 = y + h;
    
    m_has_video = true;
    m_new_win_pos = true;
    m_seek_flush = true;
    
}

void MainPlayer::setLayerNum( int layer )
{
    m_layer = layer;
}
/*
sscanf(result.getWinArg(), "%f %f %f %f", &DestRect.x1, &DestRect.y1, &DestRect.x2, &DestRect.y2);
m_has_video = true;
new_win_pos = true;
seek_flush = true;
break;
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::SetSpeed( int iSpeed )
{
    if( !m_av_clock )
        return;
    
    m_omx_reader.SetSpeed(iSpeed);
    
    // flush when in trickplay mode
    if (TRICKPLAY(iSpeed) || TRICKPLAY(m_av_clock->OMXPlaySpeed()))
        FlushStreams(DVD_NOPTS_VALUE);
    
    m_av_clock->OMXSetSpeed(iSpeed);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
void MainPlayer::FlushStreams(double pts)
{
    m_av_clock->OMXStop();
    m_av_clock->OMXPause();
    
    if(m_has_video)
        m_player_video.Flush();
    
    if(m_has_audio)
        m_player_audio.Flush();
    
    if(pts != DVD_NOPTS_VALUE)
        m_av_clock->OMXMediaTime(0.0);
    
    /*
    if(m_has_subtitle)
        m_player_subtitles.Flush();
    */
    if(m_omx_pkt)
    {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/* Static*/ void MainPlayer::sig_handler(int s)
{
    if (s==SIGINT && !g_abort)
    {
        signal(SIGINT, SIG_DFL);
        g_abort = true;
        return;
    }
    signal(SIGABRT, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGFPE, SIG_DFL);

    abort();
}



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

bool MainPlayer::prepare()
{

    if (m_isRunning)
    {
        printf("\n can't call prepare() when video is playing... ");
        return false;
    }
    bool                  dump_format         = false;
    
    bool                  refresh             = false ;
    
    float audio_fifo_size  = 0.0; // zero means use default

    float audio_queue_size = 0.0;

    float threshold      = -1.0f; // amount of audio/video required to come out of buffering
    float timeout        = 10.0f; // amount of time file/network operation can stall for before timing out
    int   orientation      = -1; // unset
    float fps            = 0.0f; // unset
    
    
    enum PCMLayout m_layout = PCM_LAYOUT_2_0;
    
    std::string mode;
    
//    //Build default keymap just in case the --key-config option isn't used
//    map<int,int> keymap = KeyConfig::buildDefaultKeymap();
    
    
    
    auto PrintFileNotFound = [](const std::string& path)
    {
        printf("File \"%s\" not found.\n", path.c_str());
    };
    
    bool filename_is_URL = IsURL( m_filename );
    
    if(!filename_is_URL && !IsPipe( m_filename ) && !Exists( m_filename ))
    {
        PrintFileNotFound( m_filename );
        return false;
    }
    
    if(m_asked_for_font && !Exists(m_font_path))
    {
        PrintFileNotFound(m_font_path);
        return false;
    }
    
    if(m_asked_for_italic_font && !Exists(m_italic_font_path))
    {
        PrintFileNotFound(m_italic_font_path);
        return false;
    }
    
    if(m_has_external_subtitles && !Exists(m_external_subtitles_path))
    {
        PrintFileNotFound(m_external_subtitles_path);
        return false;
    }
    
    if(!m_has_external_subtitles && !filename_is_URL)
    {
        auto subtitles_path = m_filename.substr(0, m_filename.find_last_of(".")) +
        ".srt";
        
        if(Exists(subtitles_path))
        {
            m_external_subtitles_path = subtitles_path;
            m_has_external_subtitles = true;
        }
    }
    
    bool m_audio_extension = false;
    
    const CStdString m_musicExtensions = ".nsv|.m4a|.flac|.aac|.strm|.pls|.rm|.rma|.mpa|.wav|.wma|.ogg|.mp3|.mp2|.m3u|.mod|.amf|.669|.dmf|.dsm|.far|.gdm|"
    ".imf|.it|.m15|.med|.okt|.s3m|.stm|.sfx|.ult|.uni|.xm|.sid|.ac3|.dts|.cue|.aif|.aiff|.wpl|.ape|.mac|.mpc|.mp+|.mpp|.shn|.zip|.rar|"
    ".wv|.nsf|.spc|.gym|.adx|.dsp|.adp|.ymf|.ast|.afc|.hps|.xsp|.xwav|.waa|.wvs|.wam|.gcm|.idsp|.mpdsp|.mss|.spt|.rsd|.mid|.kar|.sap|"
    ".cmc|.cmr|.dmc|.mpt|.mpd|.rmt|.tmc|.tm8|.tm2|.oga|.url|.pxml|.tta|.rss|.cm3|.cms|.dlt|.brstm|.wtv|.mka";
    
    if ( m_filename.find_last_of(".") != string::npos)
    {
        CStdString extension = m_filename.substr( m_filename.find_last_of("."));
        
        if (!extension.IsEmpty() && m_musicExtensions.Find(extension.ToLower()) != -1)
            m_audio_extension = true;
        
    }

    
    else
    {
        CLog::SetLogLevel(LOG_LEVEL_NONE);
    }
    
    
    
    //      blank_background( true );
    
    
    
    int gpu_mem = get_mem_gpu();
    int min_gpu_mem = 64;
    if (gpu_mem > 0 && gpu_mem < min_gpu_mem)
        printf("Only %dM of gpu_mem is configured. Try running \"sudo raspi-config\" and ensure that \"memory_split\" has a value of %d or greater\n", gpu_mem, min_gpu_mem);
    

    
    m_omxcontrol.init(m_av_clock, &m_player_audio, &m_player_subtitles, &m_omx_reader, m_dbus_name);
    


    
    m_thread_player = true;
    
    if(!m_omx_reader.Open( m_filename.c_str(), dump_format, m_live, timeout))
        return false;
    
    if( dump_format )
        return false;
    
    m_has_video     = m_omx_reader.VideoStreamCount();
    m_has_audio     = m_audio_index_use < 0 ? false : m_omx_reader.AudioStreamCount();
    m_has_subtitle  = false; //m_has_external_subtitles || m_omx_reader.SubtitleStreamCount();
    m_loop          = m_loop && m_omx_reader.CanSeek();
    
    if (m_audio_extension)
    {
        CLog::Log(LOGWARNING, "%s - Ignoring video in audio filetype:%s", __FUNCTION__,  m_filename.c_str());
        m_has_video = false;
    }
    
    if(  m_filename.find("3DSBS") != string::npos || m_filename.find("HSBS") != string::npos)
        m_3d = CONF_FLAGS_FORMAT_SBS;
    
    else if( m_filename.find("3DTAB") != string::npos || m_filename.find("HTAB") != string::npos)
        m_3d = CONF_FLAGS_FORMAT_TB;
    
    // 3d modes don't work without switch hdmi mode
    if (   m_3d != CONF_FLAGS_FORMAT_NONE
        || m_displayController->isNativeDeinterlace()
       )
        refresh = true;
    
    // you really don't want want to match refresh rate without hdmi clock sync
    if (    ( refresh || m_displayController->isNativeDeinterlace() )
        &&  !m_no_hdmi_clock_sync
       )
        m_hdmi_clock_sync = true;
    
    if(!m_av_clock->OMXInitialize())
        return false;
    
    if(m_hdmi_clock_sync && !m_av_clock->HDMIClockSync())
        return false;
    
    m_av_clock->OMXStateIdle();
    m_av_clock->OMXStop();
    m_av_clock->OMXPause();
    
    m_omx_reader.GetHints(OMXSTREAM_AUDIO, m_hints_audio);
    m_omx_reader.GetHints(OMXSTREAM_VIDEO, m_hints_video);
    
    // le fps a été passé en arg
    if ( fps > 0.0f )
        m_hints_video.fpsrate = fps * DVD_TIME_BASE, m_hints_video.fpsscale = DVD_TIME_BASE;
    
    if(m_audio_index_use > 0)
        m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, m_audio_index_use-1);
    
//    m_displayController->storePreviousParameters();
    
    
    /*  ------- INIT Video part -----------------------------*/
    if(m_has_video && refresh)
    {

        
    //    m_displayController->setVideoMode(m_hints_video.width, m_hints_video.height, m_hints_video.fpsrate, m_hints_video.fpsscale, m_3d);
    }
    
    
    //m_displayController->displayDidChange();
    
    
    
    
    m_destRect = { 0 , 0 , 0 , 0 };
    
    
    
    printf("\n MainPlayer Size %i %i " , m_hints_video.width, m_hints_video.height);
    if (orientation >= 0)
        m_hints_video.orientation = orientation;
    
    if(    m_has_video && !openVideoPlayer() )
    {
        
        return false;
    }
    
    if( /*m_has_subtitle || */m_osd)
    {
        std::vector<Subtitle> external_subtitles;
        if(m_has_external_subtitles &&
           !ReadSrt(m_external_subtitles_path, external_subtitles))
        {
            puts("Unable to read the subtitle file.");
            return false;
        }
        
        if(!m_player_subtitles.Open(m_omx_reader.SubtitleStreamCount(),
                                    std::move(external_subtitles),
                                    m_font_path,
                                    m_italic_font_path,
                                    m_font_size,
                                    m_centered,
                                    m_ghost_box,
                                    m_subtitle_lines,
                                    0,
                                    m_layer + 1,
                                    m_av_clock))
            return false;
    }
    
    /*
    if(m_has_subtitle)
    {
        if(!m_has_external_subtitles)
        {
            if(m_subtitle_index != -1)
            {
                m_player_subtitles.SetActiveStream( std::min(m_subtitle_index, m_omx_reader.SubtitleStreamCount()-1) );
            }
            
            m_player_subtitles.SetUseExternalSubtitles(false);
        }
        
        if(m_subtitle_index == -1 && !m_has_external_subtitles)
            m_player_subtitles.SetVisible(false);
        
    }
    */
    
    m_omx_reader.GetHints(OMXSTREAM_AUDIO, m_hints_audio);
    
    
    /* --------- config AUDIO -----------------*/
    /*
    if (deviceString == "")
    {
        if (m_displayController->canSupportAudioFormat( EDID_AudioFormat_ePCM , 2 , EDID_AudioSampleRate_e44KHz , EDID_AudioSampleSize_16bit ) )
            deviceString = "omx:hdmi";
        
        else
            deviceString = "omx:local";
    }
    
    if (  (m_hints_audio.codec == CODEC_ID_AC3 || m_hints_audio.codec == CODEC_ID_EAC3)
        && m_displayController->canSupportAudioFormat(EDID_AudioFormat_eAC3, 2, EDID_AudioSampleRate_e44KHz, EDID_AudioSampleSize_16bit )
        )
        m_passthrough = false;
    
    if (    m_hints_audio.codec == CODEC_ID_DTS
        && m_displayController->canSupportAudioFormat(EDID_AudioFormat_eDTS, 2, EDID_AudioSampleRate_e44KHz, EDID_AudioSampleSize_16bit )
        )
        m_passthrough = false;
     */
    deviceString = "omx:both";

    if(    m_has_audio
       && !m_player_audio.Open ( m_hints_audio ,
                                m_av_clock,
                                &m_omx_reader,
                                deviceString,
                                m_passthrough,
                                m_use_hw_audio,
                                m_boost_on_downmix,
                                m_thread_player,
                                m_live,
                                m_layout,
                                audio_queue_size,
                                audio_fifo_size
                                )
       )
        return false;
    
    if( m_has_audio )
    {
        m_player_audio.SetVolume( m_volumeAudio /*pow(10, m_Volume / 2000.0)*/);
        
        if ( m_Amplification )
            m_player_audio.SetDynamicRangeCompression(m_Amplification);
    }
    
    if ( threshold < 0.0f)
        threshold = m_live ? 0.7f : 0.2f;
    
    //    PrintSubtitleInfo();
    
    


    
    
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::pauseStream()
{
    if ( m_isRunning)
        m_shouldPause = true;

}

void MainPlayer::flipVisibilityTo( bool visible)
{
    if ( m_has_video != visible )
    {
        _shouldFlipVisibility = true;
        
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

unsigned long MainPlayer::getCurrentTC()
{
    return (m_av_clock->OMXMediaTime()*1e-3);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::registerTC( unsigned long tc)
{
    _registeredTCNotif.push_back( { tc , false} );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::resetAllRegisteredTC()
{
    for ( TCMark &t : _registeredTCNotif )
    {
        t.fired = false;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::update()
{
       
    
    //getEffectiveSpeed
    double now = m_av_clock->GetAbsoluteClock();
    //m_player_subtitles.DisplayText( strprintf("%ld", now) , 10 /*time to display in MS*/);
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    if ( m_shouldPause )
    {
        m_Pause = !m_Pause;
        
        if (m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_NORMAL && m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_PAUSE)
        {

            m_playspeed_current = playspeed_normal;
            SetSpeed( getEffectiveSpeed( m_playspeed_current ) );
            
            m_seek_flush = true; //
        }
        
        if( m_Pause )
        {
            /*
            if(m_has_subtitle)
                m_player_subtitles.Pause();
            */
            auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
            auto dur = m_omx_reader.GetStreamLength() / 1000;
            
            if ( _showInfosOnScreen )
                DISPLAY_TEXT_LONG(strprintf("Pause\n%02d:%02d:%02d / %02d:%02d:%02d",
                                            (t/3600), (t/60)%60, t%60, (dur/3600), (dur/60)%60, dur%60));

            _parent->didPause();
            
        }
        
        else
        {
            /*
            if( m_has_subtitle )
                m_player_subtitles.Resume();
            */
            auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
            auto dur = m_omx_reader.GetStreamLength() / 1000;

            if ( _showInfosOnScreen )
                DISPLAY_TEXT_SHORT(strprintf("Play\n%02d:%02d:%02d / %02d:%02d:%02d",
                                             (t/3600), (t/60)%60, t%60, (dur/3600), (dur/60)%60, dur%60));
            
            _parent->didResume();
        }
        
        m_shouldPause = false;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    if (_shouldFlipVisibility)
    {
        if (m_has_video) // hide
        {
            m_has_video = false;
            m_player_video.Close();
        }
        else // show
        {
            m_has_video = true;

            
            m_new_win_pos = true;
            m_seek_flush = true;
        }
        
        _shouldFlipVisibility = false;
    }
    
    
    /*
case KeyConfig::ACTION_HIDE_VIDEO:
    m_has_video = false;
    m_player_video.Close();
    
    if ( live )
    {
        m_omx_reader.Close();
        idle = true;
    }
    break;
    
case KeyConfig::ACTION_UNHIDE_VIDEO:
    m_has_video = true;
    
    if ( live )
    {
        idle = false;
        if(!m_omx_reader.Open( filename.c_str(), dump_format, true))
            return false;
    }
    
    new_win_pos = true;
    seek_flush = true;
    break;
     */
}


bool MainPlayer::openVideoPlayer()
{
    return m_player_video.Open(m_hints_video,
                               m_av_clock,
                               m_destRect,
                               m_Deinterlace ? VS_DEINTERLACEMODE_FORCE:m_NoDeinterlace ? VS_DEINTERLACEMODE_OFF:VS_DEINTERLACEMODE_AUTO,
                               m_anaglyph,
                               m_hdmi_clock_sync,
                               m_thread_player,
                               m_displayController->getCurrentMode().aspectRatio /*m_display_aspect*/,
                               0, /* m_display?*/
                               m_layer,
                               m_video_queue_size,
                               m_video_fifo_size);
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/*
    Main loop
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
bool MainPlayer::run()
{
    double  loop_from           = 0 ;
    double  last_check_time     = 0.0 ;
    
    double  startpts            = 0;
    bool    sentStarted         = false;

    double  last_seek_pos       = 0;
    bool    idle                = false;

    
    bool    send_eos            = false;
    bool    packet_after_seek   = false;
    

    bool firstPacket = true;
    m_stop = false;
    
    m_av_clock->OMXReset(m_has_video, m_has_audio);
    m_av_clock->OMXStateExecute();

    
    m_playspeed_current = playspeed_normal;
    
    m_isRunning = true;
    
    _parent->willStart();
    
    while( !m_bStop && !m_stop )
    {
        if(g_abort)
            return false;
        
        double now = m_av_clock->GetAbsoluteClock();
        
        /* TC NOTIF */

        for ( TCMark &t : _registeredTCNotif )
        {
            if ( ( t.fired == false )  && ( getCurrentTC() >= t.millis)  )
            {
                t.fired = true;
                _parent->didReachTC( t.millis );
            }
        }
        
        /* END OF TC NOTIF */
        
        bool shouldUpdate = false;
            
        if ( last_check_time == 0.0 || last_check_time + DVD_MSEC_TO_TIME(20) <= now )
        {
            shouldUpdate = true;
            last_check_time = now;
        }

        // a priori que sur de la source live
        if ( idle )
        {
            usleep(10000);
            continue;
        }
        
        // seek flush
        if( m_seek_flush || m_incr != 0)
        {
            double seek_pos     = 0;
            double pts          = 0;
            /*
            if(m_has_subtitle)
                m_player_subtitles.Pause();
            */
            pts = m_av_clock->OMXMediaTime();
            
            seek_pos = (pts ? pts / DVD_TIME_BASE : last_seek_pos) + m_incr;
            last_seek_pos = seek_pos;
            
            seek_pos *= 1000.0;
            
            m_incr = 0;
            
            if( m_omx_reader.SeekTime( (int) seek_pos, m_incr < 0.0f, &startpts ) )
            {
                unsigned t = (unsigned)(startpts*1e-6);
                auto dur = m_omx_reader.GetStreamLength() / 1000;
                
                if (! m_new_win_pos)
                {
                    if ( _showInfosOnScreen )
                        DISPLAY_TEXT_LONG( strprintf("Seek\n%02d:%02d:%02d / %02d:%02d:%02d",
                                                     (t/3600),
                                                     (t/60)%60,
                                                     t%60,
                                                     (dur/3600),
                                                     (dur/60)%60, dur%60)
                                         );
                    

                }
                else
                {
                    m_new_win_pos = false;
                }
                
                FlushStreams( startpts );
             }
            
            
            m_player_video.Close();
            
            sentStarted = false;
            
            if(    m_has_video && !openVideoPlayer() )
                    return false;
            
            CLog::Log(LOGDEBUG, "Seeked %.0f %.0f %.0f\n", DVD_MSEC_TO_TIME(seek_pos), startpts, m_av_clock->OMXMediaTime());
            
            m_av_clock->OMXPause();
            
            /*
            if(m_has_subtitle)
                m_player_subtitles.Resume();
            */
            packet_after_seek = false;
            m_seek_flush = false;
            
        } // end of seek flush
        
        
        else if( packet_after_seek && TRICKPLAY(m_av_clock->OMXPlaySpeed()) )
        {
            double seek_pos     = 0;
            double pts          = 0;
            
            pts = m_av_clock->OMXMediaTime();
            seek_pos = (pts / DVD_TIME_BASE);
            
            seek_pos *= 1000.0;
            
            if(m_omx_reader.SeekTime((int)seek_pos, m_av_clock->OMXPlaySpeed() < 0, &startpts))
                ; //FlushStreams(DVD_NOPTS_VALUE);
            
            CLog::Log(LOGDEBUG, "Seeked %.0f %.0f %.0f\n", DVD_MSEC_TO_TIME(seek_pos), startpts, m_av_clock->OMXMediaTime());
            
            //unsigned t = (unsigned)(startpts*1e-6);
            unsigned t = (unsigned)(pts*1e-6);
            printf("Seek to: %02d:%02d:%02d\n", (t/3600), (t/60)%60, t%60);
            packet_after_seek = false;
        }
        
        
        /* player got in an error state */
        if(m_player_audio.Error())
        {
            printf("audio player error. emergency exit!!!\n");
            return false;
        }
        
        if (  shouldUpdate )
        {
            
            update();
            /* when the video/audio fifos are low, we pause clock, when high we resume */
            
            double stamp     = m_av_clock->OMXMediaTime();
            double audio_pts = m_player_audio.GetCurrentPTS();
            double video_pts = m_player_video.GetCurrentPTS();
            
            // utile ??
            if (0 && m_av_clock->OMXIsPaused())
            {
                double old_stamp = stamp;
                if (audio_pts != DVD_NOPTS_VALUE && (stamp == 0 || audio_pts < stamp))
                    stamp = audio_pts;
                
                if (video_pts != DVD_NOPTS_VALUE && (stamp == 0 || video_pts < stamp))
                    stamp = video_pts;
                
                if (old_stamp != stamp)
                {
                    m_av_clock->OMXMediaTime(stamp);
                    stamp = m_av_clock->OMXMediaTime();
                }
            }
            
            float audio_fifo = audio_pts == DVD_NOPTS_VALUE ? 0.0f : audio_pts / DVD_TIME_BASE - stamp * 1e-6;
            float video_fifo = video_pts == DVD_NOPTS_VALUE ? 0.0f : video_pts / DVD_TIME_BASE - stamp * 1e-6;
            float threshold = std::min(0.1f, (float)m_player_audio.GetCacheTotal() * 0.1f);
            
            bool audio_fifo_low  = false;
            bool video_fifo_low  = false;
            bool audio_fifo_high = false;
            bool video_fifo_high = false;
            
            
            // affichage des stats
            if( m_stats )
            {
                static int count;
                if ((count++ & 7) == 0)
                    printf("M:%8.0f V:%6.2fs %6dk/%6dk A:%6.2f %6.02fs/%6.02fs Cv:%6dk Ca:%6dk                            \r", stamp,
                            video_fifo,
                            (m_player_video.GetDecoderBufferSize()-m_player_video.GetDecoderFreeSpace())>>10,
                            m_player_video.GetDecoderBufferSize()>>10,
                            audio_fifo, m_player_audio.GetDelay(),
                            m_player_audio.GetCacheTotal(),
                            m_player_video.GetCached()>>10,
                            m_player_audio.GetCached()>>10
                           );
            }
            
            // ??
            if(m_tv_show_info)
            {
                static unsigned count;
                if ((count++ & 7) == 0)
                {
                    char response[80];
                    
                    if (m_player_video.GetDecoderBufferSize() && m_player_audio.GetCacheTotal())
                        vc_gencmd(response, sizeof response, "render_bar 4 video_fifo %d %d %d %d",
                                  (int)(100.0*m_player_video.GetDecoderBufferSize()-m_player_video.GetDecoderFreeSpace())/m_player_video.GetDecoderBufferSize(),
                                  (int)(100.0*video_fifo/m_player_audio.GetCacheTotal()),
                                  0, 100);
                    
                    if (m_player_audio.GetCacheTotal())
                        vc_gencmd(response, sizeof response, "render_bar 5 audio_fifo %d %d %d %d",
                                  (int)(100.0*audio_fifo/m_player_audio.GetCacheTotal()),
                                  (int)(100.0*m_player_audio.GetDelay()/m_player_audio.GetCacheTotal()),
                                  0, 100);
                    
                    vc_gencmd(response, sizeof response, "render_bar 6 video_queue %d %d %d %d",
                              m_player_video.GetLevel(), 0, 0, 100);
                    
                    vc_gencmd(response, sizeof response, "render_bar 7 audio_queue %d %d %d %d",
                              m_player_audio.GetLevel(), 0, 0, 100);
                }
            }
            
            if (audio_pts != DVD_NOPTS_VALUE)
            {
                audio_fifo_low  =  m_has_audio && audio_fifo < threshold;
                audio_fifo_high = !m_has_audio || (audio_pts != DVD_NOPTS_VALUE && audio_fifo > threshold);
            }
            if (video_pts != DVD_NOPTS_VALUE)
            {
                video_fifo_low = m_has_video && video_fifo < threshold;
                video_fifo_high = !m_has_video || (video_pts != DVD_NOPTS_VALUE && video_fifo > threshold);
            }
            
            CLog::Log(LOGDEBUG, "Normal M:%.0f (A:%.0f V:%.0f) P:%d A:%.2f V:%.2f/T:%.2f (%d,%d,%d,%d) A:%d%% V:%d%% (%.2f,%.2f)\n",
                                stamp, audio_pts,
                                video_pts,
                                m_av_clock->OMXIsPaused(),
                                audio_pts == DVD_NOPTS_VALUE ? 0.0:audio_fifo,
                                video_pts == DVD_NOPTS_VALUE ? 0.0:video_fifo,
                                threshold, audio_fifo_low,
                                video_fifo_low,
                                audio_fifo_high,
                                video_fifo_high,
                                m_player_audio.GetLevel(),
                                m_player_video.GetLevel(),
                                m_player_audio.GetDelay(),
                                (float)m_player_audio.GetCacheTotal()
                      );
            
            // keep latency under control by adjusting clock (and so resampling audio)
            if ( m_live )
            {
                float latency = DVD_NOPTS_VALUE;
                
                if (m_has_audio && audio_pts != DVD_NOPTS_VALUE)
                    latency = audio_fifo;
                
                else if (!m_has_audio && m_has_video && video_pts != DVD_NOPTS_VALUE)
                    latency = video_fifo;
                
                if (!m_Pause && latency != DVD_NOPTS_VALUE)
                {
                    if (m_av_clock->OMXIsPaused())
                    {
                        if (latency > threshold)
                        {
                            CLog::Log(LOGDEBUG, "Resume %.2f,%.2f (%d,%d,%d,%d) EOF:%d PKT:%p\n",
                                        audio_fifo,
                                        video_fifo,
                                        audio_fifo_low,
                                        video_fifo_low,
                                        audio_fifo_high,
                                        video_fifo_high,
                                        m_omx_reader.IsEof(),
                                        m_omx_pkt
                                      );
                            
                            m_av_clock->OMXResume();
                            m_latency = latency;
                        }
                    }
                    
                    else
                    {
                        m_latency = m_latency*0.99f + latency*0.01f;
                        float speed = 1.0f;
                        
                        if (m_latency < 0.5f*threshold)
                            speed = 0.990f;
                        
                        else if (m_latency < 0.9f*threshold)
                            speed = 0.999f;
                        
                        else if (m_latency > 2.0f*threshold)
                            speed = 1.010f;
                        
                        else if (m_latency > 1.1f* threshold)
                            speed = 1.001f;
                        
                        m_av_clock->OMXSetSpeed(S(speed));
                        m_av_clock->OMXSetSpeed(S(speed), true, true);
                        CLog::Log(LOGDEBUG, "Live: %.2f (%.2f) S:%.3f T:%.2f\n", m_latency, latency, speed, threshold);
                    }
                }
            } // end of if (m_live)
            
            else if(    !m_Pause
                      && (   m_omx_reader.IsEof()
                          || m_omx_pkt
                          || TRICKPLAY( m_av_clock->OMXPlaySpeed() )
                          || ( audio_fifo_high && video_fifo_high)
                         )
                    )
            {
                if (m_av_clock->OMXIsPaused())
                {

                    CLog::Log( LOGDEBUG, "Resume %.2f,%.2f (%d,%d,%d,%d) EOF:%d PKT:%p\n",
                               audio_fifo,
                               video_fifo,
                               audio_fifo_low,
                               video_fifo_low,
                               audio_fifo_high,
                               video_fifo_high,
                               m_omx_reader.IsEof(),
                               m_omx_pkt
                              );
                    
                    m_av_clock->OMXResume();
                    
                    _parent->didResume();
                }
            }
            
            // si en pause ou fifos audio et/ou video pleines
            else if ( m_Pause || audio_fifo_low || video_fifo_low)
            {
                if (!m_av_clock->OMXIsPaused())
                {
                    if (!m_Pause)
                        threshold = std::min(2.0f * threshold, 16.0f);
                    
                    CLog::Log( LOGDEBUG, "Pause %.2f,%.2f (%d,%d,%d,%d) %.2f\n",
                               audio_fifo,
                               video_fifo,
                               audio_fifo_low,
                               video_fifo_low,
                               audio_fifo_high,
                               video_fifo_high,
                               threshold
                              );
                    
                    m_av_clock->OMXPause();
                    
                    _parent->didPause();
                }
            }
            
            
        } // end of ShouldUpdate
        
        // on a renvoyé la commande 'play':
        // sentStarted = false => reset
        // sentStarted = true  => lecture 'normale'
        if (!sentStarted)
        {
            CLog::Log(LOGDEBUG, "COMXPlayer::HandleMessages - player started RESET");
            m_av_clock->OMXReset(m_has_video, m_has_audio);
            sentStarted = true;
        }
        
        // paquet NULL, on lit
        if(!m_omx_pkt)
        {
            m_omx_pkt = m_omx_reader.Read();
        }
        
        // un paquet lu, on n'est pas end-of-stream
        if(m_omx_pkt)
        {
            if (firstPacket)
            {
                _parent->didStart();
                firstPacket = false;
            }
            send_eos = false;
        }
        
        // end-of-stream ET pas de paquet lu ,
        // on flag send_eos et on continue de lire s'il reste des paquets en attente
        if( m_omx_reader.IsEof() && !m_omx_pkt)
        {
            // demuxer EOF, but may have not played out data yet
            if (    ( m_has_video && m_player_video.GetCached() )
                 || ( m_has_audio && m_player_audio.GetCached() )
               )
            {
                OMXClock::OMXSleep(10);
                continue;
            }
            
            if (m_loop)
            {
                m_incr = loop_from - (m_av_clock->OMXMediaTime() ? m_av_clock->OMXMediaTime() / DVD_TIME_BASE : last_seek_pos);
                continue;
            }
            
            if (!send_eos && m_has_video)
                m_player_video.SubmitEOS();
            
            if (!send_eos && m_has_audio)
                m_player_audio.SubmitEOS();

            if (!send_eos)
                _parent->willEnd();
            
            send_eos = true;
            

            
            if (   (m_has_video && !m_player_video.IsEOS())
                || (m_has_audio && !m_player_audio.IsEOS()) )
            {
                OMXClock::OMXSleep(10);
                continue;
            }
            
            break;
            
        }
        
        // si le fichier a de la vidéo, qu'il y a un paquet lu et qu'il est de nature video
        if(    m_has_video && m_omx_pkt
            && m_omx_reader.IsActive(OMXSTREAM_VIDEO, m_omx_pkt->stream_index)
          )
        {
            if (TRICKPLAY(m_av_clock->OMXPlaySpeed()))
            {
                packet_after_seek = true;
            }
            
            // on envoie le paquet au player video et on le NULLise pour pouvoir écrire dedans à la prochaine étape
            if(m_player_video.AddPacket(m_omx_pkt))
            {
                m_omx_pkt = NULL;
            }
            
            else
                OMXClock::OMXSleep(10);
        }

        // si le fichier a de l'audio, qu'il y a un paquet lu et qu'il est de nature audio
        else if(    m_has_audio
                 && m_omx_pkt
                 && !TRICKPLAY(m_av_clock->OMXPlaySpeed())
                 && m_omx_pkt->codec_type == AVMEDIA_TYPE_AUDIO
                )
        {
            if(m_player_audio.AddPacket(m_omx_pkt))
            {
                m_omx_pkt = NULL;
            }
            else
                OMXClock::OMXSleep(10);
            
        }
/*
        // si le fichier a des sous-titres, qu'il y a un paquet lu et qu'il est de nature sous-titre
        else if(    m_has_subtitle
                 && m_omx_pkt
                 && !TRICKPLAY(m_av_clock->OMXPlaySpeed())
                 && m_omx_pkt->codec_type == AVMEDIA_TYPE_SUBTITLE)
        {
            auto result = m_player_subtitles.AddPacket(m_omx_pkt,
                                                       m_omx_reader.GetRelativeIndex(m_omx_pkt->stream_index));
            if (result)
                m_omx_pkt = NULL;
            
            else
                OMXClock::OMXSleep(10);
            
        }
*/
        
        // sinon on delete le paquet et on continue
        else
        {
            if(m_omx_pkt)
            {
                m_omx_reader.FreePacket(m_omx_pkt);
                m_omx_pkt = NULL;
            }
            
            else
                OMXClock::OMXSleep(10);
        }
        
    } // end of main loop
    
    _parent->didEnd();

    m_isRunning = false;
    
    return true;

    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::releasePlayer()
{
    if (m_stop)
    {
        unsigned t = (unsigned)(m_av_clock->OMXMediaTime()*1e-6);
        printf("Stopped at: %02d:%02d:%02d\n", (t/3600), (t/60)%60, t%60);
    }
    

    //m_displayController->restorePreviousParameters();
    
    m_av_clock->OMXStop();
    m_av_clock->OMXStateIdle();
    
//    m_player_subtitles.Close();
    m_player_video.Close();
    m_player_audio.Close();

    
    if(m_omx_pkt)
    {
        m_omx_reader.FreePacket(m_omx_pkt);
        m_omx_pkt = NULL;
    }
    
    m_omx_reader.Close();
    
    m_av_clock->OMXDeinitialize();
    
    /*
    if (m_av_clock)
        delete m_av_clock;
    */
    vc_tv_show_info(0);

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

void MainPlayer::Process()
{

    if (!run())
    {
        printf("\n Error while trying to play video \n");
    }
    


    releasePlayer();


    


}




/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 OMXControlResult result = m_omxcontrol.getEvent();
 double oldPos;
 double newPos;
 
 switch(result.getKey())
 {
 case KeyConfig::ACTION_SHOW_INFO:
 m_tv_show_info = !m_tv_show_info;
 vc_tv_show_info(m_tv_show_info);
 break;
 
 case KeyConfig::ACTION_DECREASE_SPEED:
 if (    playspeed_current < playspeed_slow_min
 || playspeed_current > playspeed_slow_max
 )
 playspeed_current = playspeed_slow_max-1;
 
 playspeed_current = std::max(playspeed_current-1, (int) playspeed_slow_min);
 
 SetSpeed(playspeeds[playspeed_current]);
 DISPLAY_TEXT_SHORT(
 strprintf("Playspeed: %.3f", playspeeds[playspeed_current]/1000.0f));
 
 printf("Playspeed %.3f\n", playspeeds[playspeed_current]/1000.0f);
 
 m_Pause = false;
 break;
 
 case KeyConfig::ACTION_INCREASE_SPEED:
 
 if (playspeed_current < playspeed_slow_min || playspeed_current > playspeed_slow_max)
 playspeed_current = playspeed_slow_max-1;
 
 playspeed_current = std::min(playspeed_current+1, (int) playspeed_slow_max);
 SetSpeed(playspeeds[playspeed_current]);
 DISPLAY_TEXT_SHORT(
 strprintf("Playspeed: %.3f", playspeeds[playspeed_current]/1000.0f));
 printf("Playspeed %.3f\n", playspeeds[playspeed_current]/1000.0f);
 m_Pause = false;
 break;
 
 case KeyConfig::ACTION_REWIND:
 if (playspeed_current >= playspeed_ff_min && playspeed_current <= playspeed_ff_max)
 {
 playspeed_current = playspeed_normal;
 seek_flush = true;
 }
 
 else if (playspeed_current < playspeed_rew_max || playspeed_current > playspeed_rew_min)
 playspeed_current = playspeed_rew_min;
 
 else
 playspeed_current = std::max(playspeed_current-1, (int) playspeed_rew_max);
 
 SetSpeed(playspeeds[playspeed_current]);
 DISPLAY_TEXT_SHORT(
 strprintf("Playspeed: %.3f", playspeeds[playspeed_current]/1000.0f));
 printf("Playspeed %.3f\n", playspeeds[playspeed_current]/1000.0f);
 m_Pause = false;
 break;
 
 case KeyConfig::ACTION_FAST_FORWARD:
 if (playspeed_current >= playspeed_rew_max && playspeed_current <= playspeed_rew_min)
 {
 playspeed_current = playspeed_normal;
 seek_flush = true;
 }
 
 else if (playspeed_current < playspeed_ff_min || playspeed_current > playspeed_ff_max)
 playspeed_current = playspeed_ff_min;
 
 else
 playspeed_current = std::min(playspeed_current+1, (int) playspeed_ff_max);
 
 SetSpeed(playspeeds[playspeed_current]);
 DISPLAY_TEXT_SHORT(
 strprintf("Playspeed: %.3f", playspeeds[playspeed_current]/1000.0f));
 printf("Playspeed %.3f\n", playspeeds[playspeed_current]/1000.0f);
 m_Pause = false;
 break;
 
 case KeyConfig::ACTION_STEP:
 m_av_clock->OMXStep();
 printf("Step\n");
 {
 auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-3);
 auto dur = m_omx_reader.GetStreamLength() / 1000;
 DISPLAY_TEXT_SHORT(
 strprintf("Step\n%02d:%02d:%02d.%03d / %02d:%02d:%02d",
 (t/3600000), (t/60000)%60, (t/1000)%60, t%1000,
 (dur/3600), (dur/60)%60, dur%60));
 }
 break;
 
 case KeyConfig::ACTION_PREVIOUS_AUDIO:
 if(m_has_audio)
 {
 int new_index = m_omx_reader.GetAudioIndex() - 1;
 
 if(new_index >= 0)
 {
 m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, new_index);
 DISPLAY_TEXT_SHORT(
 strprintf("Audio stream: %d", m_omx_reader.GetAudioIndex() + 1));
 }
 }
 break;
 
 case KeyConfig::ACTION_NEXT_AUDIO:
 if(m_has_audio)
 {
 m_omx_reader.SetActiveStream(OMXSTREAM_AUDIO, m_omx_reader.GetAudioIndex() + 1);
 DISPLAY_TEXT_SHORT(
 strprintf("Audio stream: %d", m_omx_reader.GetAudioIndex() + 1));
 }
 break;
 
 case KeyConfig::ACTION_PREVIOUS_CHAPTER:
 if(m_omx_reader.GetChapterCount() > 0)
 {
 m_omx_reader.SeekChapter(m_omx_reader.GetChapter() - 1, &startpts);
 DISPLAY_TEXT_LONG(strprintf("Chapter %d", m_omx_reader.GetChapter()));
 FlushStreams(startpts);
 }
 
 else
 {
 incr = -600.0;
 }
 
 break;
 
 case KeyConfig::ACTION_NEXT_CHAPTER:
 if(m_omx_reader.GetChapterCount() > 0)
 {
 m_omx_reader.SeekChapter(m_omx_reader.GetChapter() + 1, &startpts);
 DISPLAY_TEXT_LONG(strprintf("Chapter %d", m_omx_reader.GetChapter()));
 FlushStreams(startpts);
 }
 
 else
 {
 incr = 600.0;
 }
 
 break;
 
 case KeyConfig::ACTION_PREVIOUS_SUBTITLE:
 if(m_has_subtitle)
 {
 if(!m_player_subtitles.GetUseExternalSubtitles())
 {
 if (m_player_subtitles.GetActiveStream() == 0)
 {
 if(m_has_external_subtitles)
 {
 DISPLAY_TEXT_SHORT("Subtitle file:\n" + m_external_subtitles_path);
 m_player_subtitles.SetUseExternalSubtitles(true);
 }
 }
 else
 {
 auto new_index = m_player_subtitles.GetActiveStream()-1;
 DISPLAY_TEXT_SHORT(strprintf("Subtitle stream: %d", new_index+1));
 m_player_subtitles.SetActiveStream(new_index);
 }
 }
 
 m_player_subtitles.SetVisible(true);
 //                        PrintSubtitleInfo();
 }
 break;
 
 case KeyConfig::ACTION_NEXT_SUBTITLE:
 if(m_has_subtitle)
 {
 if(m_player_subtitles.GetUseExternalSubtitles())
 {
 if(m_omx_reader.SubtitleStreamCount())
 {
 assert(m_player_subtitles.GetActiveStream() == 0);
 DISPLAY_TEXT_SHORT("Subtitle stream: 1");
 m_player_subtitles.SetUseExternalSubtitles(false);
 }
 }
 else
 {
 auto new_index = m_player_subtitles.GetActiveStream()+1;
 if(new_index < (size_t) m_omx_reader.SubtitleStreamCount())
 {
 DISPLAY_TEXT_SHORT(strprintf("Subtitle stream: %d", new_index+1));
 m_player_subtitles.SetActiveStream(new_index);
 }
 }
 
 m_player_subtitles.SetVisible(true);
 //                        PrintSubtitleInfo();
 }
 break;
 
 case KeyConfig::ACTION_TOGGLE_SUBTITLE:
 if(m_has_subtitle)
 {
 m_player_subtitles.SetVisible(!m_player_subtitles.GetVisible());
 //   PrintSubtitleInfo();
 }
 break;
 
 
 case KeyConfig::ACTION_DECREASE_SUBTITLE_DELAY:
 if(m_has_subtitle && m_player_subtitles.GetVisible())
 {
 auto new_delay = m_player_subtitles.GetDelay() - 250;
 DISPLAY_TEXT_SHORT(strprintf("Subtitle delay: %d ms", new_delay));
 m_player_subtitles.SetDelay(new_delay);
 //  PrintSubtitleInfo();
 }
 break;
 
 case KeyConfig::ACTION_INCREASE_SUBTITLE_DELAY:
 if(m_has_subtitle && m_player_subtitles.GetVisible())
 {
 auto new_delay = m_player_subtitles.GetDelay() + 250;
 DISPLAY_TEXT_SHORT(strprintf("Subtitle delay: %d ms", new_delay));
 m_player_subtitles.SetDelay(new_delay);
 //   PrintSubtitleInfo();
 }
 break;
 
 case KeyConfig::ACTION_EXIT:
 m_stop = true;
 return true;
 break;
 
 case KeyConfig::ACTION_SEEK_BACK_SMALL:
 if(m_omx_reader.CanSeek()) incr = -30.0;
 break;
 
 case KeyConfig::ACTION_SEEK_FORWARD_SMALL:
 if(m_omx_reader.CanSeek()) incr = 30.0;
 break;
 
 case KeyConfig::ACTION_SEEK_FORWARD_LARGE:
 if(m_omx_reader.CanSeek()) incr = 600.0;
 break;
 
 case KeyConfig::ACTION_SEEK_BACK_LARGE:
 if(m_omx_reader.CanSeek()) incr = -600.0;
 break;
 
 case KeyConfig::ACTION_SEEK_RELATIVE:
 incr = result.getArg() * 1e-6;
 break;
 
 case KeyConfig::ACTION_SEEK_ABSOLUTE:
 newPos = result.getArg() * 1e-6;
 oldPos = m_av_clock->OMXMediaTime()*1e-6;
 incr = newPos - oldPos;
 break;
 
 case KeyConfig::ACTION_PAUSE:
 m_Pause = !m_Pause;
 
 if (m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_NORMAL && m_av_clock->OMXPlaySpeed() != DVD_PLAYSPEED_PAUSE)
 {
 printf("resume\n");
 playspeed_current = playspeed_normal;
 SetSpeed(playspeeds[playspeed_current]);
 seek_flush = true;
 }
 
 if(m_Pause)
 {
 if(m_has_subtitle)
 m_player_subtitles.Pause();
 
 auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
 auto dur = m_omx_reader.GetStreamLength() / 1000;
 DISPLAY_TEXT_LONG(strprintf("Pause\n%02d:%02d:%02d / %02d:%02d:%02d",
 (t/3600), (t/60)%60, t%60, (dur/3600), (dur/60)%60, dur%60));
 }
 else
 {
 if(m_has_subtitle)
 m_player_subtitles.Resume();
 
 auto t = (unsigned) (m_av_clock->OMXMediaTime()*1e-6);
 auto dur = m_omx_reader.GetStreamLength() / 1000;
 DISPLAY_TEXT_SHORT(strprintf("Play\n%02d:%02d:%02d / %02d:%02d:%02d",
 (t/3600), (t/60)%60, t%60, (dur/3600), (dur/60)%60, dur%60));
 }
 break;
 
 case KeyConfig::ACTION_MOVE_VIDEO:
 sscanf(result.getWinArg(), "%f %f %f %f", &DestRect.x1, &DestRect.y1, &DestRect.x2, &DestRect.y2);
 m_has_video = true;
 new_win_pos = true;
 seek_flush = true;
 break;
 
 case KeyConfig::ACTION_HIDE_VIDEO:
 m_has_video = false;
 m_player_video.Close();
 
 if ( live )
 {
 m_omx_reader.Close();
 idle = true;
 }
 break;
 
 case KeyConfig::ACTION_UNHIDE_VIDEO:
 m_has_video = true;
 
 if ( live )
 {
 idle = false;
 if(!m_omx_reader.Open( filename.c_str(), dump_format, true))
 return false;
 }
 
 new_win_pos = true;
 seek_flush = true;
 break;
 
 case KeyConfig::ACTION_DECREASE_VOLUME:
 m_Volume -= 300;
 m_player_audio.SetVolume(pow(10, m_Volume / 2000.0));
 DISPLAY_TEXT_SHORT(strprintf("Volume: %.2f dB",
 m_Volume / 100.0f));
 printf("Current Volume: %.2fdB\n", m_Volume / 100.0f);
 break;
 
 case KeyConfig::ACTION_INCREASE_VOLUME:
 m_Volume += 300;
 m_player_audio.SetVolume(pow(10, m_Volume / 2000.0));
 DISPLAY_TEXT_SHORT(strprintf("Volume: %.2f dB",
 m_Volume / 100.0f));
 printf("Current Volume: %.2fdB\n", m_Volume / 100.0f);
 break;
 
 
 default:
 break;
 
 }*/


