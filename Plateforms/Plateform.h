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
    #define EXTERNAL_EXTENTION "br_arm"

#elif defined __APPLE__
    #include "MacOS/MacOS.h"
    #define EXTERNAL_EXTENTION "br_darwin"

#elif defined TARGET_CUBIE2
    #include "CubieBoard/CubieBoard.h"

#elif defined TARGET_BEAGLEBONE_BLACK
    #include "BeagleBoneBlack/BeagleBoneBlack.h"


#endif


/*
 
 According to the targeted plateform, the following will or will not be defined
 
 ----------------+-----------+--------------+--------------+--------------+-------------+
 Library/ dep    | EGL       | Open GL ES 2 |   Open VG    | Open MAX     |   FFMPEG    |
 ----------------+-----------+--------------+--------------+--------------+-------------+
 Macro           | HAVE_EGL  |  HAVE_GLES2  | HAVE_OPENVG  | HAVE_OPENMAX | HAVE_FFMPEG |
 ================o===========o==============o==============o==============o=============o
 BeagleBoneBlack |   YES     |   YES        |     ?        |    ?         |       ?     |
 ----------------+-----------+--------------+--------------+--------------+-------------+
 RaspberryPI     |   YES     |   YES        |     YES      |    YES       |     YES     |
 ----------------+-----------+--------------+--------------+--------------+-------------+
 Cubieboard2     |   YES     |   YES        |      NO      |    NO?       |       ?     |
 ----------------+-----------+--------------+--------------+--------------+-------------+
 Macos           |           |              |              |              |     YES     |
 ----------------+-----------+--------------+--------------+--------------+-------------+
 
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
