//
//  AUElement.h
//  
//
//  Created by Manuel Deneu on 16/08/14.
//
//

#ifndef ____AUElement__
#define ____AUElement__

#include <assert.h>
#include <iostream>
#include "../Internal/Element.h"



/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class AUElement : public virtual Element
{
public:
    
    AUElement();
    virtual ~AUElement();

    virtual void process( const float **inputs , float **outputs, const int numSamples) = 0;
    
    
protected:

private:
    
};

#endif /* defined(____AUElement__) */
