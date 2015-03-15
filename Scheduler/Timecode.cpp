//
//  Timecode.c
//  Broadway_test
//
//  Created by Manuel Deneu on 06/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <stdio.h>

#include "Timecode.h"
#include "../Config.h"

/* **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** */
/*
 Operateurs
 */
/* **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** ****  **** **** */
bool operator==(const Timecode& lhs, const Timecode& rhs)
{
    return (    (lhs.h     == rhs.h)
             && (lhs.min   == rhs.min)
             && (lhs.sec   == rhs.sec)
             && (lhs.milli == rhs.milli)
            );
}

bool operator<(const Timecode& lhs, const Timecode& rhs)
{

    return lhs.getInMs() < rhs.getInMs();

    
}

bool operator>(const Timecode& lhs, const Timecode& rhs)
{
    return lhs.getInMs() > rhs.getInMs();
}




bool operator!=(const Timecode& lhs, const Timecode& rhs)
{
    return !(lhs == rhs);
}


bool operator>=(const Timecode& lhs, const Timecode& rhs)
{
    return (!(lhs<rhs)) || (lhs==rhs) ;
}

bool operator<=(const Timecode& lhs, const Timecode& rhs)
{
    return ( (lhs<rhs) || (lhs==rhs) );
}

/* **** **** **** **** **** **** **** **** */


Timecode operator+=(Timecode& lhs, const Timecode& rhs)
{
    lhs.milli += rhs.milli;
    lhs.sec   += rhs.sec;
    lhs.min   += rhs.min;
    lhs.h     += rhs.h;
    
    
    return lhs;
}

Timecode operator-=(Timecode& lhs, const Timecode& rhs)
{
    lhs.milli -= rhs.milli;
    lhs.sec   -= rhs.sec;
    lhs.min   -= rhs.min;
    lhs.h     -= rhs.h;
    
    
    return lhs;
}

Timecode operator-(const Timecode& lhs, const Timecode& rhs)
{
    Timecode r;

    
    if ( rhs>lhs )
        return r;

    const auto diff = lhs.getInMs() -rhs.getInMs();
    
    r.milli = diff;
    
    r.reconstruct();
    
    return r;
}

Timecode operator+(const Timecode& lhs, const Timecode& rhs)
{
    Timecode r( lhs.h , lhs.min , lhs.sec , lhs.milli ) ;
    r.milli += rhs.milli;
    r.sec   += rhs.sec;
    r.min   += rhs.min;
    r.h     += rhs.h;
    
    
    return r;
}

/* **** **** ****  **** **** */


void printTime(Timecode tc)
{
    printf("\n TC_ %lu:%lu:%lu:%lu", tc.h , tc.min , tc.sec , tc.milli);
}



/* **** **** ****  **** **** */
unsigned long getTimeToWaitInMS(Timecode &currentTime , Timecode &tc )
{
    DEBUG_ASSERT(tc>=currentTime);
    
    return (tc - currentTime).getInMs();
}


/* **** **** ****  **** **** */


bool isValidTimecode(Timecode &tc)
{
    
    return tc != InvalidTimeCode;
}
