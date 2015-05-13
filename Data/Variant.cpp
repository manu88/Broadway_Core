//
//  Value.cpp
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//
#include "../Config.h"
#include "../Log/Log.h"
#include "Variant.h"
#include "ValueImpl.h"


#ifdef USE_JAVA_INTERPRETER
#include "../JSMachine/JSMachine.h"
#endif

#ifdef USE_NETWORK
#include <oscpack/osc/OscReceivedElements.h>
#endif


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

Variant::Variant():
_variant ( new Value< void* >(  nullptr  ,ValueImpl::T_NULL ) )
{
    
}


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





Variant::Variant( std::initializer_list< Variant > args) :
_variant ( new Value< VariantList >( args ,ValueImpl::T_LIST ) )
{
    /*
    for (const Variant &val : args )
    {
        
    }
    */
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

#ifdef USE_JAVA_INTERPRETER
Variant::Variant ( CScriptVar* var )
{

    if ( var->isInt() )
        Variant ( var->getInt() );
    
    else if ( var->isBool() )
        Variant ( var->getBool() );
    
    else if ( var->isDouble() )
        Variant ( var->getDouble() );

    else if ( var->isString() )
        Variant ( var->getString() );
    
    else if ( var->isArray() )
    {
        const int size = var->getArrayLength();
        
        for( int i = 0; i< size ; i++)
        {
            
        }
    }
    /*


     bool isNumeric()
     bool isFunction()
     bool isObject() {
     bool isArray() {
     bool isNative() {
     bool isUndefined
     bool isNull()
     bool isBasic()
     */
    
    
    

    
}

#endif

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


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
    _variant->release();

    if (_variant->refCount() <=0 )
    {
        delete _variant;
        _variant = nullptr;
    }

    _variant = copy._variant;
    
    _variant->retain();
    
    return *this;
}
/*
const Variant& Variant::operator=(Variant const& copy) const
{
    printf("\n REF = %i" , _variant->release() );
    
    if (_variant->getType() != copy._variant->getType() )
    {
        printf("\n assign type differ! %i" , _variant->getId() );

    }
    
    _variant = copy._variant;
    
    _variant->retain();
    
    return *this;
}
*/
/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


Variant::~Variant()
{
    if (_variant->release() ==0 )
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

const VariantList& Variant::getList() const
{
    return _variant->getList();
}


const std::vector< uint8_t > Variant::getByteArray() const
{
    return _variant->getByteArray();
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

bool Variant::isList() const
{
    return _variant->isList();
}

bool Variant::isNull() const
{
    return _variant->isNull();
}

bool Variant::isByteArray() const
{
    return _variant->isByteArray();
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
bool Variant::operator==( const Variant& rhs) const
{
    return _variant == rhs._variant;
}

bool Variant::operator==( const void* ptr) const
{
    return getValue<void*>() == ptr;
}

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

std::ostream& operator<<( std::ostream& os, const Variant& val)
{
    return os << val.getString();
}



