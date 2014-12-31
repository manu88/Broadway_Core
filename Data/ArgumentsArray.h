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

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

class BaseArg : public Object
{
public:
    BaseArg()
    {
        className = "BaseArg";
    }
    virtual ~BaseArg()
    {}
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */


class ArgumentsArray  : public Object
{
private:
    
    /* **** **** **** **** **** **** **** **** **** **** */
    
    template <typename T>
    class Argument : public BaseArg
    {
    public:
        
        Argument(T value) :
        m_data( value )
        {
            className = "ArgumentsArray";            
        }
        
        ~Argument()
        {
        }
        
        T getValue() const
        {
            return m_data;
        }
        
        
        template<typename Type>
        bool isType() const
        {
            return ( typeid( m_data ) == typeid ( Type ) );
        }
        
        
    private:
        T m_data;
    };
    
    /* **** **** **** **** **** **** **** **** **** **** */    
    
public:
    
    /* Conversions methods*/
    static std::unique_ptr<ArgumentsArray> getArrayFromFolder( const std::string &folder);


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

        for (auto it : m_list )
            delete it;
    }

    ArgumentsArray& operator<<( float rhs )
    {
        addValue<float>( rhs );
        
        return *this;
    }

    

    
    template<typename Type>
    void addValue(Type value)
    {
        m_list.push_back( new Argument<Type>( value ) );
    }
    
    int getSize() const
    {
        return ( int ) m_list.size();
    }
    
    template<typename Type>
    Type getValueAtIndex(const int index) const
    {
        Argument<Type> *val = dynamic_cast< Argument<Type>* >( m_list[index] );
        
        return val->getValue();

    }
    
    /* *** *** *** *** *** *** *** *** */
    // Specialized methods to ensure explicit convertion
    
    float getFloatValueAtIndex( const int index ) const
    {
        if ( isType<float>( index ) )
            return getValueAtIndex< float >( index );

        else if ( isType< int >( index ) )
            return static_cast < float >( getValueAtIndex< int >( index ));
        
        // add specific cast
        assert( false );
        
        return 0.;
    }
    
    /**/
    
    int getIntValueAtIndex( const int index ) const
    {
        if ( isType< int >( index ) )
            return getValueAtIndex< int >( index );
        
        else if ( isType< float >( index ) )
            return static_cast < int >( getValueAtIndex< float >( index ));
        
        // add specific cast
        assert( false );
        
        return 0;
    }
    
    /* *** *** *** *** *** *** *** *** */
    // Test variable type
    
    template<typename Type>
    bool isType(const int index) const
    {
        // cast will fail if not from tested type
        return ( dynamic_cast< Argument<Type>* >( m_list[index] ) ) != nullptr;

    }


    
private:
    
    std::vector<BaseArg*> m_list;


};





#endif /* defined(____ArgumentsArray__) */
