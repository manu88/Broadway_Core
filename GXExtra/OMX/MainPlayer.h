//
//  MainPlayer.h
//  
//
//  Created by Manuel Deneu on 26/07/14.
//
//

#ifndef ____MAINPLAYER__
#define ____MAINPLAYER__


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <termios.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <getopt.h>
#include <string.h>
#include <vector>

#include <condition_variable>

//#define AV_NOWARN_DEPRECATED // ?



#include "OMXStreamInfo.h"

#include "utils/log.h"

#include "DllAvUtil.h"
#include "DllAvFormat.h"
#include "DllAvCodec.h"
#include "linux/RBP.h"

#include "OMXVideo.h"
#include "OMXAudioCodecOMX.h"
#include "utils/PCMRemap.h"
#include "OMXClock.h"
#include "OMXAudio.h"
#include "OMXReader.h"
#include "OMXPlayerVideo.h"
#include "OMXPlayerAudio.h"
#include "OMXPlayerSubtitles.h"


#include "OMXControl.h" // a suppr

#include "OMXThread.h"

#include "DllOMX.h"
#include "Srt.h"
#include "KeyConfig.h"
//
#include "Keyboard.h"

#include <string>
#include <utility>




#include "../../GX/DisplayController.h"



// when we repeatedly seek, rather than play continuously
#define TRICKPLAY(speed) (speed < 0 || speed > 4 * DVD_PLAYSPEED_NORMAL)

#define DISPLAY_TEXT(text, ms) if(m_osd) m_player_subtitles.DisplayText(text, ms)

#define DISPLAY_TEXT_SHORT(text) DISPLAY_TEXT(text, 1000)
#define DISPLAY_TEXT_LONG(text) DISPLAY_TEXT(text, 2000)


#define S(x) (int)(DVD_PLAYSPEED_NORMAL*(x))

typedef struct
{
    unsigned long millis;
    bool          fired;
    
} TCMark;

class GXVideo;

/* **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** */

class MainPlayer : public OMXThread
{
public:
    
    
    MainPlayer( GXVideo *parent);
    ~MainPlayer();
    
    void setClock(OMXClock *clockToUse)
    {
        m_av_clock = clockToUse;
    }
    
    void setDisplayController(DisplayController* controllerToUse)
    {
        m_displayController = controllerToUse;
    }

    void setVideoFileSource( const std::string &filePath )
    {
        m_filename = filePath;
    }
    
    void setLooped( bool loop)
    {
        m_loop = loop;
    }
    
    bool isLooped() const
    {
        return m_loop;
    }
    
    void setBounds(int x , int y , int w , int h);
    void setLayerNum( int layer );
    
    void setAudioDeviceName (const std::string &device)
    {
        deviceString = device;
    }
    
    int getStreamLength() // should go const ..
    {
        return ( int ) m_omx_reader.GetStreamLength() / 1000;
    }
    
    unsigned long getCurrentTC();
    
    void registerTC( unsigned long tc);
    void resetAllRegisteredTC();
    
    
    
    
    bool prepare();
    void start(); // main entry point
    void pauseStream();
    void stop();
    void releasePlayer();
    
    void flipVisibilityTo( bool visible);
    
    void seekTo(double timeInS);

    bool isRunning() const
    {
        return m_isRunning;
    }
    
    bool isPaused() const
    {
        return m_Pause;
    }

    
    void SetSpeed(int iSpeed);
    
    // def method here
    void setSpeedAsync( int newSpeed );
    
    void FlushStreams(double pts);
    
    enum {  playspeed_slow_min = 0 ,
            playspeed_slow_max = 7 ,
            playspeed_rew_max = 8 ,
            playspeed_rew_min = 13 ,
            playspeed_normal = 14 ,
            playspeed_ff_min = 15 ,
            playspeed_ff_max = 19
         };
    
    int getEffectiveSpeed(int index)
    {   
        switch (index)
        {
            case 0:
                return S(0);
                
            case 1:
                return S(1/16.0);
                
            case 2:
                return S(1/8.0);

            case 3:
                return S(1/4.0);
                
            case 4:
                return S(1/2.0);
            
            case 5:
                return S(0.975);
                
            case 6:
                return S(1.0);
                
            case 7:
                return S(1.125);
                
            case 8:
                return S(-32.0);
                
            case 9:
                return S(-16.0);
                
            case 10:
                return S(-8.0);
                
            case 11:
                return S(-4);
                
            case 12:
                return S(-2);
                
            case 13:
                return S(-1);
                
            case 14:
                return S(1);
                
            case 15:
                return S(2.0);
                
            case 16:
                return S(4.0);
                
            case 17:
                return S(8.0);
                
            case 18:
                return S(16.0);
                
            case 19:
                return S(32.0);

            default:
                return 0;
        }
    }
    
    void setVolume( float vol)
    {
        if ( vol <0)
            vol = 0.f;
        m_volumeAudio = vol;
        
         m_player_audio.SetVolume( m_volumeAudio );
        
    }
    
