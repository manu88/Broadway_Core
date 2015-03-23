//
//  GraphicsComponents.cpp
//  Broadway_test
//
//  Created by Manuel Deneu on 22/10/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//
#include <math.h>
#include "GraphicsComponents.h"

#include "../Config.h"


CircleWaitComponent::CircleWaitComponent()
{
    className = "CircleWaitComponent";
    
    m_path = nullptr;
    
    setTransparency( true );
    
        
}

CircleWaitComponent::~CircleWaitComponent()
{

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void CircleWaitComponent::paint( const GXRect &rect , GXAnimation* anim )
{
    DEBUG_ASSERT( m_path != nullptr );

    const GXPoint center = makePoint( getBounds().size.width/2, getBounds().size.height/2);

    m_count ++;

    if (m_count > 5)
    {
        m_count = 0;
        m_animIndex++;
        m_diam += ( m_animDir )*m_diameter/m_segments;
    }

    m_path->clear();
    

    


    
    m_path->setStrokeWidth( m_weight );
    m_path->setJoinStyle( Join_Round );
    m_path->setCapStyle( Cap_Round );
    
    
    int alpha = (255* m_animIndex/ m_segments) +100;
    
    base.a = alpha;
    sel4.a = alpha;
    
    for (int i = 0; i< m_segments ; i++)
    {
        m_path->setStrokeColor( base );
        
        if ( m_animIndex == i )
        {
            m_path->setStrokeColor( sel );
        }
        
        else if ( ( m_animIndex == ((i-1)% m_segments) ) ||  ( m_animIndex == ( (i+1) % m_segments) ))
        {
            m_path->setStrokeColor( sel2 );
        }
        
        else if ( ( m_animIndex == ((i-2)% m_segments) ) ||  (m_animIndex == ( (i+2) % m_segments) ))
        {
            m_path->setStrokeColor( sel3 );
        }
        
        else if ( ( m_animIndex == ((i-3)% m_segments) ) ||  ( m_animIndex == ( (i+3) % m_segments) ))
        {
            m_path->setStrokeColor( sel4 );
        }
        
        m_start = i* m_rad + m_offset;
        float end  =  m_rad - m_offset;
        
        //arc(200, 200, diam, diam, start,  end);
        m_path->addArc( center , makeSize(m_diam, m_diam), m_start,end , Arc_Open);
        m_path->stroke();        
    }
    

    

    
    if ( m_animIndex >= m_segments )
        m_animIndex = 0;
    
    
    if ( m_diam < 0.1)
    {
        if ( m_waitIndex < m_waitTime)
        {
            m_animDir = 0;
            m_waitIndex++;
        }
        else
        {
            m_animDir = 1;
            m_waitIndex = 0;
        }
    }
    
    if ( m_diam > m_diameter)
    {
        if ( m_waitIndex < m_waitTime)
        {
            m_animDir = 0;
            m_waitIndex++;
        }
        else 
        {
            m_animDir = -1;
            m_waitIndex = 0;
        }
    }

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void CircleWaitComponent::prepareRessources( )
{
    m_diameter = 100;
    const GXRect bounds = makeRect( getParentElement()->getBounds().size.width /2 - (m_diameter/2)-10,
                                    getParentElement()->getBounds().size.height/2 - (m_diameter/2)-10,
                                    m_diameter+20,
                                    m_diameter+20
                                   );
    setBounds( bounds );
    
    if (m_path == nullptr)
        m_path = new GXPath( getBounds() );
    
    m_path->shouldClearAfterRenderOps( true );
    
    m_segments = 10;
    
    m_weight = 7;
    
    m_rad = ( 2 * M_PI ) / m_segments;
    m_offset = ( 2 * M_PI ) / ( m_segments*2 );
    
    m_start = 0; // param
    m_diam = m_diameter;
    m_animDir = -1;
    m_waitIndex = 0;
    m_waitTime = 12;
    m_animIndex = 0;
    
    
    base = makeColor(255,255,255);
    sel  = makeColor(0,0,255);
    sel2  = makeColor(100,100,255);
    sel3  = makeColor(200,200,255);
    sel4  = makeColor(250,250,255);
    
    m_count = 0;
    
    

    

}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void CircleWaitComponent::deleteRessources()
{
    if (m_path != nullptr)
        delete m_path;
    
    m_path = nullptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

void CircleWaitComponent::changed()
{
    
    
}
