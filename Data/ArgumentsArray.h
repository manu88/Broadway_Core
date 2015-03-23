//
//  ArgumentsArray.h
//  
//
//  Created by Manuel Deneu on 05/09/14.
//
//

#ifndef ____ArgumentsArray__
#define ____ArgumentsArray__
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
    
    //! Will return a list from the content of a specified folder.
    /*!
     \param   A filder's path.
     \return  A scoped pointer with the list (ABC sorted) of files and/or folders.
     */
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
//    static std::unique_ptr<ArgumentsArray> getArrayFromFolder( const std::string &folder);


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

    //! Create an ArgumentArray with an initializer list.
    /*
     Use this when returning an ArgumentsArray or passing it to a method
     { 1,12.1,true,"foo"}
     is eq to
     ArgumentArray array;
     array.addValue(1);
     array.addValue(12.1);
     array.addValue(true);
     array.addValue("foo");
     
     
     */
    ArgumentsArray( std::initializer_list< Variant > args)
    {
        for (const Variant &val : args )
            addValue( val );
    }
    
    ~ArgumentsArray()
    {

    }

    /*
    ArgumentsArray& operator<<( float rhs )
    {
        addValue<float>( rhs );
        
        return *this;
    }
*/

    void addValue( const Variant &value )
    {
        _list.push_back(  value );
    }
    
    int getSize() const
    {
        return ( int ) _list.size();
    }
    
    const Variant& getValueAtIndex(const int index) const
    {
        return _list.at(index);
    }
    
private:
    
    std::vector< Variant > _list;


};





#endif /* defined(____ArgumentsArray__) */
