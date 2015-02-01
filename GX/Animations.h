//
//  Animations.h
//  Broadway_core
//
//  Created by Manuel Deneu on 01/02/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__Animations__
#define __Broadway_core__Animations__

#include <iostream>
#include "../Internal/Object.h"

typedef enum
{
    Anim_None = 0,              /* No animation  */
    Anim_MoveTo   = ( 1 << 0),  /* translation   */
    Anim_JumpTo   = ( 1 << 1),  /* pos. jumps to */
    
    Anim_RotateTo = ( 1 << 4 ), /* rotation      */
    
} Anim_flag;

/* **** **** **** **** **** */

class Animation 
{
    friend class GXAnimation;
    
public:
    
    Animation();
    
    ~Animation();
    
private:
    
    Anim_flag _flags;
    
    int _duration;
    
    int _tX;
    int _tY;
    int _rZ;
    
    
};


#endif /* defined(__Broadway_core__Animations__) */
