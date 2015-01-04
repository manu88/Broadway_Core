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
#include <algorithm>
#include <ctype.h>

#include "../Config.h"
#include "../Internal/Object.h"

#include "StringOperations.h"

template <typename T1 >
class Database : public Object
{
public:
    
    Database()
    {
        className = "Database";
    }
    
    ~Database()
    {
        
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    bool parseFile(const std::string &fileName , const char delim)
    {

        FILE* file = fopen(fileName.c_str() , "r");
        
        if (!file)
            return false;
        
        char line[256];
        
        int count = 0;
        while (fgets(line, sizeof(line), file))
        {
            std::stringstream ss(line);
            std::string item = "";
            std::string value = "";
            
            if ( strncmp(line, "#", 1 ) != 0 ) // skipped if begin with '#' ( commantary )
            {

                std::getline(ss, item, delim);
                std::getline(ss, value, delim);

                if ( !isEmpty( item) && !isEmpty( value )) // acceptable pair item/value, ie. not empty
                {
                    bool overwrite = true;
                    

                    if ( item.find("+") != std::string::npos )
                    {

                        
                        item.erase(item.end() -1);
                        
                        overwrite = false;
                    }
                    if (delim != ' ' )
                    {
                        item.erase( remove_if(  item.begin(), item.end(), ::isspace ), item.end());
                        
                        /* trim value, ie remove leading & trailing spaces */
                        value = StringOperations::trim( value , " \t\n" );

                        if (!contains(value, "\""))
                        {
                        }
                        
                        else // quoted token
                        {
                            // si un seul " trouvé et dans la premiere moité de la chaine, considérée comme premier,
                            // sinon considéré comme second
                            const std::string::size_type halfSize = value.size() / 2;
                            
                            std::string::size_type first  = 0;

                            first = value.find( "\"", first );

                            std::string::size_type second = first + 1 ;
                            
                            second = value.find( "\"", second );

                            if ( second == std::string::npos)
                            {
                                const std::string::size_type pos = first;
                                if ( pos < halfSize)
                                {
                                    value.append("\"");
                                    second = value.size() -1;
                                }
                                else
                                {
                                    second = pos;
                                    first = 0;
                                    value.insert(0, "\"");
                                }
                                    
                            }

                            value.erase( value.begin() + second  , value.end() );
                            value.erase( value.begin()           , value.begin() +  first +1 );

                        }
                    }
                                        
                    if (value != "" ) // valid ( &nice ) pair
                    {
                        if ( itemExists(item)) // existant
                        {

                            
                            auto pos = findItemPosition( item );
                            std::string oldVal = getValueForItemName<std::string>(item);

                            if ( pos != m_dataList.end() )
                            {
                                m_dataList.erase( pos);
                                count--;
                                
                                if (!overwrite) // on écrase la valeur
                                {


                                    value += " " + oldVal;
                                    
                                }
                            }

                        }
                        
                        count++;
                        addValue( item , value);
                       
                        
//                        printf("\n add value '%s' for '%s'" , value.c_str() , item.c_str());
                    }
                }

            }
            
        }
        
        if ( count != (int) m_dataList.size() )
        {
            /*list size and iter count don't match */
            DEBUG_ASSERT( false );

        }

        fclose(file);
        
        return true;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    std::string getItemNameAtIndex( const int index) const
    {
        return m_dataList[index].first;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    template<typename T>
    T getValueAtIndex(const int index) const
    {
        return m_dataList[index].second;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    
    template<typename T>
    T getValueForItemName(const std::string &item) const
    {

        for (const auto i : m_dataList)
        {
            if ( i.first == item)
                return i.second;
        }
        
        return 0;
    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    /*
        Conversions methods, not checked !
        use should use itemExists() to test the item before
     */
    
    inline int getValueForItemNameAsInt(const std::string &item) const
    {
        return atoi( getValueForItemName<T1>( item ).c_str() );
    }
    
    inline bool getValueForItemNameAsBool(const std::string &item) const
    {
        return static_cast<bool>( atoi( getValueForItemName<T1>( item ).c_str() ) );
    }
    
    inline const std::vector<T1> getValueForItemNameAsVector( const std::string &item) const
    {
        std::vector< std::string> list;
        
        std::istringstream f( getValueForItemName<T1>( item ) );
        std::string s;
        
        while (getline(f, s, ' '))
        {
            if ( !s.empty() )
                list.push_back(s);
        }
        return list;
        
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    

    bool itemExists(const std::string &item) const
    {

        for (const auto i : m_dataList)
        {
            if ( i.first == item)
                return true;
        }
        return false;
        
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    
    int getSize() const
    {
        return (int) m_dataList.size();
    }
    
    void clear()
    {
        m_dataList.clear();
        
    }
    
    
private:
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    // specialized methods only! see cpp file
    void addValue( const std::string &item, const std::string  &value )
    {
        /*should not be called, specialization ony ... */
        assert( false );

    }

    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    // check if token only contains \t \n ' ' '\r' ( isspace )
    inline bool isEmpty( const std::string &token)
    {
        return std::all_of( token.begin() , token.end() , isspace );
    }
    
    inline bool contains( const std::string &token , const std::string &toFind)
    {
        return token.find( toFind ) != std::string::npos ;
    }
    
    /* **** **** **** **** **** **** **** **** **** **** **** **** **** */
    
    typename std::vector< std::pair<std::string , T1> >::iterator findItemPosition( const std::string &item )
    {
        return std::find_if( m_dataList.begin(), m_dataList.end(),  FindItemPredicate(item) );
    }
    
    
    struct FindItemPredicate
    {
        FindItemPredicate( std::string const& s ) : _s(s)
        {}
        
        bool operator () (std::pair<std::string, T1> const& p)
        {
            return (p.first == _s);
        }
        
        std::string _s;
    };

    
    std::vector< std::pair<std::string , T1> > m_dataList;
    
    
};


template<> void Database< std::string >::addValue( const std::string &item, const std::string  &value );
template<> void Database< int         >::addValue( const std::string &item, const std::string  &value );

#endif /* defined(____Database__) */
