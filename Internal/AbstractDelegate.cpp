//
//  AbstractDelegate.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 04/04/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "AbstractDelegate.h"


AbstractDelegate::AbstractDelegate() :
_ready ( false ) 
{
    
}


AbstractDelegate::~AbstractDelegate()
{
    
}



void AbstractDelegate::setReady( bool ready )
{
    _ready = ready;
}