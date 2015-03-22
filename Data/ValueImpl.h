//
//  ValueImpl.h
//  MediaCenter
//
//  Created by Manuel Deneu on 22/03/15.
//  Copyright (c) 2015 Manuel Deneu. All rights reserved.
//

#ifndef _ValueImpl_h
#define _ValueImpl_h

class ValueImpl : public Object
{
public:
    
    int retain()
    {
        return ++_refCount;
    }
    
    int release()
    {

        return --_refCount;
    }
    
    
    virtual ~ValueImpl()
    {}
    
    virtual bool        getBool() const = 0;
    virtual int         getInt() const = 0;
    virtual float       getFloat() const = 0;
    virtual double       getDouble() const = 0;
    virtual std::string  getString() const = 0;
    
    virtual bool isInt() const = 0;
    virtual bool isFloat() const = 0;
    virtual bool isDouble() const = 0;
    virtual bool isString() const = 0;
    virtual bool isBool() const = 0;
protected:
    
    int _refCount;
    
    ValueImpl()
    {
        _refCount = 1;
        className = "ValueImpl";
    }
};

//! Template Class for arguments stored in ArgumentsArray
template <typename T>
class Value : public ValueImpl
{
public:
    
    /* Values factory */
    
    static Value<std::string>* value( const std::string &val)
    {
        return new Value<std::string>( val );
    }
    
    static Value<int>* value( int val)
    {
        return new Value< int >( val );
    }
    
    static Value< float >* value( float val)
    {
        return new Value< float >( val );
    }
    
    static Value< double >* value( double val)
    {
        return new Value< double >( val );
    }
    
    static Value< bool >* value( bool val)
    {
        return new Value< bool >( val );
    }
    
    /**/
    
    Value(T value) :
    _data( value )
    {
    //    className = "Value";
    }
    
    ~Value()
    {
    }
    
    const T getValue() const
    {
        return _data;
    }
    
    void setValue( T val)
    {
        _data = val;
    }
    
    bool isInt() const
    {
        return isType<int>();
    }
    
    bool isFloat() const
    {
        return isType<float>();
    }
    
    bool isDouble() const
    {
        return isType<double>();
    }
    
    bool isString() const
    {
        return isType<std::string>();
    }
    
    bool isBool() const
    {
        return isType<bool>();
    }
    
    
    /* Get value */
    
    std::string getString() const
    {
        if ( isString() )
            return (reinterpret_cast<const Value<std::string>*>(this) )->getValue();
        
        else if ( isFloat() )
            return std::to_string( getFloat() );
        
        else if ( isDouble() )
            return std::to_string( getDouble() );
        
        else if ( isBool() )
            return std::to_string( getBool() );
        
        else if ( isInt() )
            return std::to_string( getInt() );
        
        
        return "undefined";
    }
    
    bool getBool() const
    {
        if (isBool() )
            return (reinterpret_cast<const Value< bool >*>(this) )->getValue();
        
        return false;
    }
    
    int getInt() const
    {
        if (isInt() )
            return (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else if ( isString() )
            return std::stoi( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
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
        
        else if ( isString() )
            return std::stof( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() )
            return (float) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    double getDouble() const
    {
        if (isDouble() )
            return (reinterpret_cast<const Value< double >*>(this) )->getValue();
        
        else if ( isString() )
            return std::stod( (reinterpret_cast<const Value<std::string>*>(this) )->getValue() );
        
        else  if (isInt() )
            return (double) (reinterpret_cast<const Value< int >*>(this) )->getValue();
        
        else  if (isFloat() )
            return (float) (reinterpret_cast<const Value< float >*>(this) )->getValue();
        
        return 0.f;
        
    }
    
    template<typename Type>
    bool isType() const
    {
        return ( typeid( _data ) == typeid ( Type ) );
    }
    
    
private:
    T _data;
};



#endif
