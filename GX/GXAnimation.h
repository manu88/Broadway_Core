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

#include "GXElement.h"

class GXAnimation : public GXElement
{
    
public:
    GXAnimation();
    ~GXAnimation();
    
protected:
    virtual void paint( const GXRect &rect );
    virtual void prepareRessources();
    virtual void deleteRessources();
    virtual void changed();

private:
    
    
    
};

#endif /* defined(__Broadway_test__GXAnimation__) */
