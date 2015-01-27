//
//  GXDefs.h
//  Broadway_core
//
//  Created by Manuel Deneu on 27/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_core_GXDefs_h
#define Broadway_core_GXDefs_h

/* *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** */
/*
    This file will define proper symbols according to the plateform you're building on.
    
    In order to avoid a lot of #ifdef in your files, include it directly
 
 */

#ifdef TARGET_RASPBERRY_PI

    #ifndef HAVE_EGL
    #define HAVE_EGL

        #include <EGL/egl.h>

    #endif

    #ifndef HAVE_GLES2
    #define HAVE_GLES2

        #include <GLES/gl.h>

    #endif

    #ifndef HAVE_OPENVG
    #define HAVE_OPENVG

        #include "VG/openvg.h"
        #include "VG/vgu.h"

    #endif

/* end of TARGET_RASPBERRY_PI*/

#elif defined __APPLE__


    #ifndef HAVE_OPENVG
    #define HAVE_OPENVG

        // dummy defs
        #include "../Plateforms/MacOS/MacDummies/vg_mac.h"

    #endif
/* end of __APPLE__ */

#endif


#endif /*Broadway_core_GXDefs_h*/
