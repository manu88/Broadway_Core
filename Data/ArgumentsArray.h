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
    ArgumentsArray( const std::string &argument);
    ArgumentsArray( const char* &argument);
    ArgumentsArray( const float argument);
    ArgumentsArray( const double argument);
    ArgumentsArray( const int argument);
    ArgumentsArray( const bool argument);

    
    ~ArgumentsArray()
    {

    }

    ArgumentsArray& operator<<( float rhs )
    {
        addValue<float>( rhs );
        
        return *this;
    }

    

    
    template<typename Type>
    void addValue(Type value)
    {
        _list.push_back( Variant( value ) );
    }
    
    int getSize() const
    {
        return ( int ) _list.size();
    }
    
    const Variant& getValueAtIndex(const int index) const
    {
        return _list.at(index);
    }
    
    /* *** *** *** *** *** *** *** *** */
    // Specialized methods to ensure explicit convertion
    
    float getFloatValueAtIndex( const int index ) const
    {
        return _list.at(index).getFloat();
    }
    
    /**/
    
    int getIntValueAtIndex( const int index ) const
    {
        return _list.at(index).getInt();
    }
    
    /**/
    
    const std::string getStringValueAtIndex( const int index ) const
    {
        return _list.at(index).getString();
    }
    



    
private:
    
    std::vector< Variant > _list;


};





#endif /* defined(____ArgumentsArray__) */
