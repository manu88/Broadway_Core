//
//  DisplayImpl.h
//  Broadway_core
//
//  Created by Manuel Deneu on 27/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__DisplayImpl__
#define __Broadway_core__DisplayImpl__

#include <iostream>

#include "../../Internal/Object.h"

class DisplayController; // forward


class DisplayImpl : public Object
{
    friend class DisplayController;
    
private:
    
    DisplayImpl();
    ~DisplayImpl();
    
    bool init();
    bool deInit();
    
    bool _initOk;
    
};

#endif /* defined(__Broadway_core__DisplayImpl__) */
