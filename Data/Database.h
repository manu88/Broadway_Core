//
//  Database.h
//  
//
//  Created by Manuel Deneu on 06/09/14.
//
//

#ifndef ____Database__
#define ____Database__

#include <assert.h>
#include <iostream>
#include <vector>
#include <utility>
#include <sstream>
#include <iostream>     
#include <string.h>
//#include <algorithm>
#include <ctype.h>

#include <fstream>

#include "../Config.h"
#include "../Internal/Object.h"
#include "../Internal/FileSystem.h"

#include "StringOperations.h"

#include "../Scheduler/TimeDefs.h"
#include "../Scheduler/Timecode.h"

#include "Value.h"


typedef std::pair<std::string , Variant> DataPair;

class Database : public Object
{
public:
    
    Database()
    {
        className = "Database";
    }
    
    Database( std::initializer_list< std::pair<std::string , std::string> > args)
    {

        for (const std::pair<std::string ,  std::string> &p : args )
        {

            insertValue(p.first, p.second);
            
        }

        
    }

    ~Database();
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    bool parseFile(const std::string &fileName , const char delim);
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    //! Save a Database to a file, using a specitic delimiter.
    /*
        If you overwrite an already existing database file, this method will
        _NOT_ save any comments, nor any page formatting.
     */
    bool saveToFile(const std::string &fileName , const char delim  );
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    std::string getItemNameAtIndex( const int index) const
    {
        return _dataList[index].first;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    

    const Variant getValueAtIndex(const int index) const
    {
        return _dataList[index].second;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    const Variant* getValueForItemName(const std::string &item)/* const*/
    {

        return &( findItemPosition( item)->second );
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    bool addValue( const std::string &name , const Variant &val )
    {
        return setValueForItemName( name, val);
    }
    
    bool setValueForItemName( const std::string &name , const Variant &val )
    {
        auto iter = findItemPosition( name );
        
        if ( iter != _dataList.end() )
        {
            iter->second = val;
            
            return true;
        }
        else
            insertValue( name , val ) ;
        
        return false;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    /*
        Conversions methods, not checked !
        use should use itemExists() to test the item before
     */
    
    inline const std::string getValueForItemNameAsString( const std::string &item ) /* const*/
    {
        return findItemPosition( item )->second.getString();
    }
    
    inline float getValueForItemNameAsFloat(const std::string &item) /*const*/
    {
        return findItemPosition( item )->second.getFloat();
    }
    
    inline int getValueForItemNameAsInt(const std::string &item) /*const*/
    {
        return findItemPosition( item )->second.getInt();
    }
    
    inline bool getValueForItemNameAsBool(const std::string &item) /*const*/
    {
        return findItemPosition( item )->second.getBool();
    }
    
    inline const std::vector< Variant* > getValueForItemNameAsVector( const std::string &item) /*const*/
    {
        std::vector< Variant* > list;
        /*
        std::istringstream f( getValueForItemName<T1>( item ) );
        std::string s;
        
        while (getline(f, s, ' '))
        {
            if ( !s.empty() )
                list.push_back(s);
        }
         */
        return list;
        
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    

    bool itemExists(const std::string &item) const
    {

        for (const auto i : _dataList)
        {
            if ( i.first == item)
                return true;
        }
        return false;
        
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    
    int getSize() const
    {
        return (int) _dataList.size();
    }
    
    void clear()
    {
        _dataList.clear();
        
    }
    
    

    


private:    
    
    void insertValue( const std::string &name , const  Variant &val )
    {
        _dataList.push_back( std::make_pair( name , val ) );
    }
    
    typename std::vector< DataPair >::iterator findItemPosition( const std::string &item )
    {
        return std::find_if( _dataList.begin(), _dataList.end(),  FindItemPredicate(item) );
    }
    
    
    struct FindItemPredicate
    {
        FindItemPredicate( std::string const& s ) : _s(s)
        {}
        
        bool operator () ( DataPair const& p)
        {
            return (p.first == _s);
        }
        
        std::string _s;
    };

    
    std::vector< DataPair > _dataList;
    
    
};





#endif /* defined(____Database__) */
