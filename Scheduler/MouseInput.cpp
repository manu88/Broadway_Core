//
//  MouseInput.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 03/11/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <istream>
#include <unistd.h>

#include "../Plateforms/Plateform.h"

#ifdef TARGET_RASPBERRY_PI
#include <linux/input.h>

#else // dummies

struct input_event
{
    struct timeval time;
    uint16_t type;
    uint16_t code;
    uint32_t value;
};

#endif

#include "MouseInput.h"

MouseEvent::MouseEvent():
m_dX   ( 0 ),
m_dY   ( 0 ),
m_absX ( 0 ),
m_absY ( 0 )
{
    
}

MouseEvent::~MouseEvent()
{
    
}

/* **** **** **** **** **** **** **** **** **** **** **** **** */

MouseListener::MouseListener() :
    m_fd ( -1 ),
    m_deviceInput ( "/dev/input/mice" )
{
    m_lastState.m_dX = 0;
    m_lastState.m_dY = 0;
    m_lastState.m_absX = 0;
    m_lastState.m_absY = 0;
    
    test = 0;
    


}

MouseListener::~MouseListener()
{
    
}

const MouseEvent& MouseListener::getLastState() const
{

    return m_lastState;
}

bool MouseListener::openDevice()
{
    m_fd = -1;
    if( ( m_fd = open(m_deviceInput.c_str() , O_RDONLY)) == -1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool MouseListener::pollInput()
{
    if (!openDevice())
        return false;

    struct input_event ie;
    
    read( m_fd, &ie, sizeof(struct input_event));
    
    int x = 0;
    int y = 0;
    unsigned char button;
    unsigned char bLeft;
    unsigned char bMiddle;
    unsigned char bRight;
    
    unsigned char *ptr = (unsigned char*)&ie;


    button=ptr[0];
    bLeft = button & 0x1;
    bMiddle = ( button & 0x4 ) > 0;
    bRight = ( button & 0x2 ) > 0;
    
    x=(char) ptr[1];
    y=(char) ptr[2];
    
    int xx = 0;
    int yy = 0;
    if (x != 0)
    {
        if (x < 127)
        {
            xx += x;
        }
        else
        {
            xx  -= (255 - x);
        }
    }
    if (y!= 0)
    {
        if (y < 127)
        {
            yy += y;
        }
        else
        {
            yy -= (255-y);
        }
    }
    m_lastState.m_dX = xx;
    m_lastState.m_dY = yy;
    
    m_lastState.m_absX += xx;
    m_lastState.m_absY += yy;

//    printf("\nbLEFT:%d, bMIDDLE: %d, bRIGHT: %d, rx: %i  ry=%i \n",bLeft,bMiddle,bRight, absolute_x, absolute_y);
    

    
    return true;
}


