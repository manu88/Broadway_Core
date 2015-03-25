//
//  Value.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#include "Value.h"
#include "ValueImpl.h"

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant( int val):
_variant ( new Value< int >( val ,ValueImpl::T_INT ) )
{

}

Variant::Variant( float val ):
_variant ( new Value< float >( val , ValueImpl::T_FLOAT ) )
{
    
}

Variant::Variant( double val ):
_variant ( new Value< double >( val ,ValueImpl::T_DOUBLE ) )
{
    
}

Variant::Variant(bool val):
_variant ( new Value< bool >( val ,ValueImpl::T_BOOL ) )
{
    
}


Variant::Variant(const std::string &val):
_variant ( new Value< std::string >( val , ValueImpl::T_STRING ) )
{
    
}

Variant::Variant( const char* val ) :
_variant( new Value<std::string> ( std::string( val ) , ValueImpl::T_STRING  ) )
{

}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


Variant::~Variant()
{
    if (_variant->release() <=0 )
    {
        delete _variant;
    }
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

template <typename T>
T Variant::getValue() const
{
    return (reinterpret_cast<const Value< T >*>( _variant) )->getValue() ;
    
}

template <typename T>  bool Variant::isType() const
{
    return typeid( this ) == typeid(T);
}

template unsigned long Variant::getValue< unsigned long >() const;
template bool Variant::isType<unsigned long> () const;

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::ostream& operator<<( std::ostream& os, const Variant& val)
{
    return os << val.getString();
}



