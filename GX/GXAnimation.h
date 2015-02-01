//
//  GXAnimation.h
//  Broadway_test
//
//  Created by Manuel Deneu on 17/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GXAnimation__
#define __Broadway_test__GXAnimation__

#include <iostream>
#include <vector>

#include "GXElement.h"
#include "../Internal/Object.h"
#include "Animations.h"

class GXAnimation : Object
{
    
public:
    GXAnimation();
    ~GXAnimation();
    
    /* Async Animation design */
    void moveTo( const GXPoint &point , int duration);
    void jumpTo( const GXPoint &point , int duration);
    void moveAndRotateTo( const GXPoint & point , float destAngle , int duration);
    void waitFor( int duration );
    
    /* Anim control */
    bool start( int start_time = 0);
    void pause();
    void stop();
    
private:
    
    /* accessed by GXElement on redraw ( GUI Thread!) */
    GXPoint getNextPointForDt( int dt);
    float   getRZForDT( int dt );
    
    GXElement *_element;
    
    int  _currentTime;
    bool _running;
    std::vector< Animation > _animList;
    
    
};

#endif /* defined(__Broadway_test__GXAnimation__) */
