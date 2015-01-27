//
//  TARGET_RASPBERRY_PIberryPI.h
//  Broadway_test
//
//  Created by Manuel Deneu on 23/09/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_RaspberryPI_h
#define Broadway_RaspberryPI_h

// gdb : handle SIGILL nostop
//       thread apply all backtrace

#include <bcm_host.h>



static void initializePlateform()
{
    bcm_host_init();    
}


static void deInitializePlateform()
{

}

#endif
