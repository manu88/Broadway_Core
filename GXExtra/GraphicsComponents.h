//
//  GraphicsComponents.h
//  Broadway_test
//
//  Created by Manuel Deneu on 22/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_test__GraphicsComponents__
#define __Broadway_test__GraphicsComponents__

#include "../GX/GXElement.h"
#include "../GX/GXPath.h"

class CircleWaitComponent : public GXElement
{
public:
    CircleWaitComponent();
    ~CircleWaitComponent();
    
    
    
private:
    
    void paint( const GXRect &rect , GXAnimation* anim );
    void prepareRessources();
    void deleteRessources();
    
    virtual void changed();

    GXPath *m_path;
    

    
    int   m_segments;
    int   m_diameter;
    int   m_weight;
    float m_rad;
    float m_offset;
    
    float m_start;
    float m_diam;
    int   m_animDir;
    int   m_waitIndex;
    int   m_waitTime;
    int   m_animIndex;
    
    int m_count;
    
    
    GXColor base;
    GXColor sel;
    GXColor sel2;
    GXColor sel3;
    GXColor sel4;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class MousePointer : public GXElement
{
public:
    MousePointer();
    ~MousePointer();
private:
    virtual void paint( const GXRect &rect );
    virtual void prepareRessources();
    virtual void deleteRessources();
    
    virtual void changed();
    
    GXPath *m_path;    
};

#endif /* defined(__Broadway_test__GraphicsComponents__) */
