//
//  Utils.cpp
//  Broadway_core
//
//  Created by Manuel Deneu on 30/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "Utils.h"


bool Utils::checkLittleEndian()
{
    const unsigned int lilEndianTest = 1;

    return (((unsigned char *)&lilEndianTest)[0] == 1);
}

bool Utils::checkBigEndian()
{
    return !checkLittleEndian();
}