    float getVolume() const
    {
        return m_volumeAudio;
    }
    
    void showInfosOnScreen( bool show )
    {
        _showInfosOnScreen = show;
    }
    
    bool getInfoOnScreen() const // Todo : better name :-)
    {
        return _showInfosOnScreen;
    }

    /**/
    
    void signalSourceChange()
    {
        _sourceWillChange = true;
    }
    
    void signalSourceChanged()
    {
        _wakeUp.notify_all();
    }

    /**/    

protected:
    
    void waitIfNeeded( std::unique_lock<std::mutex> &lock ,const double  startpts );
    /* Thread's starting point */
    virtual void Process();
    
    void update();
    
    
private:
    bool run();
    
    bool openVideoPlayer();
    
    /* *** *** *** *** *** *** */
    // cleaned attributes
    GXVideo         *_parent;
    vector< TCMark > _registeredTCNotif;
    std::string     m_filename;
    
    // flags
    bool    m_shouldPause;
    bool    m_isRunning;
    
    bool _shouldChangeSpeed;
    int  _nextSpeed;
    
    bool _firstPacket;
    
    bool _shouldFlipVisibility;

    
    DisplayController *m_displayController;
    
    int     m_playspeed_current;
    bool    m_seek_flush;
    
    double  m_incr;
    bool    m_new_win_pos; // flag to signal new win;
    
    bool _showInfosOnScreen;
    
    
    volatile bool _sourceWillChange;
    std::mutex  _sync;
    
    std::condition_variable _wakeUp;

    /* *** *** *** *** *** *** */
    
    bool        m_live;
    float       m_video_queue_size;
    float       m_video_fifo_size;
    float       m_latency;
    CRect       m_destRect; // GXElement
    
    FORMAT_3D_T       m_3d;
    
    COMXStreamInfo    m_hints_audio;
    COMXStreamInfo    m_hints_video;
    
    /* Relatifs au player en général */
    //static void sig_handler(int s);

    
    
    //////////////
    // à trier ...

    
    enum PCMChannels  *m_pChannelMap;

    // a virer

    OMXPlayerSubtitles  m_player_subtitles;


    bool              m_stats; /* Afficher stats */
    bool              m_passthrough; /* ? */

//    long              m_Volume; // à jarter
    float m_volumeAudio;
    long              m_Amplification;
    bool              m_Deinterlace;
    bool              m_NoDeinterlace;
    
    
    OMX_IMAGEFILTERANAGLYPHTYPE m_anaglyph;
    
    bool              m_HWDecode;
    
    std::string       deviceString;
    
    int               m_use_hw_audio;
    
    bool              m_osd;
    
    
    std::string       m_external_subtitles_path;
    bool              m_has_external_subtitles;
    
    std::string       m_font_path;
    std::string       m_italic_font_path;
    std::string       m_dbus_name;
    
    bool              m_asked_for_font;
    bool              m_asked_for_italic_font;
    
    float             m_font_size;
    
    bool              m_centered;
    
    bool              m_ghost_box;
    
    unsigned int      m_subtitle_lines;
    
    bool              m_Pause;
    
    OMXReader         m_omx_reader;
    
    int               m_audio_index_use;
    
    bool              m_thread_player;
    
    OMXClock          *m_av_clock; // partageable avec d'autres instances
    
    // paquet en sortie de décodage fichier vers player audio ou video ou sous-titres
    OMXPacket         *m_omx_pkt;
    
    bool              m_hdmi_clock_sync;
    
    bool              m_no_hdmi_clock_sync;
    
    bool              m_stop;
    
    int               m_subtitle_index;
    

    
    OMXPlayerVideo      m_player_video;
    OMXPlayerAudio      m_player_audio;
    
    
    
    int               m_tv_show_info;
    
    bool              m_has_video;
    bool              m_has_audio;
    bool              m_has_subtitle;
    

    
    bool              m_boost_on_downmix;

    bool              m_loop;
    int               m_layer;
    
    enum {ERROR=-1,SUCCESS,ONEBYTE};
    
    /* Statics */
    //static volatile sig_atomic_t g_abort;

};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/
static bool Exists(const std::string& path)
{
    struct stat buf;
    auto error = stat(path.c_str(), &buf);
    return !error || errno != ENOENT;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

static bool IsURL(const std::string& str)
{
    auto result = str.find("://");
    if(result == std::string::npos || result == 0)
        return false;
    
    for(size_t i = 0; i < result; ++i)
    {
        if(!isalpha(str[i]))
            return false;
    }
    return true;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

static bool IsPipe(const std::string& str)
{
    if (str.compare(0, 5, "pipe:") == 0)
        return true;
    return false;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** *****/

static int get_mem_gpu(void)
{
    char response[80] = "";
    int gpu_mem = 0;
    if (vc_gencmd(response, sizeof response, "get_mem gpu") == 0)
        vc_gencmd_number_property(response, "gpu", &gpu_mem);
    return gpu_mem;
}


#endif /* defined(____MAINPLAYER__) */
