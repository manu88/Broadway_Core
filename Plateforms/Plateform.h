//
//  Plateform.h
//  Broadway_test
//
//  Created by Manuel Deneu on 23/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_Plateform_h
#define Broadway_test_Plateform_h

#ifdef TARGET_RASPBERRY_PI
    #include "RaspberryPI/RaspberryPI.h"

#elif defined __APPLE__
    #include "MacOS/MacOS.h"

    #ifdef USE_OPENMAXIL
    #undef USE_OPENMAXIL
    #endif

#elif defined TARGET_CUBIE2
    #include "CubieBoard/CubieBoard.h"

#elif defined TARGET_BEAGLEBONE_BLACK
    #include "BeagleBoneBlack/BeagleBoneBlack.h"


#endif


/*
 
 According to the targeted plateform, the following will or will not be defined
 
                 | <---------------------- Display ------------------- >  |             |                |  Computation  |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 Library/ dep    | EGL       | Open GL ES 2 |   Open VG    | Open MAX     |   FFMPEG    |  linux-can     |    Neon       |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 Macro           | HAVE_EGL  |  HAVE_GLES2  | HAVE_OPENVG  | HAVE_OPENMAX | HAVE_FFMPEG | HAVE_LINUX_CAN |    ARM_NEON   |
 ================o===========o==============o==============o==============o=============o================o===============o
 BeagleBoneBlack |   YES     |   YES        |     ?        |    ?         |       ?     |    YES         |      ?        |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 RaspberryPI     |   YES     |   YES        |     YES      |    YES       |     YES     |    YES?        |      Nope     |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 Cubieboard2     |   YES     |   YES        |      NO      |    NO?       |       ?     |    YES?        |      Yep      |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 Macos           |           |              |              |              |     YES     |    YES?        | Nope, use dsp |
 ----------------+-----------+--------------+--------------+--------------+-------------+----------------+---------------+
 
 
 
                 |        IMG parsers            |
 ----------------+---------------+---------------+
 Library/ dep    |    libJPEG    |   libPNG      |
 ----------------+---------------+---------------+
 Macro           | HAVE_JPEG_LIB | HAVE_JPEG_LIB |
 ================o===============o===============o
 BeagleBoneBlack |     ?         |     ?         |
 ----------------+---------------+---------------+
 RaspberryPI     |     YES       |     ?         |
 ----------------+---------------+---------------+
 Cubieboard2     |     ?         |     ?         |
 ----------------+---------------+---------------+
 Macos           |     ?         |     ?         |
 ----------------+---------------+---------------+

 
 */


class ScopedPlateformConfig
{
public:
    ScopedPlateformConfig()
    {
        initializePlateform();
    }
    ~ScopedPlateformConfig()
    {
        deInitializePlateform();
    }
    

};



#endif
