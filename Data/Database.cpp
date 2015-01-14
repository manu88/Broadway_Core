//
//  Database.cpp
//  
//
//  Created by Manuel Deneu on 06/09/14.
//
//

#include "Database.h"



template<>
void Database<std::string>::addValue( const std::string &item, const std::string  &value )
{
    _dataList.push_back( std::make_pair( item , value ) );
}

template<>
void Database< int >::addValue( const std::string &item, const std::string  &value )
{
    int val = atoi(value.c_str() );
    
    _dataList.push_back( std::make_pair( item , val ) );
}


