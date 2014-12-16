//
//  ArgumentsArray.cpp
//  
//
//  Created by Manuel Deneu on 05/09/14.
//
//
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <dirent.h>

#include "ArgumentsArray.h"

#include "../Log/Log.h"
#include "../Config.h"


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*
    simple way to create single Arguments
 */
ArgumentsArray::ArgumentsArray( const std::string &argument)
{
    ArgumentsArray();
    addValue< std::string >( argument );
}

ArgumentsArray::ArgumentsArray( const char* &argument)
{
    ArgumentsArray();
    addValue< const char* >( argument );
}

ArgumentsArray::ArgumentsArray( const float argument)
{
    ArgumentsArray();
    addValue< float >( argument );
}

ArgumentsArray::ArgumentsArray( const double argument)
{
    ArgumentsArray();
    addValue< double >( argument );
}

ArgumentsArray::ArgumentsArray( const int argument)
{
    ArgumentsArray();
    addValue< int >( argument );
}

ArgumentsArray::ArgumentsArray( const bool argument)
{
    ArgumentsArray();
    addValue< bool >( argument );
}


/* **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** **** */
/*static*/ std::unique_ptr<ArgumentsArray> ArgumentsArray::getArrayFromFolder( const std::string &folder)
{
    ArgumentsArray* array = new ArgumentsArray();
    
    
    DIR * d = nullptr;
    
    d = opendir ( folder.c_str() );
    
    if (! d)
    {
        Log::log ( "Cannot open directory '%s': %s\n",
                  folder.c_str(), strerror (errno));


    }
    else
    {
        
        int count = 0;
        
        
        while (1)
        {
            struct dirent * entry;
            
            entry = readdir (d);
            
            if (! entry)
            {
                break;
            }
            
            if (   ( strcmp( entry->d_name , "."  ) != 0 )
                && ( strcmp( entry->d_name , ".." ) != 0 )
                )
            {
                //Log::log ("%s\n", entry->d_name);
                array->addValue( std::string(entry->d_name) );
                
                //vars->setArrayIndex( count , new CScriptVar( std::string( entry->d_name ) ) );
                count++;
                
            }
        }
        
        // ABC Sort
        /*
        for ( int i = 0; i < count - 1 ; i++)
        {
            for (int j = i + 1; j < count; j++)
            {
                
                if (strcmp( vars->getArrayIndex( i )->getString( ).c_str() , vars->getArrayIndex( j )->getString( ).c_str()  ) > 0)
                {
                    const std::string temp =  vars->getArrayIndex( i )->getString( );
                    
                    vars->getArrayIndex( i )->setString( vars->getArrayIndex( j )->getString( ) );
                    
                    vars->getArrayIndex( j )->setString( temp );
                }
            }
        }
        */
        // end of sort
        
        
        /* Close the directory. */
        if ( closedir (d) )
        {
            Log::log ("Could not close '%s': %s\n",
                      folder.c_str(), strerror (errno));
        }
    }
    
    return std::unique_ptr<ArgumentsArray>(array);
}




