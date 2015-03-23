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

class Variant;
typedef std::vector<Variant> VariantList;

class Variant
{
public:
    
    Variant( int val );

    
    Variant( float val );
    Variant( double val );

    Variant( const std::string &val );

    // Added to prevent litterals c—strings from being implicitly converted to bool
    Variant( const char* val );
    
    Variant( bool val );
    
    /* copy & assignment ctors */
    
    Variant ( const Variant &val );
    Variant& operator=(Variant const& copy);
    
    ~Variant();
    
    friend std::ostream& operator<<( std::ostream& os, const Variant& val );

    /* get val */
    
    int    getInt() const;
    float  getFloat() const;
    double getDouble() const;
    bool   getBool() const;
    const  std::string getString() const;

    
    
    //! careful! The type will not be checked, and reinterpret_cast may fail!
    template <typename T> T getValue() const;
    
    template <typename T>  bool isType() const;
    
    /* test type */
    
    bool isInt() const;
    bool isFloat() const;
    bool isDouble() const;
    bool isBool() const;
    bool isString() const;

    

protected:
    
    ValueImpl* _variant;
};

std::ostream& operator<<( std::ostream& os, const Variant& val);







#endif /* defined(__MediaCenter__Value__) */
