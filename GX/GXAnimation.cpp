//
//  GXAnimation.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 17/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include "GXAnimation.h"


GXAnimation::GXAnimation() :
_element     ( nullptr ),
_currentTime ( 0       ),

_running     ( false )
{
    
}

GXAnimation::~GXAnimation()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAnimation::moveTo( const GXPoint &point , int duration)
{
    Animation anim;
    anim._flags = Anim_MoveTo;

    anim._tX = point.x;
    anim._tY = point.y;
    
    anim._duration = duration;
    
    _animList.push_back( anim );

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAnimation::jumpTo( const GXPoint &point , int duration)
{
    Animation anim;
    anim._flags = Anim_JumpTo;
    
    anim._tX = point.x;
    anim._tY = point.y;
    
    anim._duration = duration;
    
    _animList.push_back( anim );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAnimation::moveAndRotateTo( const GXPoint & point , float destAngle , int duration)
{

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

void GXAnimation::waitFor( int duration )
{
    Animation anim;
    
    anim._duration = duration;
    
    _animList.push_back( anim );
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

bool GXAnimation::start( int start_time )
{
    if (_element == nullptr )
    {
        printf("\n ERROR : no GXElement attached to this Animation...");
        return false;
    }
    
    return true;
}

void GXAnimation::pause()
{
    
}

void GXAnimation::stop()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

GXPoint GXAnimation::getNextPointForDt( int dt)
{
    return makePointNULL();
}

float GXAnimation::getRZForDT( int dt )
{
    return 0.0f;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */




