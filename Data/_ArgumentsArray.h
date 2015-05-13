//
//  ArgumentsArray.h
//  
//
//  Created by Manuel Deneu on 05/09/14.
//
//

#ifndef ____ArgumentsArray__
#define ____ArgumentsArray__
/*
#include <assert.h>

#include <stdio.h>
#include <typeinfo>
#include <string>
#include <vector>
#include <stdarg.h>
#include <memory>

#include "../Internal/Object.h"
#include "Value.h"




class ArgumentsArray  : public Object
{
private:
    
        
public:


    ArgumentsArray()
    {
        className = "Argument Array";
    }
    
    // simple way to create single Arguments
    ArgumentsArray( const Variant &var);
    
    
    ArgumentsArray( const char* &argument);
    ArgumentsArray( const float argument);
    ArgumentsArray( const double argument);
    ArgumentsArray( const int argument);
    ArgumentsArray( const bool argument);
    
    ArgumentsArray( const void *ptr);

    ArgumentsArray( std::initializer_list< Variant > args)
    {
        for (const Variant &val : args )
            addValue( val );
    }
    
    ~ArgumentsArray()
    {

    }

    // copy & assignment ctors
    
    ArgumentsArray ( const ArgumentsArray &val );
    ArgumentsArray& operator=(ArgumentsArray const& copy);
//    const ArgumentsArray& operator=(ArgumentsArray const& copy) const;

    void addValue( const Variant &value )
    {
        _list.push_back(  value );
    }
    
    int getSize() const
    {
        return ( int ) _list.size();
    }
    
    const Variant* getValueAtIndex(const int index) const
    {
        return &_list.at(index);
    }
    

    
    typedef typename std::vector<Variant>::iterator iterator;
    typedef typename std::vector<Variant>::const_iterator const_iterator;
    
    iterator begin() {return _list.begin();}
    const_iterator begin() const {return _list.begin();}
    const_iterator cbegin() const {return _list.cbegin();}
    iterator end() {return _list.end();}
    const_iterator end() const {return _list.end();}
    const_iterator cend() const {return _list.cend();}
    
private:
    
    std::vector< Variant > _list;


};
*/




#endif /* defined(____ArgumentsArray__) */
