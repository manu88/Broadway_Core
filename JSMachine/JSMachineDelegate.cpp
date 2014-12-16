//
//  JSMachineDelegate.cpp
//  JSMachine
//
//  Created by Manuel Deneu on 02/08/14.
//  Copyright (c) 2014 Manuel Deneu. All rights reserved.
//

#include <assert.h>

#include "JSMachineDelegate.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachineDelegate::JSMachineDelegate()  
{
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** */

JSMachineDelegate::~JSMachineDelegate()
{
    
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
 
    Empty implementation, should never be called!
 
 */
/* **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*virtual*/void JSMachineDelegate::functionCalled( const Selector *selector )
{
    assert(false);
}