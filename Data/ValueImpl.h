//
//  ValueImpl.h
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef _ValueImpl_h
#define _ValueImpl_h

#include <sstream>

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/* Bas class for Value*/

class ValueImpl : public Object
{
    friend class Variant;
public:
    
    enum {
        T_NULL ,
        T_BOOL ,
        T_INT ,
        T_FLOAT ,
        T_DOUBLE ,
        T_STRING ,
        T_LIST,
        T_BYTES

         };
    
    int retain()
    {
        return ++_refCount;
    }
    
    int release()
    {
        return --_refCount;
    }
    
    int refCount() const
    {
        return _refCount;
    }
    
    
    virtual ~ValueImpl()
    {}
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    virtual bool               getBool() const = 0;
    virtual int                getInt() const = 0;
    virtual float              getFloat() const = 0;
    virtual double             getDouble() const = 0;
    virtual const std::string  getString() const = 0;
    virtual const VariantList  &getList() const = 0;
    virtual       VariantList  &getList() = 0;
    
    virtual const std::vector< uint8_t > getByteArray() const = 0;
    
    /* **** **** **** **** **** **** **** **** **** **** **** */
    
    bool isInt() const
    {
        return _type == T_INT;
    }
    
    bool isFloat() const
    {
        return _type == T_FLOAT;
    }
    
    bool isDouble() const
    {
        return _type == T_DOUBLE;
    }
    bool isString() const
    {
        return _type == T_STRING;
    }
    bool isBool() const
    {
        return _type == T_BOOL;
    }
    
    bool isList() const
    {
        return _type == T_LIST;
    }
    
    bool isNull() const
    {
        return _type == T_NULL;
    }
    
    bool isByteArray() const
    {
        return _type == T_BYTES;
    }
    
    int getType() const
    {
        return _type;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** */

protected:

    int _type;
    
    int _refCount;
    
    ValueImpl( int type ): _type( type )
    {
        _refCount = 1;
        className = "ValueImpl";
    }
    
    ValueImpl( const ValueImpl &other) = delete;
    
};

/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** */

template <typename T>
class Value : public ValueImpl
{
public:
        
    /**/
    
    Value(T value , int type) :
    ValueImpl(type),
    _data( value )
    {
    //    className = "Value";
    }
    
    Value(const Value &other) = delete;
    
    ~Value()
    {
    }
    
    const T &getValue() const
    {
        return _data;
    }
    
    T &getValue()
    {
        return _data;
    }
    
    
    VariantList  &getList()
    {
        return ( reinterpret_cast< Value < VariantList > * >( this ) )->getValue();
    }



    void setValue( T val)
    {
        _data = val;
    }

    
    /* Get value */
    
    const std::string getString() const
    {
        if ( isString() )
            return (reinterpret_cast<const Value< std::string >*>(this) )->getValue();
        
        else if ( isFloat() )
            return std::to_string( (reinterpret_cast<const Value< float >*>(this) )->getValue() );
        
        else if ( isDouble() )
            return std::to_string( (reinterpret_cast<const Value< double >*>(this) )->getValue() );
        
        else if ( isBool() )
            return std::to_string( (reinterpret_cast<const Value< bool >*>(this) )->getValue() );
        
        else if ( isInt() )
            return std::to_string( (reinterpret_cast<const Value< int >*>(this) )->getValue() );
        
        
        
        return "undefined";
    }
    
    bool getBool() const
    {
        if (isBool() )
            return (reinterpret_cast<const Value< bool >*>(this) )->getValue();
        
        
        else if (isInt() )
            return (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else if ( isString() )
            return getIntFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isFloat() )
            return (int) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        else  if (isDouble() )
            return (int) (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        return false; // interpret as int
    }
    
    int getInt() const
    {
        if (isInt() || isBool() )
            return (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        
        else if ( isString() )
            return getIntFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isFloat() )
            return (int) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        else  if (isDouble() )
            return (int) (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        return 0;
    }
    
    float getFloat() const
    {
        if (isFloat() )
            return (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        if (isDouble() )
            return (float)(reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        else if ( isString() )
            return getFloatFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() || isBool() )
            return (float) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    double getDouble() const
    {
        if (isDouble() )
            return (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        else if ( isString() )
            return getDoubleFromStr( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() || isBool() )
            return (double) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else  if (isFloat() )
            return (float) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    const VariantList  &getList() const
    {
        return (reinterpret_cast<const Value< VariantList >*>(this) )->getValue();
    }
    
    const std::vector< uint8_t > getByteArray() const
    {
        std::vector< uint8_t> ret;
        
        if (isString() )
        {
            for (const auto &c : getString() )
            {
                ret.push_back( ( uint8_t ) c);
            }
        }
        return ret;
    }



    template<typename Type>
    bool isType() const
    {
        return ( typeid( _data ) == typeid ( Type ) );
    }
    
private:
    inline const std::vector<Variant> breakInList(/* const ValueImpl* val*/) const
    {
        std::vector< Variant > list;
        
        std::istringstream f( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        std::string s;
        
        while (getline(f, s, ' '))
        {
            if ( !s.empty() )
                list.push_back( s );
        }
        
        return list;
    }
    
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static int getIntFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stoi( str );
                             
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static float getFloatFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stof( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    /* *** *** *** *** *** *** *** *** *** */
    
    inline static double getDoubleFromStr( const std::string &str) noexcept
    {
        try
        {
            return std::stod( str );
            
        } catch (const std::invalid_argument& err)
        {
            return 0;
        }
    }
    
    T _data;
};





#endif
