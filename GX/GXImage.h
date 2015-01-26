/*!
 *  \brief     GXImage
 *  \details   GXElement specialised in jpeg display.
 *  \author    Manuel Deneu
 *  \version   0
 *  \date      2014
 *  \pre       .
 *  \bug       Only jpeg files for now
 *  \warning   .
 *  \copyright GNU Public License.
 */

#ifndef __Broadway_test__GXImage__
#define __Broadway_test__GXImage__



#ifdef TARGET_RASPBERRY_PI
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "VG/openvg.h"
#include "VG/vgu.h"

#else
#include "../Plateforms/MacOS/MacDummies/vg_mac.h"
#endif

#include "../GXDataType/GXGeometry.h"

#include "GXElement.h"

class GXImage : public GXElement
{
public:
    GXImage(const std::string &filename);
    ~GXImage();
    

    void fitImageToBounds( bool autoScale );

protected:
    virtual void paint( const GXRect &rect );    
    virtual void prepareRessources();
    virtual void deleteRessources();
    virtual void changed();
private:
    
    std::string m_filename;
    
    float       m_scaleX;
    float       m_scaleY;
    
    GXSize      m_imageSize;
    bool        m_autoScale;

    VGImage m_image;
    static VGImage createImageFromJpeg(const char *filename , int &w , int &h);


    
};



#endif /* defined(__Broadway_test__GXImage__) */
