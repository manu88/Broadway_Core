//
//  Value.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "Value.h"
#include "ValueImpl.h"

Variant::Variant( int val):
_variant ( Value< int >::value(val ) )
{

}

Variant::Variant( float val ):
_variant ( Value< float >::value(val ) )
{
    
}

Variant::Variant( double val ):
_variant ( new Value< double >( val ) )
{
    
}

Variant::Variant(bool val):
_variant ( Value<bool>::value(val ) )
{
    
}


Variant::Variant(const std::string &val):
_variant ( Value< std::string >::value(val ))
{
    
}

Variant::Variant ( const Variant &val ):
_variant ( val._variant)
{
    _variant->retain();
}

Variant& Variant::operator=(Variant const& copy)
{
    _variant = copy._variant;
    
    _variant->retain();
    
    return *this;
}


Variant::~Variant()
{
    if (_variant->release() <=0 )
    {
        delete _variant;
    }
}

int Variant::getInt() const
{
    return _variant->getInt();
}


float Variant::getFloat() const
{
    return _variant->getFloat();
}

double Variant::getDouble() const
{
    return _variant->getDouble();
}

bool Variant::getBool() const
{
    return _variant->getBool();
}
const std::string Variant::getString() const
{
    return _variant->getString();
}


bool Variant::isInt() const
{
    return _variant->isInt();
}

bool Variant::isFloat() const
{
    return _variant->isFloat();
}

bool Variant::isDouble() const
{
    return _variant->isDouble();
}

bool Variant::isBool() const
{
    return _variant->isBool();
}

bool Variant::isString() const
{
    return _variant->isString();
}



