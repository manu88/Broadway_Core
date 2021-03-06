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

#include "Variant.h"

/**/

typedef std::pair<std::string , Variant> DataPair;



class Database : public Object
{
public:
    
    Database()
    {
        className = "Database";
    }
    
    Database( std::initializer_list< DataPair > args)
    {

        for (const DataPair &p : args )
            insertValue(p.first, p.second);

        
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
    bool saveToFile(const std::string &fileName , const char delim  ) const;
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    std::string getItemNameAtIndex( const int index) const
    {
        return _dataList[index].first;
    }

    const Variant getValueAtIndex(const int index) const
    {
        return _dataList[index].second;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    const Variant* getValueForItemName(const std::string &item) const
    {

        return &( findItemPosition( item)->second );
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



    const VariantList getValueForItemNameAsVector( const std::string &item) const;

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    

    bool itemExists( const std::string &item ) const
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
    
    /* *** *** *** *** *** *** *** *** *** *** *** *** *** */
    
    typedef typename std::vector<DataPair>::iterator iterator;
    typedef typename std::vector<DataPair>::const_iterator const_iterator;
    
    iterator begin() {return _dataList.begin();}
    const_iterator begin() const {return _dataList.begin();}
    const_iterator cbegin() const {return _dataList.cbegin();}
    iterator end() {return _dataList.end();}
    const_iterator end() const {return _dataList.end();}
    const_iterator cend() const {return _dataList.cend();}
    

    

private:    
    
    void insertValue( const std::string &name , const  Variant &val )
    {
        _dataList.push_back( std::make_pair( name , val ) );
    }
    
    //! const find method
    typename std::vector< DataPair >::const_iterator findItemPosition( const std::string &item ) const
    {
        return std::find_if( _dataList.begin(), _dataList.end(),  FindItemPredicate(item) );
    }

    //! find method    
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
