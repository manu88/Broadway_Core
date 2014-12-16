//
//  MouseInput.h
//  Broadway_test
//
//  Created by Manuel Deneu on 03/11/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__MouseInput__
#define __Broadway_test__MouseInput__

#include "InputEvent.h"
#include "../GXDataType/GXGeometry.h"

class MouseEvent : public InputEvent
{
    friend class MouseListener;
public:
    
    MouseEvent( );
    ~MouseEvent();
    
    GXPoint getDelta() const
    {
        return makePoint(m_dX, m_dY);
    }
    
    GXPoint getPos() const
    {
        return makePoint(m_absX, m_absY);
    }
    
    
private:
    

    int m_dX;
    int m_dY;
    
    int m_absX;
    int m_absY;
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class MouseListener : EventListener
{
public:
    MouseListener();
    ~MouseListener();
    
    const MouseEvent &getLastState() const;
    
    virtual bool pollInput();
    
private:
    bool openDevice();
    int m_fd;
    
    std::string m_deviceInput;
    MouseEvent m_lastState;
    
    int test;
    
};


#endif /* defined(__Broadway_test__MouseInput__) */
/*
 #include <stdio.h>
 #include <stdlib.h>
 #include <fcntl.h>
 #include <linux/input.h>
 #include <istream>
 
 #define MOUSEFILE m_deviceInput = "/dev/input/mice"
 //
 int main()
 {
 int fd;
 struct input_event ie;
 //
 unsigned char button,bLeft,bMiddle,bRight;
 char x,y;
 int absolute_x,absolute_y;
 
 if((fd = open(MOUSEFILE, O_RDONLY)) == -1)
 {
 printf("Device open ERROR\n");
 exit(EXIT_FAILURE);
 }
 else
 {
 printf("Device open OK\n");
 }
 //
 while(read(fd, &ie, sizeof(struct input_event)))
 {
 unsigned char *ptr = (unsigned char*)&ie;
 int i;
 //
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
 //                      printf("Dir X 1 ");
 xx += x;
 }
 else
 {
 //                      printf("dir X -1 ");
 xx  -= (255 - x);
 }
 }
 if (y!= 0)
 {
 if (y < 127)
 {
 //                      printf(" dir Y 1");
 yy += y;
 }
 else
 {
 //                      printf(" dir Y -1");
 yy -= (255-y);
 }
 }
 //
 absolute_x+=xx;
 absolute_y-=yy;
 printf("\nbLEFT:%d, bMIDDLE: %d, bRIGHT: %d, rx: %i  ry=%i \n",bLeft,bMiddle,bRight, absolute_x, absolute_y);
 
 if (bRight==1)
 {
 absolute_x=0;
 absolute_y=0;
 printf("Absolute x,y coords origin recorded\n");
 }
 }
 
 return 0;
 }
 
 */