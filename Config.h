//
//  Config.h
//  Broadway_test
//
//  Created by Manuel Deneu on 21/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef Broadway_test_Config_h
#define Broadway_test_Config_h




#include <assert.h>

#ifdef BROADWAY_DEBUG
    #define DEBUG_ASSERT(X) assert(X)
#else
    #define DEBUG_ASSERT(X) 
#endif

#endif
