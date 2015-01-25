//
//  Memory.h
//  Broadway_core
//
//  Created by Manuel Deneu on 17/01/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Broadway_core__Memory__
#define __Broadway_core__Memory__

namespace Memory
{

    void fillBufferFloat(const float* source, float* dest , unsigned int num);
    
    void fillBufferCustom(const void* source , void* dest , unsigned int size );
    
} // Memory

#endif /* defined(__Broadway_core__Memory__) */
