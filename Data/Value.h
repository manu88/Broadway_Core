//
//  Value.h
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef __Value__
#define __Value__

#include "../Internal/Object.h"


/* This is the public part of Variant/Value implementation */

class ValueImpl;

class Variant
{
public:
    
    Variant( int val );
    Variant( float val );
    Variant( double val );
    Variant( bool val );
    Variant( const std::string &val );
    
    Variant ( const Variant &val );
    Variant& operator=(Variant const& copy);
    
    ~Variant();

    /* get val */
    
    int   getInt() const;
    float getFloat() const;
    double getDouble() const;
    bool  getBool() const;
    const std::string getString() const;
    
    /* test type */
    
    bool isInt() const;
    bool isFloat() const;
    bool isDouble() const;
    bool isBool() const;
    bool isString() const;
protected:
    
    ValueImpl* _variant;
};






#endif /* defined(__MediaCenter__Value__) */
